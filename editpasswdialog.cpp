#include "editpasswdialog.h"
#include "ui_editpasswdialog.h"
#include "logindialog.h"
#include <QMessageBox>
#include <QString>
//#include "qhttptool.h"

editPasswDialog* editPasswDialog::peditPasswDialog=NULL;
editPasswDialog::editPasswDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::editPasswDialog)
{
    ui->setupUi(this);
    setWindowTitle("修改密码");
    peditPasswDialog=this;
    ui->lineEdit_newAccount->setFocus();
    QWidget::setTabOrder(ui->lineEdit_newAccount, ui->lineEdit_newPassw);
    ui->lineEdit_oldPassw->hide();
    //QWidget::setTabOrder(ui->lineEdit_oldPassw, ui->lineEdit_newPassw);
    QWidget::setTabOrder(ui->lineEdit_newPassw, ui->lineEdit_confirmPassw);
    ui->lineEdit_newPassw->setPlaceholderText("请输入字母或数字");
    ui->lineEdit_oldPassw->setPlaceholderText("请输入字母或数字");
    ui->lineEdit_newAccount->setPlaceholderText("请输入字母或数字");
    ui->lineEdit_confirmPassw->setPlaceholderText("请输入字母或数字");
}

editPasswDialog::~editPasswDialog()
{
    delete ui;
}

void editPasswDialog::on_buttonBox_accepted()
{
    if(ui->lineEdit_newAccount->text()=="" ||ui->lineEdit_newPassw->text()=="" || ui->lineEdit_confirmPassw->text()=="")
    {
//        QApplication::setQuitOnLastWindowClosed(false);
        QMessageBox::warning(NULL , tr("提示"), tr("账号密码不能为空，请重新输入"), QMessageBox::Yes);
    }
    else
    {
        QString newAcc=ui->lineEdit_newAccount->text().trimmed();
//        QString oldPassw=ui->lineEdit_oldPassw->text().trimmed();
        QString newPassw=ui->lineEdit_newPassw->text().trimmed();
        QString confirmPassw=ui->lineEdit_confirmPassw->text().trimmed();
        if(newPassw!=confirmPassw)
        {
//            QApplication::setQuitOnLastWindowClosed(false);
            QMessageBox::warning(NULL , tr("提示"), tr("两次输入密码不同，请重新输入"), QMessageBox::Yes);
        }
        else
        {   QJsonObject js;
            js.insert("id",1);
            js.insert("account",newAcc);
            js.insert("password",newPassw);
            qHttpTool.setIP(csIP);
            qHttpTool.setPort(csPort);
            qHttpTool.setData(js);
            qHttpTool.setRouting("/user/updateSaler");
            qHttpTool.bindCallback(interfaceJump);
            qHttpTool.POST(1);
        }
    }
}

void editPasswDialog::jump()
{
    QMessageBox::warning(NULL , tr("提示"), tr("修改成功"), QMessageBox::Yes);
}

void editPasswDialog::on_buttonBox_rejected()
{
    emit editpassw();
    this->close();
}
void editPasswDialog::showMsg(QString title,QString str)
{
    QMessageBox msg;
    msg.setWindowTitle(title);
    msg.setText(str);
    msg.exec();
}
