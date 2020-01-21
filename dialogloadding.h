#ifndef DIALOGLOADDING_H
#define DIALOGLOADDING_H

#include <QDialog>

namespace Ui {
class DialogLoadding;
}

class DialogLoadding : public QDialog
{
    Q_OBJECT

public:
    explicit DialogLoadding(QWidget *parent = 0);
    ~DialogLoadding();
public slots:
    void showOver();
    void showMess(QString str);

private:
    Ui::DialogLoadding *ui;
};

#endif // DIALOGLOADDING_H
