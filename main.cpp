#include "logindialog.h"
#include "managewindow.h"
#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    loginDialog logIn;
    MainWindow main;
    logIn.setWindowTitle("登录");
    logIn.show();

//    点击登录对话框的登录按钮，进入主界面
    QObject::connect(&logIn,SIGNAL(login()),&main,SLOT(show_main()));
    //点击主界面的注销，返回登录对话框
    QObject::connect(&main,SIGNAL(logout()),&logIn,SLOT(receiveLogin()));
    try {
        return a.exec();
    } catch (exception e) {
        QFile file(".\\config.txt");
        if(file.exists()){
            file.remove();
        }
        return 0 ;
    }
}
