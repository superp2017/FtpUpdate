#include "nsysconfig.h"
#include <QSettings>
#include <QDebug>
#include <QApplication>
#pragma execution_character_set("utf-8")
nSysConfig::nSysConfig(QObject *parent) : QObject(parent)
{

}

void nSysConfig::loadConfig()
{
    QSettings m_settings(QApplication::applicationDirPath()+"/ftpconfig.ini",QSettings::IniFormat);
    m_settings.setIniCodec("uft-8");
    m_settings.beginGroup("FTP");
    m_ftpConfig.Host  = m_settings.value("Host").toString();
    m_ftpConfig.Port  = m_settings.value("Port").toString();
    m_ftpConfig.User  = m_settings.value("User").toString();
    m_ftpConfig.Code  = m_settings.value("Code").toString();
    m_settings.endGroup();

    m_settings.beginGroup("HTTP");
    m_httpConfig.Host  = m_settings.value("Host").toString();
    m_httpConfig.Port  = m_settings.value("Port").toString();
    m_settings.endGroup();

    m_settings.beginGroup("EXEC");
    m_MainExec  = m_settings.value("MainExec").toString();
    m_settings.endGroup();
}

void nSysConfig::saveConfg(FtpConfig &ftp, HTTPConfig &http, QString path)
{
    m_ftpConfig  = ftp;
    m_httpConfig = http;
    m_MainExec = path;
    QSettings m_settings(QApplication::applicationDirPath()+"/ftpconfig.ini",QSettings::IniFormat);
    m_settings.setIniCodec("uft-8");
    m_settings.beginGroup("FTP");
    m_settings.setValue("Host",ftp.Host);
    m_settings.setValue("Port",ftp.Port);
    m_settings.setValue("User",ftp.User);
    m_settings.setValue("Code",ftp.Code);
    m_settings.endGroup();

    m_settings.beginGroup("HTTP");
    m_settings.setValue("Host",http.Host);
    m_settings.setValue("Port",http.Port);
    m_settings.endGroup();

    m_settings.beginGroup("EXEC");
    m_settings.setValue("MainExec",m_MainExec);
    m_settings.endGroup();
}

FtpConfig& nSysConfig::ftpConfig()
{
    return m_ftpConfig;
}

HTTPConfig& nSysConfig::httpConfig()
{
    return m_httpConfig;
}

QString& nSysConfig::MainExec()
{
    return m_MainExec;
}


