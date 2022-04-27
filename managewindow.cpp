#include "managewindow.h"
#include "ui_managewindow.h"
#include <string>
#include "salesmantable.h"
#include <QTableView>
#include <QRandomGenerator>
#include <QMessageBox>
#include "qhttptool.h"
#include <QThread>
#include <QApplication>

using namespace std;
managewindow* managewindow::pmanageWindow=NULL;

managewindow::managewindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::managewindow)
{
    ui->setupUi(this);
    managewindow::pmanageWindow=this;
    setWindowTitle("管理员工");
    mymodel=new salesmantable(ui->tV_salesman);
    ui->tV_salesman->setModel(mymodel);
    ui->tV_salesman->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//所有列设置自动列宽
    //ui->tV_salesman->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);//对第0列单独设置固定宽度
    //ui->tV_salesman->setColumnWidth(1, 100);//设置固定宽度
    ui->tV_salesman->setSelectionMode(QAbstractItemView::SingleSelection);    //设置只能选单行
    ui->tV_salesman->setSelectionBehavior(QAbstractItemView::SelectRows);     //设置只能选中行
    ui->lineEdit_acc->setPlaceholderText("请输入字母或数字");
    ui->lineEdit_passw->setPlaceholderText("请输入字母或数字");
    this->setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    connect(this,SIGNAL(DisplayData(QList<salesman>)),this,SLOT(DisplayData(QList<salesman>)));
    setData();
}

managewindow::~managewindow()
{
    delete ui;
}

void managewindow::on_pB_homepage_clicked()
{
    emit manage();
    this->close();
}
void managewindow::DisplayData(QList<salesman> new_data)
{
    qDebug()<<"DisplayData";
    mymodel->setModelData(new_data);
    salesmanVec=new_data;
}

void managewindow::setData()
{
    //一般我们从数据库、文件、网络获取到了数据后，再通过model的接口去刷新
    QJsonObject js;
    qHttpTool.setIP(csIP);
    qHttpTool.setPort(csPort);
    qHttpTool.setData(js);
    qHttpTool.setRouting("/user/findAllU");
    qHttpTool.bindCallback(interfaceJump);
    qHttpTool.GET(0);
}
QList<salesman> managewindow::getData()
{
    return mymodel->getModelData();
}
void managewindow::addRow()
{
    //在选中行和下一行之间插入
    //model->insertRows(ui->tableView->currentIndex().row()+1,1);
    //model->insertRow(ui->tableView->currentIndex().row()+1);
    QString account=ui->lineEdit_acc->text();
    QString password=ui->lineEdit_passw->text();
    mymodel->insertModelData(ui->tV_salesman->currentIndex().row()+1,salesman{
                                 1,
                               QString(account),
                               QString(password)});
}
void managewindow::delRow()
{
    //删除选中行
    //model->removeRows(ui->tableView->currentIndex().row(),1);
    mymodel->removeRow(ui->tV_salesman->currentIndex().row());
}

bool accisSameOrNot(QString acc,QList<salesman> data)
{
    for(int i=0;i<data.size();i++)
    {
        if(acc==data.at(i).getaccount())
            return true;
    }
    return false;
}

void managewindow::on_pB_addsalesman_clicked()
{
    QString account=ui->lineEdit_acc->text().trimmed();
    QString password=ui->lineEdit_passw->text().trimmed();
    if(account.isEmpty()||password.isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("用户名或密码不能为空"), QMessageBox::Yes);
    }
    else
    {
        QJsonObject js;
        if(accisSameOrNot(account,salesmanVec))
        {
//            QApplication::setQuitOnLastWindowClosed(false);
            QMessageBox::warning(NULL, tr("提示"), tr("账号不能重复"), QMessageBox::Yes);
        }
        else
        {
            int id=salesmanVec.size();
            cout<<"idsdfsdf"<<id<<endl;
            qHttpTool.clearData();
            js.empty();
            js.insert("account",account);
            js.insert("password",password);
            qHttpTool.setIP(csIP);
            qHttpTool.setPort(csPort);
            qHttpTool.setData(js);
            qHttpTool.setRouting("/user/AddSaler");
            qHttpTool.bindCallback(interfaceJump);
            qHttpTool.POST(1);
//            ui->lineEdit_acc->clear();
//            ui->lineEdit_passw->clear();
        }
    }    
}

void managewindow::jump1()
{
//    QApplication::setQuitOnLastWindowClosed(false);
    QMessageBox::warning(NULL , tr("提示"), tr("添加成功"), QMessageBox::Yes);
    addRow();
    ui->lineEdit_acc->clear();
    ui->lineEdit_passw->clear();
    pmanageWindow->setData();
}

void managewindow::on_pB_editsalesman_clicked()
{
    QList<salesman> salesmanList;
    //QModelIndexList list=ui->tV_salesman->selectedIndexes();
    QModelIndex index=ui->tV_salesman->currentIndex();
    int iSel=index.row();
    ui->lineEdit_acc->setText(salesmanList.at(iSel).getaccount());
    ui->lineEdit_passw->setText(salesmanList.at(iSel).getpassword());
}

int whatisId(QList<salesman> data,QString acc)
{
    for(int i=0;i<data.size();i++)
    {
        if(acc==data.at(i).getaccount())
            return data.at(i).getsID();
    }
    return -1;
}

void managewindow::on_pB_deletesalesman_clicked()
{
    int row = ui->tV_salesman->currentIndex().row();
    if(nullptr == mymodel || row < 0 || row > mymodel->rowCount()-1)
    {
        QMessageBox::information(NULL,"提示","找不到操作项",QStringLiteral("确定")
                                 ,QStringLiteral("取消"),"");
        return ;
    }
    if(nullptr == mymodel || row < 0 || row > mymodel->rowCount()-1)
        return;
//    QApplication::setQuitOnLastWindowClosed(false);
    QMessageBox *msgBox = new QMessageBox(QMessageBox::Question, tr("提示"), tr("确定要删除吗?"), QMessageBox::Yes | QMessageBox::No);
    msgBox->button(QMessageBox::Yes)->setText("是");
    msgBox->button(QMessageBox::No)->setText("否");
    int res = msgBox->exec();
    if(res==QMessageBox::Yes)
    {
        QJsonObject js;
        QModelIndex aindex=ui->tV_salesman->model()->index(row,1);
        QModelIndex pindex=ui->tV_salesman->model()->index(row,2);
        QString account=ui->tV_salesman->model()->data(aindex).toString();
        QString password=ui->tV_salesman->model()->data(pindex).toString();
        if(whatisId(salesmanVec,account)==-1)
        {
            return;
        }
        js.insert("id",whatisId(salesmanVec,account));
        js.insert("account",account);
        js.insert("password",password);
        qHttpTool.setIP(csIP);
        qHttpTool.setPort(csPort);
        qHttpTool.setData(js);
        qHttpTool.setRouting("/user/DeleteSaler");
        qHttpTool.bindCallback(interfaceJump);
        qHttpTool.POST(2);
        mymodel->removeRows(row, 1);
    }
}

void managewindow::jump2()
{
//    QApplication::setQuitOnLastWindowClosed(false);
    QMessageBox::warning(NULL , tr("提示"), tr("删除成功"), QMessageBox::Yes);
    pmanageWindow->setData();
}
