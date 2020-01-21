#ifndef UPDATE_H
#define UPDATE_H

#include <QMainWindow>
#include "cftp_c.h"
#include "global.h"
#include "version.h"
#include "nsysconfig.h"
#include "checkversion.h"
#include "dialogloadding.h"


const QString FileList= QString("FILE.LIST");

namespace Ui {
class Update;
}


class Update : public QMainWindow
{
    Q_OBJECT

public:
    explicit Update(QWidget *parent = nullptr);
    ~Update();
    void checkVersion();
    void setIsBackGround(bool isBackGround);

signals:
    void startCheck();
    void updateValue(bool,double);
    void downLoadRet(int);
    void upLoadRet(int);
private slots:
    void initView();
    void on_pushButton_select_clicked();

    void on_pushButton_up_clicked();

    void on_pushButton_connect_clicked();

    void on_pushButton_select_down_clicked();

    void on_pushButton_down_load_clicked();

    void on_pushButton_get_Version_clicked();

    bool upFileAndUpdateVersion(QString dir, QString version, QString date, QString des);

    bool putFile(QString strpath,QString version);

    int  downloadFile(QString version,QString savePath);

    bool updateVersion(VersionInfo info);

    bool getNetVersion(VersionInfo &info);

    bool connectFtp(QString host,QString port,QString user,QString code);

    void Logout();

    void ProcessExe(QString file);

    void Zip(QString filename, QString zipfilename);

    void Unzip(QString zipfilename, QString filename);

    void startDownload(bool s);

    void startMain();

    void showftpSetting();

    void downloadCallback(int ret);

    void uploadCallback(int ret);

    void updateProgress(bool isUp, double value);
private:
    Ui::Update *ui;
    CFTP_C  m_ftp;
    Version m_ver;
    nSysConfig mConfig;
    CheckVersion mCheck;
    bool m_isBackGround;
    DialogLoadding mloadding;
};

#endif // UPDATE_H
