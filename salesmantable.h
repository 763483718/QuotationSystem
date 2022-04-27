#ifndef SALESMANTABLE_H
#define SALESMANTABLE_H
#include <QAbstractTableModel>
#include <QVector>
#include <QMap>
#include <QStringList>
#include "salesman.h"
#include "qhttptool.h"
#include <QMessageBox>
#include <QApplication>

class salesmantable:public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit salesmantable(QObject *parent = 0);
    //自定义导入导出数据的接口
    void setModelData(const QList<salesman> &datas);
    void setModelData();
    QList<salesman> getModelData() const;
    //自定义插入行数据
    bool insertModelData(int row,const salesman &datas);

    //获取表格的行数
    virtual int rowCount(const QModelIndex &parent=QModelIndex()) const override;
    //获取表格的列数
    virtual int columnCount(const QModelIndex &parent=QModelIndex()) const override;
    //获取某一格子的内容
    QVariant data(const QModelIndex &index, int role) const override;
    //返回某一列表头的内容
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    //设置单元格数据
    bool setData(const QModelIndex &index, const QVariant &value,int role = Qt::EditRole) override;
    //单元格的可操作性标志位，如可编辑，可选中
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    //添加行列
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    //bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

    //移除行列
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    //bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;
    static void interfaceJump(QJsonObject info,int requestCode, QJsonValue data)
    {

        if(info.value("statusCode").toInt() == 200){//right
            switch (requestCode)
            {
            case 0:
                if(data.toArray().size()>=0)
                {
                    QJsonArray salesmanlist=data.toArray();
                    const int row_count=salesmanlist.size();
                    QList<salesman> new_data;
                    for(int row=0;row<row_count;row++)
                    {
                        int id=salesmanlist.at(row)["id"].toInt();
                        QString account=salesmanlist.at(row)["account"].toString();
                        QString password=salesmanlist.at(row)["password"].toString();
                        new_data.push_back({id,account,password});
                    }
                   emit psalesmantable->getAllU(new_data);
                }
                else
                {
                }
                break;
            case 1:
//                cout<<"datadatadatadd"<<data.toBool()<<endl;
                if(data.toBool())
                    psalesmantable->jump0();
                else if(!data.toBool())
                {
                    QMessageBox::warning(NULL , tr("提示"), tr("修改失败，请重新修改"), QMessageBox::Yes);
                }
                break;
            case 2:
//                cout<<"datadatadatapwd"<<data.toBool()<<endl;
                if(data.toBool())
                    psalesmantable->jump1();
                else if(!data.toBool())
                {
                    QMessageBox::warning(NULL , tr("提示"), tr("修改失败，请重新修改"), QMessageBox::Yes);
                }
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
    void jump0();
    void jump1();
    bool accisSameOrNot(QString acc,QList<salesman> data);

signals:

public slots:
    void getAllU(QList<salesman>);

private:
    QList<salesman> salesmanVec;
    QStringList header;//表格头部内容
    void populateModel();
    static salesmantable* psalesmantable;
    QHttpTool qHttpTool;
};

#endif // SALESMANTABLE_H
