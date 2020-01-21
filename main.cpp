#include <QApplication>
#include "update.h"
#include <QDir>
#include <QDebug>
#include <QFileInfo>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Update w;
    if(argc==2&&QString(argv[1])=="-b"){
        w.setIsBackGround(true);
        w.checkVersion();
    }else{
        w.setIsBackGround(false);
        w.show();
        return a.exec();
    }
    return 0;
}
