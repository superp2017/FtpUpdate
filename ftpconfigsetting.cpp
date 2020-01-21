#include "ftpconfigsetting.h"
#include "ui_ftpconfigsetting.h"
#include <QFileDialog>
#include <QApplication>
#include <QToolTip>
#pragma execution_character_set("utf-8")
FtpConfigSetting::FtpConfigSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FtpConfigSetting)
{
    ui->setupUi(this);
    on_pushButton_reload_clicked();
}

FtpConfigSetting::~FtpConfigSetting()
{
    delete ui;
}

void FtpConfigSetting::on_pushButton_clicked()
{
    FtpConfig  m_ftpConfig;
    HTTPConfig m_httpConfig;
    m_ftpConfig.Host = ui->lineEdit_ftp_host->text().trimmed();
    m_ftpConfig.Port = ui->lineEdit_ftp_port->text().trimmed();
    m_ftpConfig.User = ui->lineEdit_ftp_user->text().trimmed();
    m_ftpConfig.Code = ui->lineEdit_ftp_code->text().trimmed();

    m_httpConfig.Host = ui->lineEdit_http_host->text().trimmed();
    m_httpConfig.Port = ui->lineEdit_http_port->text().trimmed();

    if(m_ftpConfig.Host.isEmpty()){
        QToolTip::showText(ui->lineEdit_ftp_host->mapToGlobal(QPoint(100, 0)), "主机不能为空!");
        return;
    }
    if(m_ftpConfig.Port.isEmpty()){
        QToolTip::showText(ui->lineEdit_ftp_port->mapToGlobal(QPoint(100, 0)), "端口不能为空!");
        return;
    }
    if(m_ftpConfig.User.isEmpty()){
        QToolTip::showText(ui->lineEdit_ftp_user->mapToGlobal(QPoint(100, 0)), "用户名不能为空!");
        return;
    }
    if(m_ftpConfig.Code.isEmpty()){
        QToolTip::showText(ui->lineEdit_ftp_code->mapToGlobal(QPoint(100, 0)), "密码不能为空!");
        return;
    }
    if(m_httpConfig.Host.isEmpty()){
        QToolTip::showText(ui->lineEdit_http_host->mapToGlobal(QPoint(100, 0)), "主机不能为空!");
        return;
    }
    if(m_httpConfig.Port.isEmpty()){
        QToolTip::showText(ui->lineEdit_http_port->mapToGlobal(QPoint(100, 0)), "端口不能为空!");
        return;
    }

    QString execPath = ui->lineEdit_exec->text().trimmed();
    if(execPath.isEmpty()){
        QToolTip::showText(ui->lineEdit_exec->mapToGlobal(QPoint(100, 0)), "没有选择可执行程序!");
    }
    mconfig.saveConfg(m_ftpConfig,m_httpConfig,execPath);
}

void FtpConfigSetting::initView()
{
    ui->lineEdit_ftp_host->setText(mconfig.ftpConfig().Host);
    ui->lineEdit_ftp_port->setText(mconfig.ftpConfig().Port);
    ui->lineEdit_ftp_user->setText(mconfig.ftpConfig().User);
    ui->lineEdit_ftp_code->setText(mconfig.ftpConfig().Code);
    ui->lineEdit_http_host->setText(mconfig.httpConfig().Host);
    ui->lineEdit_http_port->setText(mconfig.httpConfig().Port);
    ui->lineEdit_exec->setText(mconfig.MainExec());
}

void FtpConfigSetting::on_pushButton_reload_clicked()
{
    mconfig.loadConfig();
    initView();
}

void FtpConfigSetting::on_pushButton_select_clicked()
{

    QString fileName = QFileDialog::getOpenFileName(this, tr("Select exe File"), QApplication::applicationDirPath(),tr("Exe (*.exe)"));
    if(!fileName.isEmpty()){
        ui->lineEdit_exec->setText(fileName);
    }
}
