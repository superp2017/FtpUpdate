#ifndef CHECKVERSION_H
#define CHECKVERSION_H

#include <QDialog>
#include "global.h"

namespace Ui {
class CheckVersion;
}

class CheckVersion : public QDialog
{
    Q_OBJECT

public:
    explicit CheckVersion(QWidget *parent = 0);
    ~CheckVersion();
    VersionInfo version() const;
    void setNetVersion(VersionInfo info);
signals:
    void startUpdate(bool);
private slots:
    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();

private:
    Ui::CheckVersion *ui;
    VersionInfo m_version;
};

#endif // CHECKVERSION_H
