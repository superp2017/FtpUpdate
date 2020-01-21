#include "update.h"
#include "ui_update.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QFileInfo>
#include <QProcess>
#include <QCoreApplication>
#include <QDate>
#include <QProcess>
#include <QStringList>
#include <QRegExp>
#include <QToolTip>
#include "ftpconfigsetting.h"
#include "boost/thread.hpp"

#pragma execution_character_set("utf-8")
Update::Update(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Update)
{
    ui->setupUi(this);
    connect(this,SIGNAL(updateValue(bool,double)),this,SLOT(updateProgress(bool,double)));
    connect(&mCheck,SIGNAL(startUpdate(bool)),this,SLOT(startDownload(bool)));
    connect(this,SIGNAL(downLoadRet(int)),this,SLOT(downloadCallback(int)));
    connect(this,SIGNAL(upLoadRet(int)),this,SLOT(uploadCallback(int)));


    mConfig.loadConfig();
    initView();
    m_isBackGround = false;
}

Update::~Update()
{
    delete ui;
}

void Update::initView()
{
    ui->lineEdit_userName->setText(mConfig.ftpConfig().User);
    ui->lineEdit_passwd->setText(mConfig.ftpConfig().Code);
    ui->lineEdit_server->setText(mConfig.ftpConfig().Host);
    ui->lineEdit_port->setText(mConfig.ftpConfig().Port);

    ui->dateEdit_local->setDate(QDate::currentDate());
    QMenu *menu= this->menuBar()->addMenu("设置");
    QAction*set = menu->addAction("更新配置");
    connect(set,SIGNAL(triggered(bool)),this,SLOT(showftpSetting()));
}


void Update::on_pushButton_connect_clicked()
{
    QString host = ui->lineEdit_server->text().trimmed();
    QString port = ui->lineEdit_port->text().trimmed();
    QString user = ui->lineEdit_userName->text().trimmed();
    QString code = ui->lineEdit_passwd->text().trimmed();
    if(host.isEmpty()){
        QToolTip::showText(ui->lineEdit_server->mapToGlobal(QPoint(100, 0)), "主机名不能为空!");
        return;
    }
    if(port.isEmpty()){
        QToolTip::showText(ui->lineEdit_port->mapToGlobal(QPoint(100, 0)), "端口不能为空!");
        return;
    }
    if(user.isEmpty()){
        QToolTip::showText(ui->lineEdit_userName->mapToGlobal(QPoint(100, 0)), "用户名不能为空!");
        return;
    }
    if(code.isEmpty()){
        QToolTip::showText(ui->lineEdit_passwd->mapToGlobal(QPoint(100, 0)), "密码不能为空!");
        return;
    }

    if(connectFtp(host,port,user,code)){
        QMessageBox::information(this,"提示","连接成功！");
    }else{
        QMessageBox::information(this,"提示","连接失败！");
    }
}


bool Update::connectFtp(QString host, QString port, QString user, QString code)
{
    return m_ftp.Open(host.toStdString(),port.toInt(), user.toStdString(), code.toStdString());
}



bool Update::putFile(QString strpath,QString version)
{
    bool lb = m_ftp.IsLoggedIn();
    emit updateValue(true,3);
    if (!lb){
        emit  upLoadRet(1);
        return false;
    }
    QString strdir = strpath +"/";
    QString fileName = strdir + FileList;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append | QFile::Truncate)){
        emit  upLoadRet(2);
        return false;
    }
    emit updateValue(true,5);
    QTextStream stream(&file);
    std::vector<std::string> nameList;
    std::map<std::string, std::vector<std::string>> mplist;

    if(!m_ftp.GetLocalItemList(strdir.toStdString(), false, mplist, nameList)){
        qDebug()<<"获取本地文件列表失败!";
        emit  upLoadRet(3);
        return false;
    }
    emit updateValue(true,10);


    double onceV =10;
    int fileCount = nameList.size()-2;
    std::map<std::string, std::vector<std::string>>::iterator it = mplist.begin();
    for (; it != mplist.end(); ++it){
        std::vector<std::string> listName = it->second;
        fileCount+=listName.size()-2;
    }
    double iter = 89.0/fileCount;
    stream<<"filesize,"<<fileCount<<endl;
    m_ftp.CreateDirectory(version.toStdString());
    it = mplist.begin();
    for (; it != mplist.end(); ++it)
    {
        std::string filename = it->first;
        std::vector<std::string> listName = it->second;
        m_ftp.CreateDirectory(version.toStdString()+"/" + filename);
        foreach(const std::string& str, listName){
            if (str == "." || str == ".."||str==FileList.toStdString()){
                continue;
            }
            std::string sfname = str;
            std::string nalist = filename + "/" + sfname;
            stream << nalist.c_str() << endl;
            std::string fileNameList = strdir.toStdString() + nalist;
            m_ftp.PutOneFile(version.toStdString() + "/" + filename + "/" + sfname, fileNameList);
            onceV+=iter;
            emit updateValue(true,onceV);
        }
    }
    foreach(const std::string&str, nameList){
        if (str == "." || str == ".."||str==FileList.toStdString()){
            continue;
        }
        stream << str.c_str() << "\n";
        m_ftp.PutOneFile(version.toStdString() + "/" + str, strdir.toStdString() + str);
        onceV+=iter;
        emit updateValue(true,onceV);
    }
    file.close();
    std::string strfilepath=  QString(version + "/"+FileList).toStdString();
    m_ftp.PutOneFile(strfilepath,QString(strdir+ "/"+FileList).toStdString());
    emit updateValue(true,++onceV);
    return true;
}

int Update::downloadFile(QString version, QString savePath)
{
    bool lb = m_ftp.IsLoggedIn();
    if(!lb){
        if(m_isBackGround){
            if(!connectFtp(mConfig.ftpConfig().Host,mConfig.ftpConfig().Port,mConfig.ftpConfig().User,mConfig.ftpConfig().Code)){
                emit downLoadRet(1);
                return 1;
            }
        }else{
            emit downLoadRet(1);
            return 1;
        }
    }
    std::string strfilePath =  QString(version +"/"+FileList).toStdString();
    if(!m_ftp.GetOneFile(strfilePath, QString(savePath+"/"+FileList).toStdString())){
        emit downLoadRet(2);
        return 2;
    }
    emit updateValue(false,3);
    QFile file(savePath +"/"+ FileList);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        emit downLoadRet(3);
        return 3;
    }
    emit updateValue(false,5);
    double iter = 0;
    double v =5;
    while (!file.atEnd())
    {
        QString str = file.readLine();
        str = str.trimmed();
        if(str.contains("filesize")){
            QStringList list = str.split(QRegExp(","));
            if(list.size()==2){
                int c = QString(list.at(1)).toInt();
                iter = 95.0/c;
            }
            continue;
        }
        QString saveFilePath = savePath+"/"+str;
        QFileInfo info(saveFilePath);
        QDir dir;
        if(!dir.mkpath(info.absolutePath())) continue;
        m_ftp.GetOneFile(QString(version+"/" + str).toStdString(), QString(savePath+"/" + str).toStdString());
        v+=iter;
        emit updateValue(false,v-1);
    }
    emit updateValue(false,99);
    emit downLoadRet(0);
    return 0;
}


bool Update::updateVersion(VersionInfo info)
{
    bool ok =true;
    m_ver.SetVersion(info,ok,mConfig.httpConfig().Host,mConfig.httpConfig().Port);
    return ok;
}

bool Update::getNetVersion(VersionInfo &info)
{
    bool ok;
    info = m_ver.GetVersion(ok,mConfig.httpConfig().Host,mConfig.httpConfig().Port);
    return true;
}


void Update::checkVersion()
{
    VersionInfo netVersion;
    bool ok = getNetVersion(netVersion);
    if(ok){
        VersionInfo localVer = m_ver.versionInfo();
        if(netVersion.Version.toDouble()>localVer.Version.toDouble() ){
            mCheck.setNetVersion(netVersion);
            mCheck.exec();
            emit startCheck();
            return;
        }else{
            startMain();
        }
    }else{
        startMain();
    }
}


void Update::on_pushButton_select_clicked()
{

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    QApplication::applicationDirPath(), QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    ui->lineEdit_local_dir->setText(dir);
}

void Update::on_pushButton_get_Version_clicked()
{
    VersionInfo info;
    if(getNetVersion(info)){
        ui->lineEdit_net_version->setText(info.Version);
        ui->lineEdit_net_Date->setText(info.Date);
        ui->textEdit_net_des->setText(info.Des);
    }
}


void Update::on_pushButton_up_clicked()
{
    if(!m_ftp.IsLoggedIn()){
        QMessageBox::information(this,"提示","连接失败");
        return ;
    }
    QString  file = ui->lineEdit_local_dir->text().trimmed();
    QString  version = ui->lineEdit_local_version->text().trimmed();
    QString  date = ui->dateEdit_local->date().toString("yyyy-MM-dd");;
    QString  des = ui->textEdit_local->toPlainText();
    if(version.isEmpty()) {
        QMessageBox::information(this,"提示","请先设置版本！");
        return;
    }
    if(file.isEmpty()){
        QMessageBox::information(this,"提示","请先选择需要上传的文件夹！");
        return;
    }
    boost::thread(boost::bind(&Update::upFileAndUpdateVersion,this,file,version,date,des)).detach();
    mloadding.exec();
}


bool Update::upFileAndUpdateVersion(QString dir, QString version,QString date,QString des)
{
    if (!putFile(dir,version)){
        return false;
    }
    emit updateValue(true,96);
    VersionInfo info;
    info.Version = version;
    info.Date = date;
    info.Des = des;
    if(!updateVersion(info)){
        emit upLoadRet(4);
        return false;
    }
    emit updateValue(true,99);
    emit upLoadRet(0);
    return true;
}


void Update::on_pushButton_select_down_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "D:/", QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    ui->lineEdit_down_dir->setText(dir);
}

void Update::on_pushButton_down_load_clicked()
{
    QString savePath = ui->lineEdit_down_dir->text().trimmed();
    QString version = ui->lineEdit_net_version->text().trimmed();
    if(version.isEmpty()){
        QToolTip::showText(ui->lineEdit_net_version->mapToGlobal(QPoint(100, 0)), "版本不能为空!");
        return;
    }
    if(savePath.isEmpty()){
        QToolTip::showText(ui->lineEdit_down_dir->mapToGlobal(QPoint(100, 0)), "保存路径不能为空!");
        return;
    }

    boost::thread(boost::bind(&Update::downloadFile,this,version,savePath)).detach();
    mloadding.exec();
}

void Update::Logout()
{
    m_ftp.Logout();
}

void Update::ProcessExe(QString file)
{
    QProcess::startDetached(file);
}

void Update::startMain()
{
    if(!mConfig.MainExec().isEmpty())
        QProcess::startDetached(mConfig.MainExec());
}

void Update::showftpSetting()
{
    FtpConfigSetting mset;
    mset.exec();
}

void Update::downloadCallback(int ret)
{
    mloadding.showOver();
    if(m_isBackGround){
        if(!ret){
            qDebug()<<"更新成功！";
            m_ver.setCurVersion(mCheck.version());
            m_ver.saveVersion();
        }else{
            qDebug()<<"更新失败";
        }
        startMain();
    }else{
        switch (ret) {
        case 0:
            QMessageBox::information(this,"提示","下载成功!");
            break;
        case 1:
            QMessageBox::information(this,"提示","链接失败!");
            break;
        case 2:
            QMessageBox::information(this,"提示","列表文件下载失败!");
            break;
        case 3:
            QMessageBox::information(this,"提示","列表文件打开失败!");
            break;
        default:
            break;
        }
    }
}

void Update::uploadCallback(int ret)
{
    mloadding.showOver();
    if (ret==1){
        QMessageBox::information(this,"提示","链接失败");
    }else if (ret==2){
        QMessageBox::information(this,"提示","打开列表文件失败");
    }else if (ret==3){
        QMessageBox::information(this,"提示","获取本地文件列表失败");
    }else if (ret==4){
        QMessageBox::information(this,"提示","更新版本失败！");
    }else if (ret==0){
        QMessageBox::information(this,"提示","更新成功！");
    }
}

void Update::updateProgress(bool isUp, double value)
{
    if(isUp){
        mloadding.showMess(QString("正在上传(%1%)").arg(QString::number(value,'f',1)));
    }else{
        mloadding.showMess(QString("正在下载(%1%)").arg(QString::number(value,'f',1)));
    }

}

void Update::setIsBackGround(bool isBackGround)
{
    m_isBackGround = isBackGround;
}


void Update::startDownload(bool s)
{
    if(s){
        boost::thread(boost::bind(&Update::downloadFile,this,mCheck.version().Version,QApplication::applicationDirPath())).detach();
        mloadding.exec();
    }else{
        startMain();
    }
}


void Update::Zip(QString filename, QString zipfilename)
{
    QFile infile(filename);
    QFile outfile(zipfilename);
    infile.open(QIODevice::ReadOnly);
    outfile.open(QIODevice::WriteOnly);
    QByteArray uncompressedData = infile.readAll();
    QByteArray compressedData = qCompress(uncompressedData, 9);
    outfile.write(compressedData);
    infile.close();
    outfile.close();
}

void Update::Unzip(QString zipfilename, QString filename)
{
    QFile infile(zipfilename);
    QFile outfile(filename);
    infile.open(QIODevice::ReadOnly);
    outfile.open(QIODevice::WriteOnly);
    QByteArray uncompressedData = infile.readAll();
    QByteArray compressedData = qUncompress(uncompressedData);
    outfile.write(compressedData);
    infile.close();
    outfile.close();
}

