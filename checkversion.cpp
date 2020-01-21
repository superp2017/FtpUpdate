#include "checkversion.h"
#include "ui_checkversion.h"
#pragma execution_character_set("utf-8")
CheckVersion::CheckVersion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CheckVersion)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint|Qt::SubWindow);//无边框
}

CheckVersion::~CheckVersion()
{
    delete ui;
}


void CheckVersion::on_pushButton_ok_clicked()
{
    emit startUpdate(true);
    this->done(0);
}

void CheckVersion::on_pushButton_cancel_clicked()
{
    emit startUpdate(false);
    this->done(-1);
}

void CheckVersion::setNetVersion(VersionInfo info)
{
    m_version = info;
    ui->lineEdit_date->setText(info.Date);
    ui->lineEdit_version->setText(info.Version);
    ui->textEdit_des->setText(info.Des);
}

VersionInfo CheckVersion::version() const
{
    return m_version;
}
