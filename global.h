#ifndef GLOBAL_H
#define GLOBAL_H
#include <QString>
#include <QJsonValue>

struct VersionInfo{
 QString Version;
 QString Date;
 QString Des;
 QString Url;
};



struct FtpConfig{
    QString Host;
    QString Port;
    QString User;
    QString Code;
};

struct HTTPConfig{
    QString Host;
    QString Port;
};


struct Ret{
    QString     msg;
    bool        ret;
    QJsonValue data;
};

const std::string Net_Getversion="/getversion";            //获取版本
const std::string Net_Setversion="/setversion";            //设置版本



#endif // GLOBAL_H
