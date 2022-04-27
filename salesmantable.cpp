#include "salesmantable.h"
#include "managewindow.h"
#include <iostream>
#include <QMessageBox>
#include <QApplication>
using namespace std;

salesmantable* salesmantable::psalesmantable=NULL;
salesmantable::salesmantable(QObject *parent) :
    QAbstractTableModel(parent)
{
    salesmantable::psalesmantable=this;
    populateModel();
    connect(this,SIGNAL(getAllU(QList<salesman>)),this,SLOT(getAllU(QList<salesman>)));
}

void salesmantable::setModelData(const QList<salesman> &datas)
{
    beginResetModel();
    salesmanVec=datas;
    endResetModel();
}

void salesmantable::setModelData()
{
//    QApplication::setQuitOnLastWindowClosed(false);
    QMessageBox::warning(NULL, tr("提示"), tr("显示成功"), QMessageBox::Yes);
}

QList<salesman> salesmantable::getModelData() const
{
    return salesmanVec;
}

bool salesmantable::insertModelData(int row, const salesman &datas)
{
    if(row<0||row>rowCount())
        return false;
    //需要将操作放到beginInsertRows和endInsertRows两个函数调用之间
    beginInsertRows(QModelIndex(), row, row);
    //在接口对应行插入空数据
    salesmanVec.insert(row,datas);
    endInsertRows();
    emit dataChanged(index(0,0),index(rowCount(),columnCount()-1),QVector<int>());
    return true;
}

int salesmantable::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return 2;
}

int salesmantable::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return salesmanVec.size();
}

QVariant salesmantable::data(const QModelIndex &index, int role= Qt::DisplayRole) const
{
    if(!index.isValid())
        return QVariant();

    if(role==Qt::DisplayRole)
    {
        const int row=index.row();
        switch(index.column())
        {
        case 0: return salesmanVec.at(row).getaccount();
        case 1:return salesmanVec.at(row).getpassword();
        }
    }
    if(role==Qt::EditRole)
    {
        const int row=index.row();
        switch(index.column())
        {
            case 0: return salesmanVec.at(row).getaccount();
            case 1:return salesmanVec.at(row).getpassword();
        }
    }
    return QVariant();
}

QVariant salesmantable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role==Qt::DisplayRole&&orientation==Qt::Horizontal)
        return header[section];
    return QAbstractTableModel::headerData(section,orientation,role);
}

void salesmantable::populateModel()
{
    header<<tr("账号")<<tr("密码");
}

bool salesmantable::accisSameOrNot(QString acc,QList<salesman> data)
{
    for(int i=0;i<data.size();i++)
    {
        if(acc==data.at(i).getaccount())
            return true;
    }
    return false;
}

bool salesmantable::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        QJsonObject js;
        qHttpTool.setIP(csIP);
        qHttpTool.setPort(csPort);
        qHttpTool.setData(js);
        qHttpTool.setRouting("/user/findAllU");
        qHttpTool.bindCallback(interfaceJump);
        qHttpTool.GET(0);
        const int row = index.row();
        QString editInputAcc,editInputPassw;
        QString account,password;
        int id;
        account=salesmanVec.at(row).getaccount();
        password=salesmanVec.at(row).getpassword();
        id=salesmanVec.at(row).getsID();
        switch(index.column())
        {
        case 0:
            editInputAcc=value.toString().trimmed();
            if(editInputAcc.isEmpty())
            {
//                QApplication::setQuitOnLastWindowClosed(false);
                QMessageBox::warning(NULL, tr("提示"), tr("账号不能为空"), QMessageBox::Yes);
            }
            else if(accisSameOrNot(editInputAcc,salesmanVec))
            {
//                QApplication::setQuitOnLastWindowClosed(false);
                QMessageBox::warning(NULL, tr("提示"), tr("账号不能重复"), QMessageBox::Yes);
            }
            else
            {
//                QApplication::setQuitOnLastWindowClosed(false);
                QMessageBox::StandardButton box;
                box = QMessageBox::question(NULL, "提示", "确定要修改吗?", QMessageBox::Yes|QMessageBox::No);
                QJsonObject js;
                js.insert("id",id);
                js.insert("account",editInputAcc);
                js.insert("password",password);
                qHttpTool.setIP(csIP);
                qHttpTool.setPort(csPort);
                qHttpTool.setData(js);
                qHttpTool.setRouting("/user/updateSaler");
                qHttpTool.bindCallback(interfaceJump);
                qHttpTool.POST(1);
                if(box==QMessageBox::Yes)
                    salesmanVec[row].setaccount(value.toString());
            }
            break;
        case 1:
            editInputPassw=value.toString().trimmed();
            if(editInputPassw.isEmpty())
            {
//                QApplication::setQuitOnLastWindowClosed(false);
                QMessageBox::warning(NULL, tr("提示"), tr("密码不能为空"), QMessageBox::Yes);
            }
            else
            {
//                QApplication::setQuitOnLastWindowClosed(false);
                QMessageBox::StandardButton box;
                box = QMessageBox::question(NULL, "提示", "确定要修改吗?", QMessageBox::Yes|QMessageBox::No);
                QJsonObject js;
                js.insert("id",id);
                js.insert("account",account);
                js.insert("password",editInputPassw);
                qHttpTool.setIP(csIP);
                qHttpTool.setPort(csPort);
                qHttpTool.setData(js);
                qHttpTool.setRouting("/user/updateSaler");
                qHttpTool.bindCallback(interfaceJump);
                qHttpTool.POST(1);
                if(box==QMessageBox::Yes)
                    salesmanVec[row].setpassword(editInputPassw);
            }
            break;
        }
        //发送信号触发刷新
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

void salesmantable::getAllU(QList<salesman> data)
{
    salesmanVec=data;
}

void salesmantable::jump0()
{
    QMessageBox::warning(NULL , tr("提示"), tr("修改成功"), QMessageBox::Yes);
}

void salesmantable::jump1()
{
    QMessageBox::warning(NULL , tr("提示"), tr("修改成功"), QMessageBox::Yes);

}
Qt::ItemFlags salesmantable::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    else if(index.row()==0)
        return Qt::ItemIsEnabled|Qt::ItemIsSelectable;
    else
        return Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsEditable;
}

bool salesmantable::insertRows(int row, int count, const QModelIndex &parent)
{
    if(row<0||count<1||row>rowCount())
        return false;
    //需要将操作放到beginInsertRows和endInsertRows两个函数调用之间
    beginInsertRows(parent, row, row + count - 1);
    for(int i=row;i<row+count;i++)
    {
        //在接口对应行插入空数据
        salesmanVec.insert(i,salesman());
    }
    endInsertRows();
    return true;
}

bool salesmantable::removeRows(int row, int count, const QModelIndex &parent)
{
    if(row<0||count<1||row+count>rowCount())
        return false;
    //需要将操作放到beginRemoveRows和endRemoveRows两个函数调用之间
    beginRemoveRows(parent, row, row + count - 1);
    for(int i=row+count-1;i>=row;i--)
    {
        //移除该行数据
        salesmanVec.removeAt(i);
    }
    endRemoveRows();
    return true;
}
