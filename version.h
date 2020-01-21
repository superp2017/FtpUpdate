#ifndef VERSION_H
#define VERSION_H

#include <QObject>
#include "global.h"
#include <QJsonObject>

class Version : public QObject
{
    Q_OBJECT
public:
    explicit Version(QObject *parent = nullptr);

    static VersionInfo GetVersion(bool &ok, QString hostname, QString hostport);

    static void SetVersion(VersionInfo info, bool &ok, QString hostname, QString hostport);

    static void SetVersion(const QJsonObject para, bool &ok, QString hostname, QString hostport);

    static QJsonObject toJsonObject(VersionInfo v);
    static VersionInfo fromJsonObject(QJsonObject obj);

    VersionInfo versionInfo() const;

    void loadVersion();

    void saveVersion();

    void setCurVersion(const VersionInfo &cur_ver);

signals:

public slots:


private:
    VersionInfo m_cur_ver;
};

#endif // VERSION_H
