#include "waitingdialog.h"
#include "ui_waitingdialog.h"
#include <QMovie>

WaitingDialog::WaitingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WaitingDialog)
{
    ui->setupUi(this);
    //设置透明度
    this->setWindowOpacity(0.8);
    //取消对话框标题
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);//设置为对话框风格，并且去掉边框
    setWindowModality(Qt::WindowModal);//设置为模式对话框，同时在构造该对话框时要设置父窗口
    ui->label_waiting->setStyleSheet("background-color: transparent;");
    m_Move = new QMovie(":/new/prefix1/loading2.gif");
    ui->label_waiting->setMovie(m_Move);
    ui->label_waiting->setScaledContents(true);
    ui->label_waiting_text->setText("加载中......");
    m_Move->start();
}

WaitingDialog::~WaitingDialog()
{
    m_Move->stop();
    delete ui;
}
