#ifndef EDITPASSWDIALOG_H
#define EDITPASSWDIALOG_H

#include <QDialog>
#include "qhttptool.h"
#include <QMessageBox>
#include <QApplication>


namespace Ui {
class editPasswDialog;
}

class editPasswDialog : public QDialog
{
    Q_OBJECT

public:
    explicit editPasswDialog(QWidget *parent = nullptr);
    ~editPasswDialog();
    static void interfaceJump(QJsonObject info,int requestCode, QJsonValue data)
    {

        if(info.value("statusCode").toInt() == 200)
        {//right
            switch (requestCode)
            {
            case 1:
                if(data.toBool())
                    peditPasswDialog->jump();
                else if(!data.toBool())
                {
                    QMessageBox::warning(NULL , tr("提示"), tr("修改失败，请重新修改"), QMessageBox::Yes);
                }
                break;
            case 2:
                break;
            default:
                break;
            }
        }
        else
        {
//            QApplication::setQuitOnLastWindowClosed(false);
            QMessageBox::warning(NULL , tr("提示"), tr("连接服务器失败！"), QMessageBox::Yes);
        }
        return;
    }
    void jump();

signals:
    void editpassw();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void showMsg(QString title,QString str);
private:
    Ui::editPasswDialog *ui;
    static editPasswDialog* peditPasswDialog;
    QHttpTool qHttpTool;
};

#endif // EDITPASSWDIALOG_H
