#ifndef FTPCONFIGSETTING_H
#define FTPCONFIGSETTING_H

#include <QDialog>
#include "global.h"
#include "nsysconfig.h"
#include "ftpconfigsetting.h"
namespace Ui {
class FtpConfigSetting;
}

class FtpConfigSetting : public QDialog
{
    Q_OBJECT

public:
    explicit FtpConfigSetting(QWidget *parent = 0);
    ~FtpConfigSetting();

private slots:
    void on_pushButton_clicked();
    void initView();
    void on_pushButton_reload_clicked();
    void on_pushButton_select_clicked();

private:
    Ui::FtpConfigSetting *ui;
    nSysConfig mconfig;
};

#endif // FTPCONFIGSETTING_H
