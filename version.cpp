#include "version.h"
#include "http.h"
#include <QDebug>
#include <QSettings>
#include <QApplication>
#pragma execution_character_set("utf-8")
Version::Version(QObject *parent) : QObject(parent)
{
    loadVersion();
}


VersionInfo Version::GetVersion(bool &ok, QString hostname, QString hostport)
{
    VersionInfo v;
    std::string url = Net_Getversion;
    bool r   = false;
    QJsonObject obj;
    Ret ret  = Http::fetch(url,obj,r,hostname,hostport);
    if(r&&ret.ret){
        if(ret.data.isObject()){
            v = fromJsonObject(ret.data.toObject());
        }
        ok =true;
        return v;
    }
    if(!ret.ret)
        qDebug()<<"GetVersion ret is not 0"<<endl;
    ok = false;
    return  v;
}

void Version::SetVersion(VersionInfo info, bool &ok, QString hostname, QString hostport)
{
    SetVersion(toJsonObject(info),ok,hostname,hostport);
}

void Version::SetVersion(const QJsonObject para, bool &ok, QString hostname, QString hostport)
{
    std::string url = Net_Setversion;
    bool r   = false;
    Ret ret  = Http::fetch(url,para,r,hostname,hostport);
    if(r&&ret.ret){
        ok =true;
        return ;
    }
    if(!ret.ret)
        qDebug()<<"SetVersion ret is not 0"<<endl;
    ok = false;
    return ;
}
QJsonObject Version::toJsonObject(VersionInfo v)
{
    QJsonObject obj;
    obj.insert("Version",v.Version);
    obj.insert("Date",v.Date);
    obj.insert("Des",v.Des);
    obj.insert("Url",v.Url);
    return obj;
}

VersionInfo Version::fromJsonObject(QJsonObject obj)
{
    VersionInfo v;
    if(obj.contains("Version")){
        QJsonValue va = obj.value("Version");
        if(va.isString())
            v.Version = va.toString();
    }

    if(obj.contains("Date")){
        QJsonValue va = obj.value("Date");
        if(va.isString())
            v.Date = va.toString();
    }
    if(obj.contains("Des")){
        QJsonValue va = obj.value("Des");
        if(va.isString())
            v.Des = va.toString();
    }
    if(obj.contains("Url")){
        QJsonValue va = obj.value("Url");
        if(va.isString())
            v.Url = va.toString();
    }
    return v;
}

VersionInfo Version::versionInfo() const
{
    return m_cur_ver;
}


void Version::loadVersion()
{
    QSettings m_settings(QApplication::applicationDirPath()+"/VERSION",QSettings::IniFormat);
    m_settings.setIniCodec("uft-8");
    m_settings.beginGroup("VERSION");
    m_cur_ver.Version = m_settings.value("Version").toString();
    m_cur_ver.Date = m_settings.value("Date").toString();
    m_cur_ver.Des = m_settings.value("Des").toString();
    m_settings.endGroup();
}

void Version::saveVersion()
{
    QSettings m_settings(QApplication::applicationDirPath()+"/VERSION",QSettings::IniFormat);
    m_settings.setIniCodec("uft-8");
    m_settings.beginGroup("VERSION");
    m_settings.setValue("Version",m_cur_ver.Version);
    m_settings.setValue("Date",m_cur_ver.Date);
    m_settings.setValue("Des",m_cur_ver.Des);
    m_settings.endGroup();
}

void Version::setCurVersion(const VersionInfo &cur_ver)
{
    m_cur_ver = cur_ver;
}


