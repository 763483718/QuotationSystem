#include "logindialog.h"
#include "mainwindow.h"
#include "ui_logindialog.h"
#include <QMessageBox>
#include <QKeyEvent>
#include "qhttptool.h"
#include <QThread>
#include <QApplication>
using namespace std;

int loginDialog::isAdmin=0;
loginDialog* loginDialog::pLoginDialog=NULL;
QComboBox* loginDialog::pcombox=NULL;

loginDialog::loginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginDialog)
{
    ui->setupUi(this);
    setWindowTitle("登录");
    ui->lineEdit_Login->setFocus();
    ui->comboBox_role->addItem("管理人员");
    ui->comboBox_role->addItem("业务人员");
    QWidget::setTabOrder(ui->lineEdit_Login, ui->lineEdit_Pwd_2);
    loginDialog::pLoginDialog=this;
    loginDialog::pcombox=ui->comboBox_role;
}

loginDialog::~loginDialog()
{
    this->disconnect();
    delete ui;
}

//int loginDialog::getisAdmin()
//{
//    return isAdmin;
//}
//void loginDialog::setisAdmin(int flag)
//{
//    isAdmin=flag;
//}

void loginDialog::jump()
{
//    this->hide();
//    MainWindow *main=new MainWindow(this);
//    main->show();
//    connect(main, SIGNAL(login()), this, SLOT(receiveLogin()));
//    this->hide();

    qDebug()<<"isAdminjump"<<isAdmin;
    this->close();
    emit login();
}

void loginDialog::on_pB_login_clicked()
{
//    this->hide();
//    isAdmin=1;
//    MainWindow *main=new MainWindow(this);
//    main->show();
//    connect(main, SIGNAL(login()), this, SLOT(receiveLogin()));

    if(ui->comboBox_role->currentIndex()==0)
    {
        isAdmin=1;
        qDebug()<<"isAdminbtn"<<isAdmin;
        QJsonObject js;
        js.insert("account",ui->lineEdit_Login->text());
        js.insert("password",ui->lineEdit_Pwd_2->text());
        qHttpTool.setIP(csIP);
        qHttpTool.setPort(csPort);
        qHttpTool.setData(js);
        qHttpTool.setRouting("/user/login");
        qHttpTool.bindCallback(interfaceJump);
        int netStatus=qHttpTool.POST(1);
        if(netStatus==0)
        {
            ui->lineEdit_Login->clear();
            ui->lineEdit_Pwd_2->clear();
            ui->lineEdit_Login->setFocus();
        }
        else if(netStatus==1)
        {
//            QApplication::setQuitOnLastWindowClosed(false);
            QMessageBox::warning(NULL , tr("提示"), tr("服务器网络未连接"), QMessageBox::Yes);
        }
    }
    else if(ui->comboBox_role->currentIndex()==1)
    {
        isAdmin=0;
        qDebug()<<"isAdminbtn"<<isAdmin;
        QJsonObject js;
        js.insert("account",ui->lineEdit_Login->text());
        js.insert("password",ui->lineEdit_Pwd_2->text());
        qHttpTool.setIP(csIP);
        qHttpTool.setPort(csPort);
        qHttpTool.setData(js);
        qHttpTool.setRouting("/user/login");
        qHttpTool.bindCallback(interfaceJump);
        qHttpTool.POST(1);
        ui->lineEdit_Login->clear();
        ui->lineEdit_Pwd_2->clear();
        ui->lineEdit_Login->setFocus();
    }
}

void loginDialog::receiveLogin()
{
    this->show();
}

void loginDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        emit ui->pB_login->click();
    }
}

void loginDialog::on_comboBox_role_currentIndexChanged(int index)
{
    isAdmin=!index;
}
