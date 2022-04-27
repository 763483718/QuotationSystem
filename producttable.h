#ifndef PRODUCTTABLE_H
#define PRODUCTTABLE_H
#include <QAbstractTableModel>
#include <QStandardItemModel>
#include <QStringList>
#include "products.h"
#include "qexceltool.h"
#include "waitingdialog.h"

class producttable:public QAbstractTableModel //QAbstractTableModel
{
    Q_OBJECT
public:
    explicit producttable(QObject *parent = 0);
    ~producttable();
    //自定义导入导出数据的接口
    void setModelData();
    void setModelData1(const QList<products> &datas,int row);
    void setModelData(QList<products> &datas);
    void setModelData(QList<products> &datas,const int row);
    QList<products> getModelData();// const
    int getModelRows();
    //自定义插入行数据
    bool insertModelData(int row,const products &datas);

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

    int rowitem;
    int tableRows;
    QString operation;
    QList<products> productVec;
    static producttable* pproducttable;
    static QList<products> * pproductVec;

signals:

public slots:
    void getAllP(QList<products>,int row);
    void AddNewP(QList<products>,int row,products newdata);
    void AddNewItem(products p,QList<products> data,int row);
    void editP(products p,QList<products>,int row);
    void deleteP(products p,QList<products>,int row);
    void adjustP(int adjustValue);
    void dealSignals(int,int);//处理信号

private:
    QStringList header;//表格头部内容
    void populateModel();
    //bool* const flag = new bool(true);
};

#endif // PRODUCTTABLE_H
