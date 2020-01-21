#ifndef NSYSCONFIG_H
#define NSYSCONFIG_H

#include <QObject>
#include <QSet>
#include "global.h"

#pragma execution_character_set("utf-8")

class nSysConfig : public QObject
{
    Q_OBJECT
public:
    explicit nSysConfig(QObject *parent = nullptr);
    void loadConfig();
    void saveConfg(FtpConfig &ftp,HTTPConfig &http,QString path);

    FtpConfig& ftpConfig() ;
    HTTPConfig& httpConfig() ;

    QString &MainExec() ;

signals:

private:
    FtpConfig  m_ftpConfig;
    HTTPConfig m_httpConfig;
    QString    m_MainExec;
};

#endif // NSYSCONFIG_H
