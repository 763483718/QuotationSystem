#include "producttable.h"
#include <QMessageBox>
#include <QApplication>
#include "mainwindow.h"
#include "buttondelegate.h"
#include <iostream>
#include <QTableView>

using namespace std;

//int* const producttable::productId=new int(0);
//int* const producttable::productItemId=new int(-1);
producttable* producttable::pproducttable=NULL;
QList<products> * producttable::pproductVec=NULL;

producttable::producttable(QObject *parent) :
    QAbstractTableModel(parent)//QStandardItemModel
{
    populateModel();
    producttable::pproducttable=this;
//    qHttpTool=new QHttpTool();
    MainWindow::qExcelTool->setHttpTool(MainWindow::qHttpTool);
    connect(MainWindow::qExcelTool, SIGNAL(signals_finished(int,int)), this, SLOT(dealSignals(int,int)));
//    connect(this,SIGNAL(getAllP(QList<products>,int row)),this,SLOT(getAllP(QList<products>,int row)));
//    connect(this,SIGNAL(AddNewP(QList<products>,int row)),this,SLOT(AddNewP(QList<products>,int row)));
//    connect(this,SIGNAL(deleteP(QList<products>,int row)),this,SLOT(deleteP(QList<products>,int row)));

}
producttable::~producttable()
{
//    delete pproducttable;
//    pproducttable=NULL;
}
void producttable::deleteP(products p,QList<products> data,int row)
{
    cout<<"deletep"<<endl;
    operation="delete";
    productVec=data;
    tableRows=row;
    if(MainWindow::isShowByID==false)
    {
        int a=MainWindow::qExcelTool->updateProducts(MainWindow::currentBlock,productVec);
    }
    else
    {
        int blocknum=(p.getp_id()-1)/50;
        MainWindow::qExcelTool->getProducts(blocknum,productVec);
        productVec[p.getp_id()-blocknum*50-1]=p;
        int a=MainWindow::qExcelTool->updateProducts(blocknum,productVec);
    }
    qDebug()<<"delete update result "<<productVec.size();
    MainWindow::pQwait->show();
}

//add product 是 insert
void producttable::AddNewP(QList<products> data,int row, products newdata)
{
    operation="AddNewP";
//    MainWindow::pmainWindow->setData(data);
    int res=MainWindow::qExcelTool->insertProduct(newdata);
    cout<<"addnewp"<<res<<endl;
//    connect(MainWindow::qExcelTool, SIGNAL(signals_finished(int,int)), this, SLOT(dealSignals(int,int)));
    MainWindow::pQwait->show();
//    data.push_back(newdata);
//    row++;
    productVec=data;
    tableRows=row;
//    int row_count=data.size();
//    cout<<"produc_row_count="<<row_count<<endl;
//    int item_row_count;
//    int count=0;
////    MainWindow::ptv_products->setSpan(0,5,4,1);
//    for(int i=0;i<row_count;i++)
//    {
//        item_row_count=data.at(i).getp_productItem().size();
////        cout<<"item_row_count"<<item_row_count<<endl;
//        MainWindow::ptv_products->setSpan(count,0,item_row_count,1);
//        MainWindow::ptv_products->setSpan(count,1,item_row_count,1);
//        MainWindow::ptv_products->setSpan(count,2,item_row_count,1);
//        MainWindow::ptv_products->setSpan(count,3,item_row_count,1);

////        MainWindow::ptv_products->setSpan(count,11,item_row_count,1);
//        MainWindow::ptv_products->setSpan(count,12,item_row_count,1);
//        MainWindow::ptv_products->setSpan(count,13,item_row_count,1);
//        MainWindow::ptv_products->setSpan(count,14,item_row_count,1);
//        MainWindow::ptv_products->setSpan(count,15,item_row_count,1);

//        count+=item_row_count;
//    }
//    setModelData(productVec,tableRows);
}

//add item 是 update
void producttable::AddNewItem(products p,QList<products> data,int row)
{
    operation="AddNewItem";
    if(MainWindow::isShowByID==false)
    {
        int a=MainWindow::qExcelTool->updateProducts(MainWindow::currentBlock,data);
        MainWindow::pQwait->show();
        productVec=data;
        tableRows=row;
    //    setModelData(data,row);
//        int row_count=data.size();
//        cout<<"row_count="<<row_count<<endl;
//        int item_row_count;
//        int count=0;
//    //    MainWindow::ptv_products->setSpan(0,5,4,1);
//        for(int i=0;i<row_count;i++)
//        {
//            item_row_count=data.at(i).getp_productItem().size();
//    //        cout<<"item_row_count"<<item_row_count<<endl;
//            MainWindow::ptv_products->setSpan(count,0,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,1,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,2,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,3,item_row_count,1);

//    //        MainWindow::ptv_products->setSpan(count,11,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,12,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,13,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,14,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,15,item_row_count,1);

//            count+=item_row_count;
//        }
    }
    else
    {
        int blocknum=(p.getp_id()-1)/50;
        MainWindow::qExcelTool->getProducts(blocknum,productVec);
        productVec[p.getp_id()-blocknum*50-1]=p;
        int a=MainWindow::qExcelTool->updateProducts(blocknum,productVec);
        MainWindow::pQwait->show();
        productVec=data;
        tableRows=row;
    //    setModelData(data,row);
//        int row_count=data.size();
//        cout<<"row_count="<<row_count<<endl;
//        int item_row_count;
//        int count=0;
//    //    MainWindow::ptv_products->setSpan(0,5,4,1);
//        for(int i=0;i<row_count;i++)
//        {
//            item_row_count=data.at(i).getp_productItem().size();
//    //        cout<<"item_row_count"<<item_row_count<<endl;
//            MainWindow::ptv_products->setSpan(count,0,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,1,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,2,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,3,item_row_count,1);

//    //        MainWindow::ptv_products->setSpan(count,11,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,12,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,13,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,14,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,15,item_row_count,1);

//            count+=item_row_count;
//        }
    }
}

void producttable::editP(products p,QList<products> data,int row)
{
    operation="editP";
    productVec=data;
    tableRows=row;
//    MainWindow::pmainWindow->setData(data);
//    setModelData(data,row);
    if(MainWindow::isShowByID==false)
    {
        int a=MainWindow::qExcelTool->updateProducts(MainWindow::currentBlock,productVec);
        qDebug()<<"edit update result "<<a;
    }
    else
    {
        int blocknum=(p.getp_id()-1)/50;
        MainWindow::qExcelTool->getProducts(blocknum,productVec);
        productVec[p.getp_id()-blocknum*50-1]=p;
        int a=MainWindow::qExcelTool->updateProducts(blocknum,productVec);
        qDebug()<<"edit update result "<<a;
    }

//    connect(MainWindow::qExcelTool, SIGNAL(signals_finished(int,int)), this, SLOT(dealSignals(int,int)));
    MainWindow::pQwait->show();
}

void producttable::adjustP(int adjustValue)
{
    operation="adjustP";
    MainWindow::qExcelTool->batchAdjustment(adjustValue);
    MainWindow::pQwait->show();
    for(int i=0;i<productVec.size();i++)
    {
        QList<productItem> item=productVec[i].getp_productItemNotConst();
        for(int j=0;j<item.size();j++)
        {
            float price=item[j].getp_price();
            item[j].setp_price(price*= (100 + adjustValue)*0.01);//*adjustValue);
        }
        productVec[i].setp_productItem(item);
    }
//    setModelData(productVec,tableRows);
    qDebug()<<"adjustP result ";
}

void producttable::dealSignals(int OperationNumber,int block)
{
    MainWindow::pQwait->close();
//    cout<<"product"<<OperationNumber<<"\t"<<block<<endl;

    //获取版本号出错
    if(OperationNumber== 0x100)
    {

    }
    //上传文件错误
    else if(OperationNumber== 0x200) //512
    {
        MainWindow::pQwait->close();
//        QApplication::setQuitOnLastWindowClosed(false);
        QMessageBox::warning(NULL , tr("提示"), tr("操作失败"), QMessageBox::Yes);
    }
    //上传文件全部完成
    else if(OperationNumber== 0x201) //513
    {
        if(MainWindow::isShowByID==false)
        {
            MainWindow::pQwait->close();
            MainWindow::pproduct_data=&productVec;
            pproductVec=&productVec;
            MainWindow::productsSize=productVec.size();
            if(operation=="editP")
                setModelData(productVec,tableRows);
            else if(operation=="AddNewItem")
                setModelData(productVec,tableRows);
            else if(operation=="AddNewP")
            {
                setModelData(productVec,tableRows);
                imgPathDelegate::pImg_list->push_back(productVec.at(productVec.size()-1).getp_imagePath());
    //            insertModelData(tableRows,productVec[productVec.size()-1]);
            }
            else if(operation=="delete")
            {
                setModelData(productVec,tableRows);
                MainWindow::pmainWindow->setHide();
            }
        }
        else
        {
            if(operation=="AddNewP")
            {
                setModelData(productVec,tableRows);
                imgPathDelegate::pImg_list->push_back(productVec.at(productVec.size()-1).getp_imagePath());
    //            insertModelData(tableRows,productVec[productVec.size()-1]);
            }
        }

        MainWindow::ptv_products->clearSpans();

        int row_count=productVec.size();
//        cout<<"row_count="<<row_count<<endl;
        int item_row_count;
        int count=0;
    //    MainWindow::ptv_products->setSpan(0,5,4,1);
        for(int i=0;i<row_count;i++)
        {
            item_row_count=productVec.at(i).getp_productItem().size();
//            cout<<"item_row_count"<<item_row_count<<endl;
            MainWindow::ptv_products->setSpan(count,0,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,1,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,2,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,3,item_row_count,1);

    //        MainWindow::ptv_products->setSpan(count,11,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,12,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,13,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,14,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,15,item_row_count,1);

            count+=item_row_count;
        }

    }
    //上传图片错误
    else if(OperationNumber==0x202)
    {

    }
    //上传的产品全部重复
    else if(OperationNumber==0x203)
    {
        QMessageBox::warning(NULL , tr("提示"), tr("导入的产品全部重复，新增失败"), QMessageBox::Yes);
    }
    //下载文件错误
    else if(OperationNumber== 0x300)
    {

    }
    //下载文件全部完成
    else if(OperationNumber== 0x301)
    {
            }
    //无需更新
    else if(OperationNumber== 0x302)
    {
            }
    //获取图片名称错误
    else if(OperationNumber== 0x303)
    {
        QMessageBox::warning(NULL , tr("提示"), tr("下载图片失败"), QMessageBox::Yes);
    }
    //替换图片错误
    else if(OperationNumber== 0x304)
    {

    }
    //替换图片成功
    else if(OperationNumber== 0x305)
    {

    }
    else if(OperationNumber== 0x400)
    {
        MainWindow::pproduct_data=&productVec;
        pproductVec=&productVec;
        MainWindow::productsSize=productVec.size();
        setModelData(productVec,tableRows);
    }
    else if(OperationNumber==0x401)
    {
        productVec=getModelData();
        QMessageBox::warning(NULL , tr("提示"), tr("调价失败"), QMessageBox::Yes);
    }
    MainWindow::pQwait->close();
}

void producttable::getAllP(QList<products> data,int row)
{
    productVec=data;
    setModelData(data,row);
    int row_count=data.size();
//    cout<<"row_count="<<row_count<<endl;
    int item_row_count;
    int count=0;
//    MainWindow::ptv_products->setSpan(0,5,4,1);
    for(int i=0;i<row_count;i++)
    {
        item_row_count=data.at(i).getp_productItem().size();
//        cout<<"item_row_count"<<item_row_count<<endl;
        MainWindow::ptv_products->setSpan(count,0,item_row_count,1);
        MainWindow::ptv_products->setSpan(count,1,item_row_count,1);
        MainWindow::ptv_products->setSpan(count,2,item_row_count,1);
        MainWindow::ptv_products->setSpan(count,3,item_row_count,1);

        MainWindow::ptv_products->setSpan(count,11,item_row_count,1);
        MainWindow::ptv_products->setSpan(count,12,item_row_count,1);
        MainWindow::ptv_products->setSpan(count,13,item_row_count,1);
        MainWindow::ptv_products->setSpan(count,14,item_row_count,1);
        MainWindow::ptv_products->setSpan(count,15,item_row_count,1);

        count+=item_row_count;
    }
}
void producttable::setModelData()
{
    beginResetModel();
    endResetModel();
}
void producttable::setModelData1(const QList<products> &datas,int row)
{
    beginResetModel();
    cout<<"setModelData1"<<endl;
    productVec=datas;
    tableRows=row;
    endResetModel();
}
void producttable::setModelData(QList<products> &datas)
{
    beginResetModel();
    productVec=datas;
    endResetModel();
}
void producttable::setModelData(QList<products> &datas,const int row)
{
    beginResetModel();
    productVec=datas;
    tableRows=row;
    endResetModel();
}
QList<products> producttable::getModelData()
{
    return productVec;
}

int producttable::getModelRows()
{
    return tableRows;
}

bool producttable::insertModelData(int row, const products &datas)
{
    //row为0就是开始，为rowcount就在尾巴
    if(row<0||row>rowCount())
        return false;
    //需要将操作放到beginInsertRows和endInsertRows两个函数调用之间
    beginInsertRows(QModelIndex(), row, row);
    //在接口对应行插入空数据
    productVec.insert(row,datas);
    endInsertRows();
    emit dataChanged(index(0,0),index(rowCount(),columnCount()-1),QVector<int>());
    return true;
}

int producttable::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return 16;
}

int producttable::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    if(MainWindow::isShowByID)
    {
//        cout<<"MainWindow::isShowByID"<<endl;
        int count;
        int distance;
        if(MainWindow::totalpages==1)
        {
//            cout<<"MainWindow::totalpages==1"<<endl;
            count=0;
            for(int i=0;i<MainWindow::resultSize;i++)
            {
                int size=MainWindow::result_data->at(i).getItemsSize();
                count+=size;
            }
//            cout<<"count="<<count<<endl;
            return count;
        }
        else if(MainWindow::totalpages>1)
        {
//            cout<<"MainWindow::totalpages>1"<<endl;
            count=0;
            distance=MainWindow::resultSize-MainWindow::currentPage*50;
            if(distance>50)
            {
                for(int i=0;i<50;i++){
                    int size = productVec.at(i).getItemsSize();
                    count += size;
                }
//                cout<<"count="<<count<<endl;
                return count;
            }
            else {
                for(int i=0;i<productVec.size();i++){
                    int size = productVec.at(i).getItemsSize();
                    count += size;
                }
//                cout<<"count="<<count<<endl;
                return count;
            }
        }
        else
            return 0;
    }
    else
    {
//        cout<<"MainWindow::isNotShowByID"<<endl;
        int count;
        int distance;
        if(MainWindow::blockSize==1)
        {
            count=0;
            for(int i=0;i<MainWindow::productsSize;i++)
            {
                int size=productVec.at(i).getItemsSize();
                count+=size;
            }
//            cout<<"count="<<count<<endl;
            return count;
        }
        else if(MainWindow::blockSize>1)
        {
            count=0;
            distance=MainWindow::productsSize-MainWindow::currentBlock*50;
            if(distance>50)
            {
                for(int i=0;i<50;i++){
                    int size = productVec.at(i).getItemsSize();
                    count += size;
                }
//                cout<<"count="<<count<<endl;
                return count;
            }
            else {
                for(int i=0;i<productVec.size();i++){
                    int size = productVec.at(i).getItemsSize();
                    count += size;
                }
//                cout<<"count="<<count<<endl;
                return count;
            }
        }
        else
            return 0;
    }
}

QVariant producttable::data(const QModelIndex &index, int role= Qt::DisplayRole) const
{
//delete verion
//    int row=index.row();
//    int productId = 0;
//    int productItemId = 0;
//    int count = 0;
//    for(int i=0;i<productVec.size();i++){
//        int size = productVec.at(i).getItemsSize();
//        int deletedSize = 0;
//        if(1){
//            for(int j = 0;j < size;j++){
//                if(productVec.at(i).getp_productItem().at(j).getp_itemIsDelete()){
//                    deletedSize++;
//                }
//            }
//        }

//        if(count + size - deletedSize >= row+1){
//            productId = i;
//            productItemId = row - count + deletedSize;
//            break;
//        }
//        count += size;
//        count -= deletedSize;
//    }


    const int row=index.row();
    int productId = 0;
    int productItemId = 0;
    int count = 0;
    for(int i=0;i<productVec.size();i++){
        int size = productVec.at(i).getp_productItem().size();
        if(count + size >= row+1){
            productId = i;
            productItemId = row - count;
            break;
        }
        count += size;
    }


    if(!index.isValid())
        return QVariant();

    if(role==Qt::BackgroundColorRole)
    {
        int row=index.row();
        int col=index.column();
//        cout<<"isDelete delete"<<productVec[productId].getp_productItem()[productItemId].getp_itemIsDelete()<<endl;
        if(productVec[productId].getp_productItem()[productItemId].getp_itemIsDelete()&&3<col&&col<11)
        {
//            cout<<"return color"<<endl;
            return QColor(Qt::gray);
//            return QVariant();
        }
        else if(productVec[productId].getp_isDelete())
        {
//            cout<<"return color"<<endl;
            return QColor(Qt::gray);
//            return QVariant();
        }
        else
            return QVariant();

    }
//    if(role==Qt::ItemDataRole())
//    {

//    }
    if(role==Qt::DisplayRole)
    {
//#if 1
//    //提姆零号算法！！！！！！
//    const int row=index.row();
//    int* const productId = new int(0);
//    int* const productItemId = new int(0);
//    int count = 0;
//    for(int i=0;i<productVec.size();i++){
//        int size = productVec.at(i).getp_productItem().size();
//        if(count + size >= row+1){
//            *productId = i;
//            *productItemId = row - count;
//            break;
//        }
//        count += size;
//    }

//        *productItemId += 1;
//        if(*productItemId == productVec.at(*productId).getp_productItem().size()){
//            *productItemId = 0;
//            *productId += 1;
//        }

//        if(*productId >= 5){
//            *productId = 0;
//            *productItemId = 0;
//        }
//        cout<<*productItemId<<"\tproductId:"<<*productId<<endl;
//#endif
        products p;
        switch(index.column())
        {
        case 0: return productVec.at(productId).getp_id();break;
        case 1: return productVec.at(productId).getp_imagePath();break;
        case 2: return productVec.at(productId).getp_brand();break;
        case 3: return productVec.at(productId).getp_name();break;
        case 4: return productVec.at(productId).getp_productItem().at(productItemId).getp_type();break;//getp_productItem().size();//)[productItemId].getp_type();
        case 5:
            if(productVec.at(productId).getp_productItem().at(productItemId).getp_price()!=-1)
                return productVec.at(productId).getp_productItem().at(productItemId).getp_price();
            break;
        case 6: return productVec.at(productId).getp_productItem().at(productItemId).getp_source();break;
        case 7: return productVec.at(productId).getp_productItem().at(productItemId).getp_material();break;
        case 8: return productVec.at(productId).getp_productItem().at(productItemId).getp_color();break;
        case 9:
            if(productVec.at(productId).getp_productItem().at(productItemId).getp_MOQ()!="")
                return productVec.at(productId).getp_productItem().at(productItemId).getp_MOQ();
            break;
        case 10:
            if(productVec.at(productId).getp_productItem().at(productItemId).getp_MPQ()!="")
                return productVec.at(productId).getp_productItem().at(productItemId).getp_MPQ();
            break;
        case 12:
            if(productVec.at(productId).getp_hole()!="")
                return productVec.at(productId).getp_hole();
            break;
        case 13: return productVec.at(productId).getp_fitting();break;
        case 14:return productVec.at(productId).getp_remark();break;
        //case 12: return productVec.at(row).getaccount();
        }
    }
    if(role==Qt::EditRole)
    {
        switch(index.column())
        {
            //case 1:return productVec.at(row).getpassword();
        }
    }
    return QVariant();
}

QVariant producttable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role==Qt::DisplayRole&&orientation==Qt::Horizontal)
        return header[section];
    return QAbstractTableModel::headerData(section,orientation,role);
}

void producttable::populateModel()
{
    header<<tr("id")<<tr("图片")<<tr("品牌")<<tr("中文名称")<<tr("型号")<<tr("价格")<<tr("来源")<<tr("材料")<<tr("颜色")<<tr("MOQ")<<tr("MPQ")<<tr("操作")<<tr("孔位")<<tr("配件型号")<<tr("备注")<<tr("操作");
}

bool producttable::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        const int row = index.row();
        switch(index.column())
        {
        //case 0: salesmanVec[row].setaccount(value.toString()); break;
        case 1:
            QString editInputPassw=value.toString().trimmed();
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
                //if(box==QMessageBox::Yes)
                    //productVec[row].setpassword(editInputPassw);
            }
            break;
        }
        //发送信号触发刷新
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags producttable::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    else
//        return Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsEditable;
        return Qt::ItemIsEnabled|Qt::ItemIsSelectable;
}

bool producttable::insertRows(int row, int count, const QModelIndex &parent)
{
    //row为0就是开始，为rowcount就在尾巴
    if(row<0||count<1||row>rowCount())
        return false;
    //需要将操作放到beginInsertRows和endInsertRows两个函数调用之间
    beginInsertRows(parent, row, row + count - 1);
    for(int i=row;i<row+count;i++)
    {
        //在接口对应行插入空数据
        productVec.insert(i,products());
    }
    endInsertRows();
    return true;
}

bool producttable::removeRows(int row, int count, const QModelIndex &parent)
{
    if(row<0||count<1||row+count>rowCount())
        return false;
    //需要将操作放到beginRemoveRows和endRemoveRows两个函数调用之间
    beginRemoveRows(parent, row, row + count - 1);
    for(int i=row+count-1;i>=row;i--)
    {
        //移除该行数据
        productVec.removeAt(i);
    }
    endRemoveRows();
    return true;
}
