#include "dialogloadding.h"
#include "ui_dialogloadding.h"
#include <QPoint>
#include <QPalette>
#include <QApplication>
#include <QDesktopWidget>
DialogLoadding::DialogLoadding(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLoadding)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint|Qt::SubWindow);//无边框
    setAttribute(Qt::WA_TranslucentBackground);//背景透明
    QDesktopWidget *deskdop = QApplication::desktop();
    int screen_count = deskdop->screenCount();
    move((deskdop->width()/screen_count-this->width())/2, (deskdop->height()-this->height())/2);
}

DialogLoadding::~DialogLoadding()
{
    delete ui;
}

void DialogLoadding::showMess(QString str)
{
    ui->label_gif->setText(str);
}

void DialogLoadding::showOver()
{
    this->done(0);
}
