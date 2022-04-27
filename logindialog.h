#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <iostream>
#include <string>
#include "qhttptool.h"
#include <QMessageBox>
#include <QComboBox>
#include <QApplication>
using namespace std;

static string account="1";
static string password="1";

namespace Ui {
class loginDialog;
}

class loginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit loginDialog(QWidget *parent = nullptr);
    ~loginDialog();
    void keyPressEvent(QKeyEvent *event);    //设置回车响应函数
    static void interfaceJump(QJsonObject info,int requestCode, QJsonValue data)
    {
        if(info.value("statusCode").toInt() == 200){//right
            QString str;
            QStringList list;
            switch (requestCode)
            {
            case 1:
                str=data.toString();
                list=str.split(" ");
                if(list[0]==QString("true"))
                {
                    if(isAdmin)
                    {
                        if(list[1].toInt()!=1)
                            QMessageBox::warning(NULL , tr("提示"), tr("该账号不是管理员，请重新选择角色"), QMessageBox::Yes);
                        else
                            pLoginDialog->jump();
                    }
                    else if(!isAdmin)
                    {
                        if(list[1].toInt()>1)
                            pLoginDialog->jump();
                        else
                            QMessageBox::warning(NULL , tr("提示"), tr("该账号不是业务员，请重新选择角色"), QMessageBox::Yes);
                     }
                    else
                        QMessageBox::warning(NULL , tr("提示"), tr("角色选择不匹配，请重新选择"), QMessageBox::Yes);
                }

                else if(!data.toBool())
                {
                    QMessageBox::warning(NULL , tr("提示"), tr("用户名或者密码不正确，请重新输入"), QMessageBox::Yes);
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

    static int isAdmin;
    static QComboBox* pcombox;


signals:
    void login();

private slots:

    void on_pB_login_clicked();
    void receiveLogin();

    void on_comboBox_role_currentIndexChanged(int index);

private:
    static loginDialog* pLoginDialog;
    Ui::loginDialog *ui;
    QHttpTool qHttpTool;

};

#endif // LOGINDIALOG_H
