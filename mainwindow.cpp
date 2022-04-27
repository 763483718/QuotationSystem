#include "mainwindow.h"
#include "managewindow.h"
#include "ui_mainwindow.h"
#include <QDialog>
#include <QMessageBox>
#include <QTableView>
#include <QRandomGenerator>
#include <QScrollBar>
#include <QFileDialog>
#include <QDebug>
#include <QStandardItemModel>
#include "producttable.h"
#include "editpasswdialog.h"
#include "buttondelegate.h"
#include "logindialog.h"
//#include "qhttptool.h"
#include "logindialog.h"
#include "qexceltool.h"
#include "QHBoxLayout"
#include "QStandardPaths"
#include "adjustpricedialog.h"

QHttpTool* MainWindow::qHttpTool=NULL;
QHttpTool* MainWindow::qHttpToolforset=NULL;
QExcelTool* MainWindow::qExcelTool=NULL;
WaitingDialog *MainWindow::pQwait=NULL;
producttable* MainWindow::pproducttable=NULL;
MainWindow* MainWindow::pmainWindow=NULL;
bool MainWindow::isShowByID=false;
bool MainWindow::image_load_error=false;
bool MainWindow::isShowDeleted=false;
QTableView* MainWindow::ptv_products=NULL;
int MainWindow::blockSize=0;
int MainWindow::productsSize=0;
int MainWindow::currentBlock=0;
int MainWindow::totalpages=0;
int MainWindow::currentPage=0;
int MainWindow::resultSize=0;
int MainWindow::idSize=0;
QPushButton* MainWindow::pPageOn=NULL;
QPushButton* MainWindow::pPageUp=NULL;
QLineEdit* MainWindow::pPageTo=NULL;
QLabel* MainWindow::pTotalPages=NULL;
QList<QProduct>* MainWindow::pproduct_data=NULL;
QList<QProduct>* MainWindow::result_data=NULL;
QList<int>* MainWindow::pproduct_ids=NULL;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("首页");
    qHttpTool=new QHttpTool();
    qHttpToolforset=new QHttpTool();
    qExcelTool=new QExcelTool();
    MainWindow::pmainWindow=this;
    connect(qExcelTool, SIGNAL(signals_finished(int,int)), this, SLOT(dealSignals(int,int)));
    connect(ui->cB_showDeleted, SIGNAL(stateChanged(int)), this, SLOT(on_pB_showDeleted_stateChanged(int)));
    connect(this,SIGNAL(displayData(QList<products>)),this,SLOT(displayData(QList<products>)));
    connect(this, SIGNAL(stateChanged(int)), this, SLOT(on_pB_showDeleted_stateChanged(int)));

//    productsmodel=new producttable(ui->tV_products);
//    pQwait=new WaitingDialog();
//    qExcelTool.setHttpTool(qHttpTool);
}

MainWindow::~MainWindow()
{
//    verScrollBar->disconnect();
    this->disconnect();
    delete ui;
}

void MainWindow::init()
{
    qHttpTool=new QHttpTool();
    qHttpToolforset=new QHttpTool();
    productsmodel=new producttable(ui->tV_products);
    pQwait=new WaitingDialog();
    pQwait->setWindowFlags(Qt::WindowStaysOnTopHint);
    qExcelTool->setHttpTool(qHttpToolforset);
    //隐藏列序号
    ui->tV_products->verticalHeader()->hide();
    ui->tV_products->setModel(productsmodel);
    MainWindow::pmainWindow->setHide();
    cout<<"22abcdksdfiji:"<<endl;
//    ui->tV_products->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//所有列设置自动列宽
    ui->tV_products->setSelectionMode(QAbstractItemView::SingleSelection);    //设置只能选单行
    ui->tV_products->setSelectionBehavior(QAbstractItemView::SelectRows);     //设置只能选中行
    this->setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    ui->tV_products->setMinimumWidth(80);
    ui->tV_products->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);//对第0列单独设置固定宽度
//    ui->tV_products->setColumnWidth(0, 30);//设置固定宽度
    ui->tV_products->setColumnWidth(9, 50);//设置固定宽度
    ui->tV_products->setColumnWidth(10, 50);//设置固定宽度
    ui->tV_products->setColumnWidth(12, 50);//设置固定宽度
    ui->tV_products->horizontalHeader()->setSectionResizeMode(14, QHeaderView::ResizeToContents);//remark列
    ui->tV_products->verticalHeader()->setDefaultSectionSize(120);//设置固定行高
    ui->tV_products->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->tV_products->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    verScrollBar=(QScrollBar *)ui->tV_products->verticalScrollBar();//->setStyleSheet("QScrollBar:vertical");
    verScrollBar->setStyleSheet("QScrollBar:vertical");
    ui->tV_products->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal");
//    productsmodel->item(1,1)->setBackground(QColor(Qt::red));
    QWidget::setTabOrder(ui->lineEdit_type,ui->lineEdit_name);
    QWidget::setTabOrder(ui->lineEdit_name,ui->lineEdit_color);
    QWidget::setTabOrder(ui->lineEdit_color,ui->lineEdit_MPQ);
    QWidget::setTabOrder(ui->lineEdit_MPQ,ui->comboBox_brand);
    QWidget::setTabOrder(ui->comboBox_brand,ui->lineEdit_hole);
    QWidget::setTabOrder(ui->lineEdit_hole,ui->lineEdit_MOQ);
    QWidget::setTabOrder(ui->lineEdit_MOQ,ui->pB_search);
    ptv_products=ui->tV_products;
    int res=qExcelTool->CacheInit(loginDialog::isAdmin); //1是管理员
    pQwait->show();
}


void MainWindow::dealSignals(int OperationNumber,int block)
{

//    cout<<"mainwindow"<<OperationNumber<<block<<endl;
    importError=false;
    pQwait->close();
    //获取版本号出错
    if(OperationNumber== 0x100)
    {

    }
    //上传文件错误
    else if(OperationNumber== 0x200)
    {
//        QApplication::setQuitOnLastWindowClosed(false);
        QMessageBox::warning(NULL , tr("提示"), tr("导入失败"), QMessageBox::Yes);
        importError=true;

    }
    //上传文件全部完成
    else if(OperationNumber== 0x201)
    {
//        QMessageBox::warning(NULL , tr("提示"), tr("修改成功"), QMessageBox::Yes);
        if(isShowByID==false)
        {
            qExcelTool->getProducts(0,product_data);
            qExcelTool->getProductsAndBlocksSize(blockSize,productsSize);
            cout<<blockSize<<"s,s"<<currentBlock<<endl;
            isShowByID=false;
            if(blockSize==0 || blockSize==1)
                currentBlock=0;
            setData(currentBlock);
            //获取全部品牌
            if(productsSize>0)
            {
                QJsonObject js;
                std::unique_lock<std::mutex> lk(qHttpTool->mSendingMutex);
                qHttpTool->setIP(csIP);
                qHttpTool->setPort(csPort);
                qHttpTool->setData(js);
                qHttpTool->setRouting("/product/findAllBrand");
                qHttpTool->bindCallback(interfaceJump);
                qHttpTool->GET(1);
                lk.unlock();
            }
        }
        else
        {
            ids2Products(product_ids_record);
        }
    }
    //上传图片错误
    else if(OperationNumber==0x202)
    {

    }
    //新增产品重复
    else if(OperationNumber==0x203)
    {

    }
    //下载文件错误
    else if(OperationNumber== 0x300)
    {

    }
    //下载文件全部完成
    else if(OperationNumber== 0x301)
    {
        setData(0);
        pproducttable=productsmodel;
        ui->cB_showDeleted->setCheckState(Qt::Unchecked);
        MainWindow::pPageOn=ui->pB_PageOn;
        MainWindow::pPageTo=ui->lineEdit_currentPage;
        MainWindow::pPageUp=ui->pB_PageUp;
        MainWindow::pTotalPages=ui->label_totalPages;
        MainWindow::pmainWindow=this;
        ui->comboBox_brand->setEditable(true);
        qExcelTool->getProductsAndBlocksSize(blockSize,productsSize);
        //获取全部品牌
        if(productsSize>0)
        {
            QJsonObject js;
            std::unique_lock<std::mutex> lk(qHttpTool->mSendingMutex);
            qHttpTool->setIP(csIP);
            qHttpTool->setPort(csPort);
            qHttpTool->setData(js);
            qHttpTool->setRouting("/product/findAllBrand");
            qHttpTool->bindCallback(interfaceJump);
            qHttpTool->GET(1);
            lk.unlock();
        }
        //查看是否缺少图片
        qExcelTool->checkImages();
    }
    //无需更新
    else if(OperationNumber== 0x302)
    {
        setData(0);
        pproducttable=productsmodel;
        ui->cB_showDeleted->setCheckState(Qt::Unchecked);
        MainWindow::pPageOn=ui->pB_PageOn;
        MainWindow::pPageTo=ui->lineEdit_currentPage;
        MainWindow::pPageUp=ui->pB_PageUp;
        MainWindow::pTotalPages=ui->label_totalPages;
        MainWindow::pmainWindow=this;
        ui->comboBox_brand->setEditable(true);
        qExcelTool->getProductsAndBlocksSize(blockSize,productsSize);
        //获取全部品牌
        if(productsSize>0)
        {
            QJsonObject js;
            std::unique_lock<std::mutex> lk(qHttpTool->mSendingMutex);
            qHttpTool->setIP(csIP);
            qHttpTool->setPort(csPort);
            qHttpTool->setData(js);
            qHttpTool->setRouting("/product/findAllBrand");
            qHttpTool->bindCallback(interfaceJump);
            qHttpTool->GET(1);
            lk.unlock();
        }
        qExcelTool->checkImages();
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
    //下载图片失败
    else if(OperationNumber == DOWNLOAD_IMAGE_ERROR)
    {
        image_load_error=true;
    }
}

//初始查询cache显示、批量导入显示
void MainWindow::setData(int blockDisplay)
{
    //获取全部产品
    qExcelTool->getProductsAndBlocksSize(blockSize,productsSize);
    totalpages=blockSize;
    ui->label_totalPages->setText(QString::number(blockSize));
    if(blockSize==0)
    {
        ui->pB_PageOn->setEnabled(false);
        ui->pB_PageUp->setEnabled(false);
        ui->lineEdit_currentPage->setText("0");
        productsmodel->setModelData();
        MainWindow::pmainWindow->setHide();
    }
    else
    {
        if(blockSize>1)
        {
            ui->pB_PageOn->setEnabled(true);
            ui->pB_PageUp->setEnabled(true);
        }

        ui->lineEdit_currentPage->setText(QString::number(blockDisplay+1));//"1"
        currentBlock=blockDisplay;
        qExcelTool->getProducts(blockDisplay,product_data);
//        producttable::pproducttable->setModelData(product_data);

        ui->tV_products->clearSpans();
        int row_count=product_data.size();
        int row=0;
        int item_row_count;
        for(int i=0;i<row_count;i++)
        {
            item_row_count=product_data.at(i).getp_productItem().size();
//            cout<<product_data.at(i).getp_id()<<"mainwindow item_row_count="<<item_row_count<<endl;
            ui->tV_products->setSpan(row,0,item_row_count,1);
            ui->tV_products->setSpan(row,1,item_row_count,1);
            ui->tV_products->setSpan(row,2,item_row_count,1);
            ui->tV_products->setSpan(row,3,item_row_count,1);

            ui->tV_products->setSpan(row,12,item_row_count,1);
            ui->tV_products->setSpan(row,13,item_row_count,1);
            ui->tV_products->setSpan(row,14,item_row_count,1);
            ui->tV_products->setSpan(row,15,item_row_count,1);
//            cout<<"row1"<<row<<endl;
            row+=item_row_count;
//            cout<<"row2"<<row<<endl;
        }

        if(loginDialog::isAdmin==0)
        {
            ui->pB_manage->hide();
            ui->pB_batchImport->hide();
            ui->pB_editAdminPassword->hide();
            ui->cB_showDeleted->hide();
            ui->pushButton_adjustPrice->hide();
            ui->tV_products->setColumnHidden(6,true);//来源
            ui->tV_products->setColumnHidden(11,true);//操作
            ui->tV_products->setColumnHidden(15,true);//操作
        }
        else
        {
            ui->pB_manage->show();
            ui->pB_batchImport->show();
            ui->pB_editAdminPassword->show();
            ui->cB_showDeleted->show();
            ui->tV_products->setColumnHidden(6,false);//来源
            ui->tV_products->setColumnHidden(11,false);//操作
            ui->tV_products->setColumnHidden(15,false);//操作
        }
        cout<<"sssssss"<<endl;
        //调用自定义接口设置model的数据
        int rowsize=producttable::pproducttable->getModelRows();
        productsmodel->setModelData(product_data,rowsize);
        MainWindow::pmainWindow->setHide();
//        producttable::pproducttable->setModelData(product_data,row);
        if(loginDialog::isAdmin==1)
        {
           p_buttonitem = new buttonItemDelegate(ui->tV_products);
           ui->tV_products->setItemDelegateForColumn(11, p_buttonitem);//列插入按钮
           p_buttons = new buttonDelegate(ui->tV_products);
           ui->tV_products->setItemDelegateForColumn(15, p_buttons);//列插入按钮
        }
        cout<<"fffffff"<<endl;
        p_imgpath_buttons=new imgPathDelegate(ui->tV_products);
        ui->tV_products->setItemDelegateForColumn(1,p_imgpath_buttons);

//        emit productsmodel->layoutChanged();
    }        
}

void MainWindow::setHide()
{
    product_data=producttable::pproducttable->getModelData();
    //未选中，若item is delete，就hide
    if(!isShowDeleted)
    {
        int rowIndex=0;
        bool itemIsDelete;
        for(int i=0;i<product_data.size();i++)
        {
            for(int j=0;j<product_data[i].getItemsSize();j++)
            {
                itemIsDelete=product_data[i].getp_productItem()[j].getp_itemIsDelete();
                if(itemIsDelete)
                {
                    ui->tV_products->setRowHidden(rowIndex,true);
                    rowIndex++;
                }
                else
                {
                    ui->tV_products->setRowHidden(rowIndex,false);
                    rowIndex++;
                }
            }
        }
    }
    //选中，不隐藏
    else
    {
//        int row=producttable::pproducttable->getModelRows();
        int rowIndex=0;
        for(int i=0;i<product_data.size();i++)
        {
            for(int j=0;j<product_data[i].getItemsSize();j++)
            {
                ui->tV_products->setRowHidden(rowIndex,false);
                rowIndex++;
            }
        }
    }
}

QList<products> MainWindow::getData()
{
    return productsmodel->getModelData();
}
void MainWindow::addRow()
{

}
void MainWindow::delRow()
{

}

void MainWindow::on_pB_manage_clicked()
{
    managewindow *manage=new managewindow(this);
    manage->show();
    connect(manage,SIGNAL(manage()),this,SLOT(showmanage()));
//    connect(this,SIGNAL(changeUItest(QList<salesman>)),manage,SLOT(DisplayData(QList<salesman>)));
//    this->hide();
}

void MainWindow::showmanage()
{
    this->show();
}

void MainWindow::on_pB_editAdminPassword_clicked()
{
    editPasswDialog *editpassw=new editPasswDialog(this);
    editpassw->show();
    connect(editpassw,SIGNAL(editpassw()),this,SLOT(showeditpassw()));
//    this->hide();
}

void MainWindow::showeditpassw()
{
    this->show();
}

void MainWindow::on_pB_search_clicked()
{
//    ui->comboBox_brand->setUpdatesEnabled(false);
    ui->comboBox_brand->setCurrentIndex(ui->comboBox_brand->currentIndex());
    isShowByID=true;
    qDebug()<<"search";
    QString stype;
    if(ui->lineEdit_type->text().isNull())
        stype="";
    else
        stype=ui->lineEdit_type->text();
    QString sname;
    if(ui->lineEdit_name->text().isNull())
        sname="";
    else
        sname=ui->lineEdit_name->text();
    QString scolor;
    if(ui->lineEdit_color->text().isNull())
        scolor="";
    else
        scolor=ui->lineEdit_color->text();
    QString smoq;
    if(ui->lineEdit_MOQ->text().isNull())
        smoq="";
    else
        smoq=ui->lineEdit_MOQ->text();
    QString smpq;
    if(ui->lineEdit_MPQ->text().isNull())
        smpq="";
    else
        smpq=ui->lineEdit_MPQ->text();
    QString sbrand;
    if(ui->comboBox_brand->currentText()=="全部品牌")
        sbrand="";
    else
        sbrand=ui->comboBox_brand->currentText();
    QString shole;
    if(ui->lineEdit_hole->text().isNull())
        shole="";
    else
        shole=ui->lineEdit_hole->text();

    QJsonObject js;
    js.insert("brand",sbrand);
    js.insert("chineseName",sname);
    js.insert("color",scolor);
    js.insert("flag",loginDialog::isAdmin);
    js.insert("holeNum",shole);
    js.insert("model",stype);
    js.insert("moq",smoq);
    js.insert("mpq",smpq);
    qHttpTool->setData(js);
    std::unique_lock<std::mutex> lk(qHttpTool->mSendingMutex);
    qHttpTool->setIP(csIP);
    qHttpTool->setPort(csPort);
    qHttpTool->setRouting("/product/findP");
    qHttpTool->bindCallback(interfaceJump);
    qHttpTool->POST(0);
    lk.unlock();
}

//void MainWindow::displayData(int currentPage)
//{
//    ui->lineEdit_currentPage->setText(QString::number(currentPage));
//    MainWindow::currentPage=currentPage;
//    idsList=pproduct_ids->mid((currentPage-1)*50,50);
//    qExcelTool->getProducts(idsList,product_data);
//    //显示ids2pros
//    int rowsize=producttable::pproducttable->getModelRows();
//    producttable::pproducttable->setModelData(product_data,rowsize);
//    int row_count=product_data.size();
//    int row=0;
//    int item_row_count;
//    for(int i=0;i<row_count;i++)
//    {
//        item_row_count=product_data.at(i).getp_productItem().size();
////        ui->tV_products->setSpan(row,0,item_row_count,1);
//        ui->tV_products->setSpan(row,1,item_row_count,1);
//        ui->tV_products->setSpan(row,2,item_row_count,1);
//        ui->tV_products->setSpan(row,3,item_row_count,1);

////            ui->tV_products->setSpan(row,11,item_row_count,1);
//        ui->tV_products->setSpan(row,12,item_row_count,1);
//        ui->tV_products->setSpan(row,13,item_row_count,1);
//        ui->tV_products->setSpan(row,14,item_row_count,1);
//        ui->tV_products->setSpan(row,15,item_row_count,1);
//        row+=item_row_count;
//    }
//    cout<<"0row"<<row<<endl;

//    //调用自定义接口设置model的数据
//    productsmodel->setModelData(product_data,rowsize);
//    if(loginDialog::isAdmin==1)
//    {
//       p_buttonitem = new buttonItemDelegate(ui->tV_products);
//       ui->tV_products->setItemDelegateForColumn(11, p_buttonitem);//列插入按钮
//       p_buttons = new buttonDelegate(ui->tV_products);
//       ui->tV_products->setItemDelegateForColumn(15, p_buttons);//列插入按钮
//    }
//    p_imgpath_buttons=new imgPathDelegate(ui->tV_products);
//    ui->tV_products->setItemDelegateForColumn(1,p_imgpath_buttons);
//}

void MainWindow::ids2Products(QList<int> product_ids)
{
    product_ids_record=product_ids;
    setData(0);
    isShowByID=true;
    MainWindow::pproduct_ids=&product_ids;
    resultList.clear();
    qExcelTool->getProducts(product_ids,resultList);
    MainWindow::idSize=product_ids.size();
    MainWindow::resultSize=resultList.size();
//    cout<<"MainWindow::resultSize="<<MainWindow::resultSize<<endl;
    MainWindow::pproduct_data=&resultList;
    MainWindow::result_data=&resultList;
    MainWindow::totalpages=(resultList.size()%50 ? resultList.size()/50+1 : resultList.size()/50);
    MainWindow::pTotalPages->setText(QString::number(totalpages));
    MainWindow::isShowDeleted=false;
    int count=0;
    if(totalpages==0)
    {
        ui->pB_PageOn->setEnabled(false);
        ui->pB_PageUp->setEnabled(false);
        ui->lineEdit_currentPage->setText("0");
        producttable::pproducttable->setModelData();
    }
    else if(totalpages!=0)
    {
        if(resultList.size()>50)
        {
            ui->pB_PageOn->setEnabled(true);
            ui->pB_PageUp->setEnabled(true);
        }
        MainWindow::currentPage=1;
        ui->lineEdit_currentPage->setText("1");
        MainWindow::product_data=resultList.mid(0,50);
        producttable::pproducttable->setModelData(product_data);
        MainWindow::ptv_products->setModel(producttable::pproducttable);
        MainWindow::pmainWindow->setHide();
        int row_count=product_data.size();
        cout<<"id2p_row_count="<<row_count<<endl;
        int item_row_count;

        for(int i=0;i<row_count;i++)
        {
            item_row_count=product_data.at(i).getp_productItem().size();
    //        cout<<"item_row_count"<<item_row_count<<endl;
            MainWindow::ptv_products->setSpan(count,0,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,1,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,2,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,3,item_row_count,1);

//            MainWindow::ptv_products->setSpan(count,11,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,12,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,13,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,14,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,15,item_row_count,1);

            count+=item_row_count;
        }
    }
    producttable::pproducttable->setModelData1(product_data,count);
    MainWindow::pmainWindow->setHide();
//    pproduct_data=&product_data;
//    MainWindow::ui->label_totalPages->setHidden(true);
//    resultSubList=resultList;
    emit pproducttable->layoutChanged();
}

void MainWindow::displayData(QList<products> resultList)
{
//    product_data=resultList;
    MainWindow::pproduct_data=&resultList;
    MainWindow::blockSize=(resultList.size()%50 ? resultList.size()/50+1 : resultList.size()/50);
    MainWindow::pTotalPages->setText(QString::number(blockSize));
    MainWindow::productsSize=resultList.size();
//    setData(*MainWindow::pproduct_data);
//    MainWindow::currentBlock=1;
    qDebug()<<pproduct_data->size()<<"displayData blocksize:"<<MainWindow::blockSize;

    if(resultList.size()>50)
//        qDebug()<<resultSubList.size();
        qDebug()<<currentPage<<"resultsublist:"<<resultList.mid((currentPage-1)*50,50).size();
    int count=0;
    if(blockSize!=0)
    {
        producttable::pproducttable->setModelData1(resultList,count);
        MainWindow::ptv_products->setModel(producttable::pproducttable);
        int row_count=resultList.size();
//        cout<<"row_count="<<row_count<<endl;
        int item_row_count;

        for(int i=0;i<row_count;i++)
        {
            item_row_count=resultList.at(i).getp_productItem().size();
    //        cout<<"item_row_count"<<item_row_count<<endl;
            MainWindow::ptv_products->setSpan(count,0,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,1,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,2,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,3,item_row_count,1);

//            MainWindow::ptv_products->setSpan(count,11,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,12,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,13,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,14,item_row_count,1);
            MainWindow::ptv_products->setSpan(count,15,item_row_count,1);

            count+=item_row_count;
        }
        if(loginDialog::isAdmin==0)
        {
            ui->pB_manage->hide();
            ui->pB_batchImport->hide();
            ui->pB_editAdminPassword->hide();
            ui->pushButton_adjustPrice->hide();
            ui->tV_products->setColumnHidden(6,true);//来源
            ui->tV_products->setColumnHidden(11,true);//操作
            ui->tV_products->setColumnHidden(15,true);//操作
        }
        else
        {
            ui->pB_manage->show();
            ui->pB_batchImport->show();
            ui->pB_editAdminPassword->show();
            ui->tV_products->setColumnHidden(6,false);//来源
            ui->tV_products->setColumnHidden(11,false);//操作
            ui->tV_products->setColumnHidden(15,false);//操作
        }
    }
    producttable::pproducttable->setModelData1(resultList,count);
//    MainWindow::ui->label_totalPages->setHidden(true);
//    resultSubList=resultList;
    emit pproducttable->layoutChanged();
}

//void MainWindow::displayData(QList<products> resultList)
//{
////    product_data=resultList;
//    MainWindow::pproduct_data=&resultList;
//    MainWindow::blockSize=(resultList.size()%50 ? resultList.size()/50+1 : resultList.size()/50);
//    MainWindow::pTotalPages->setText(QString::number(blockSize));
//    MainWindow::productsSize=resultList.size();
////    setData(*MainWindow::pproduct_data);
////    MainWindow::currentBlock=1;
//    qDebug()<<pproduct_data->size()<<"displayData blocksize:"<<MainWindow::blockSize;

//    if(resultList.size()>50)
////        qDebug()<<resultSubList.size();
//        qDebug()<<currentPage<<"resultsublist:"<<resultList.mid((currentPage-1)*50,50).size();
//    int count=0;
//    if(blockSize!=0)
//    {
//        producttable::pproducttable->setModelData1(resultList,count);
//        MainWindow::ptv_products->setModel(producttable::pproducttable);
//        int row_count=resultList.size();
//        cout<<"row_count="<<row_count<<endl;
//        int item_row_count;

//        for(int i=0;i<row_count;i++)
//        {
//            item_row_count=resultList.at(i).getp_productItem().size();
//    //        cout<<"item_row_count"<<item_row_count<<endl;
//            MainWindow::ptv_products->setSpan(count,0,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,1,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,2,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,3,item_row_count,1);

//            MainWindow::ptv_products->setSpan(count,11,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,12,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,13,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,14,item_row_count,1);
//            MainWindow::ptv_products->setSpan(count,15,item_row_count,1);

//            count+=item_row_count;
//        }
//    }
//    producttable::pproducttable->setModelData1(resultList,count);
////    MainWindow::ui->label_totalPages->setHidden(true);
////    resultSubList=resultList;
//    emit pproducttable->layoutChanged();
//}



void MainWindow::on_pB_PageUp_clicked()
{
    //根据ID显示（批量报价、检索）
    if(MainWindow::isShowByID)
    {
//        product_data=*pproduct_data;
        qDebug()<<"pproduct_data"<<MainWindow::pproduct_data->size();

        if(currentPage>1)
        {
            currentPage--;
            ui->lineEdit_currentPage->setText(QString::number(currentPage));
            QList<products> temp;
            temp=*MainWindow::pproduct_data;
            temp=temp.mid((currentPage-1)*50,50);
            int rowsize=producttable::pproducttable->getModelRows();
            producttable::pproducttable->setModelData(temp,rowsize);
            MainWindow::ptv_products->setModel(producttable::pproducttable);
        }
        else
        {
//            QApplication::setQuitOnLastWindowClosed(false);
            QMessageBox::warning(NULL , tr("提示"), tr("已经是最开始页！"), QMessageBox::Yes);
        }
        return;
    }
    //根据块号显示（批量导入）
    else
    {
        int temp=currentBlock;
        temp=temp-1;
        if(temp<0)
        {
//            QApplication::setQuitOnLastWindowClosed(false);
            QMessageBox::warning(NULL , tr("提示"), tr("已经是最开始页！"), QMessageBox::Yes);
        }
        else {
            currentBlock--;
            setData(currentBlock);
            ui->lineEdit_currentPage->setText(QString::number(currentBlock+1));
        }
    }

}

void MainWindow::on_pB_PageOn_clicked()
{
    //根据ID显示（批量报价、检索）
    if(MainWindow::isShowByID)
    {
//        product_data=*pproduct_data;
//        connect(this,SIGNAL(displayData(QList<products>)),this,SLOT(displayData(QList<products>)));
        if(currentPage<totalpages)
        {
            currentPage++;
            ui->lineEdit_currentPage->setText(QString::number(currentPage));
            QList<products> temp;
            temp=*MainWindow::pproduct_data;
            temp=temp.mid((currentPage-1)*50,50);
            int rowsize=producttable::pproducttable->getModelRows();
            producttable::pproducttable->setModelData(temp,rowsize);
            MainWindow::ptv_products->setModel(producttable::pproducttable);
//            displayData(currentPage);
        }
        else
        {
//            QApplication::setQuitOnLastWindowClosed(false);
            QMessageBox::warning(NULL , tr("提示"), tr("已经是最后一页！"), QMessageBox::Yes);
        }
        return;
    }
    //根据块号显示（批量导入）
    else
    {
        int temp=currentBlock;
        temp=temp+1;
        if(temp==blockSize)
        {
//            QApplication::setQuitOnLastWindowClosed(false);
            QMessageBox::warning(NULL , tr("提示"), tr("已经是最后一页！"), QMessageBox::Yes);
        }
        else {
            currentBlock++;
            setData(currentBlock);
            ui->lineEdit_currentPage->setText(QString::number(currentBlock+1));
        }
    }
}

void MainWindow::on_lineEdit_currentPage_editingFinished()
{
    //根据ID显示（批量报价、检索）
    if(MainWindow::isShowByID)
    {
        int temp=ui->lineEdit_currentPage->text().toInt();
        if(temp>totalpages)
        {
//            QApplication::setQuitOnLastWindowClosed(false);
            QMessageBox::warning(NULL , tr("提示"), tr("超过最大页！"), QMessageBox::Yes);
        }
        else
        {
            currentPage=temp;
            ui->lineEdit_currentPage->setText(QString::number(temp));
            QList<products> temp;
            temp=*MainWindow::pproduct_data;
            temp=temp.mid((currentPage-1)*50,50);
            int rowsize=producttable::pproducttable->getModelRows();
            producttable::pproducttable->setModelData(temp,rowsize);
            MainWindow::ptv_products->setModel(producttable::pproducttable);
        }
    }
    //根据块号显示（批量导入）
    else
    {
        int temp=ui->lineEdit_currentPage->text().toInt();
        if(temp>blockSize)
        {
//            QApplication::setQuitOnLastWindowClosed(false);
            QMessageBox::warning(NULL , tr("提示"), tr("超过最大页！"), QMessageBox::Yes);
        }
        else
        {
            currentBlock=temp-1;
            setData(currentBlock);
            ui->lineEdit_currentPage->setText(QString::number(temp));
        }
    }
}

void MainWindow::on_cB_showDeleted_stateChanged(int state)
{
//    isShowDeleted=false;
    if (state == Qt::Checked) // "选中"
    {
        isShowDeleted=true;
        setHide();
    }
    else if(state == Qt::Unchecked)
    {
        isShowDeleted=false;
        setHide();
    }
}
void MainWindow::on_pB_batchImport_clicked()
{ 
//    QString curPath=QDir::currentPath();//获取当前路径
//    QString str=QFileDialog::getOpenFileName(this,"打开文件",curPath);
    QString location = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    QString str=QFileDialog::getOpenFileName(this,"打开Excel工作簿",location,
                                         tr("Excel工作簿(*.xlsx);;Excel97-2003工作簿(*.xls)"));
    if(str.isEmpty())
    {
        //显示全部
        setData(0);
    }
    else
    {
        //导入文件

        int res=qExcelTool->open(str);
//        qDebug()<<qExcelTool.open(str)<<"res";
        //excel_ok
        if(EXCEL_OK==res)
        {
//            QMessageBox::warning(NULL , tr("提示"), tr("格式无误"), QMessageBox::Yes);
//            connect(qExcelTool, SIGNAL(signals_finished(int,int)), this, SLOT(Signals(int,int)));
        }
        //excel_error
        else if(EXCEL_ERROR==res)
        {
//            connect(qExcelTool, SIGNAL(signals_finished(int,int)), this, SLOT(dealSignals(int,int)));
//            QApplication::setQuitOnLastWindowClosed(false);
            QMessageBox::warning(NULL , tr("提示"), tr("格式有误，请选择路径保存文件"), QMessageBox::Yes);
            QString location = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

            QString strSaveName=QFileDialog::getSaveFileName(this,"打开Excel工作簿",location,
                                                 tr("Excel工作簿(*.xlsx);;Excel97-2003工作簿(*.xls)"));
//            QString strSaveName = QFileDialog::getSaveFileName(this,tr("保存的文件"),tr("."),tr("*.xlsx"));
            strSaveName.replace("/","\\");
            qDebug()<<"strSaveErrorName"<<strSaveName;
            //判断文件名
            if(strSaveName.isEmpty())
            {
//               QApplication::setQuitOnLastWindowClosed(false);
               QMessageBox::warning(NULL , tr("提示"), tr("文件名称不能为空，已保存到桌面！"), QMessageBox::Yes);
               QString username=QDir::home().dirName();
               strSaveName="C:\\Users\\"+username+"\\Desktop\\导入数据错误处理结果.xlsx";
            }
            int ret = qExcelTool->saveErrorExcel(strSaveName);
            qDebug()<<"save file"<<ret;
            //写入错误文件
            if(EXCEL_OK==ret)
            {
//                QApplication::setQuitOnLastWindowClosed(false);
                QMessageBox::warning(NULL , tr("提示"), tr("文件已保存！"), QMessageBox::Yes);

            }
            else if(EXCEL_ERROR==ret)
            {
//                QApplication::setQuitOnLastWindowClosed(false);
                QMessageBox::warning(NULL , tr("提示"), tr("文件保存失败"), QMessageBox::Yes);
            }
        }
        pQwait->show();
    }
    qDebug()<<str<<"open str";
}

void MainWindow::on_pB_batchQuotation_clicked()
{
//    connect(this,SIGNAL(ids2Products(QList<int>)),this,SLOT(ids2Products(QList<int>)));
//    connect(this,SIGNAL(displayData(QList<products>)),this,SLOT(displayData(QList<products>)));
//    QString curPath=QDir::currentPath();//获取当前路径
//    QString str=QFileDialog::getOpenFileName(this,"打开文件",curPath);
    QString location = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    QString str=QFileDialog::getOpenFileName(this,"打开Excel工作簿",location,
                                         tr("Excel工作簿(*.xlsx);;Excel97-2003工作簿(*.xls)"));
    if(str.isEmpty())
    {
        setData(0);
    }
    else
    {
        MainWindow::currentPage=1;
        QJsonObject obj;
        obj.insert("flag",QString::fromStdString(std::to_string(loginDialog::isAdmin)));
        std::unique_lock<std::mutex> lk_http(qHttpTool->mSendingMutex);
        qHttpTool->setIP(csIP);
        qHttpTool->setPort(csPort);
        qHttpTool->setData(obj);
        qHttpTool->setRouting("/product/queryBlock");
        qHttpTool->setFilePath(str);
        qHttpTool->bindCallback(interfaceJump);
        qHttpTool->POST(0);
        lk_http.unlock();
    }
    qDebug()<<str;
}

void MainWindow::on_pB_exportProduct_clicked()
{
//    QString strSaveName = QFileDialog::getSaveFileName(this,tr("保存的文件"),tr("."),tr("*.xlsx"));
    QString location = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString strSaveName=QFileDialog::getSaveFileName(this,"打开Excel工作簿",location,
                                         tr("Excel工作簿(*.xlsx);;Excel97-2003工作簿(*.xls)"));
    strSaveName.replace("/","\\");
    qDebug()<<"strSaveName"<<strSaveName;
    //判断文件名
    if( strSaveName.isEmpty() )
    {
//       QApplication::setQuitOnLastWindowClosed(false);
       QMessageBox::warning(NULL , tr("提示"), tr("文件名称不能为空，请重新输入"), QMessageBox::Yes);
       return;
    }
    else
    {
        int res = qExcelTool->exportProducts(MainWindow::product_data,strSaveName,isShowDeleted);
//        QApplication::setQuitOnLastWindowClosed(false);
        QMessageBox::warning(NULL , tr("提示"), tr("文件已保存！"), QMessageBox::Yes);
    }
}

void MainWindow::on_pB_LogOut_clicked()
{
//    delete pQwait;
    this->close();
    ui->tV_products->clearSpans();
    ui->tV_products->close();
    pproducttable=NULL;
    pmainWindow=NULL;
    isShowByID=false;
    ptv_products=NULL;
    blockSize=0;
    productsSize=0;
    currentBlock=0;
    totalpages=0;
    currentPage=0;
    resultSize=0;
    idSize=0;
    pPageOn=NULL;
    pPageUp=NULL;
    pPageTo=NULL;
    pTotalPages=NULL;
    pproduct_data=NULL;
    result_data=NULL;
    pproduct_ids=NULL;
    ui->label_totalPages->setText(QString::number(blockSize));
    product_data.clear();
    ui->pB_PageOn->setEnabled(false);
    ui->pB_PageUp->setEnabled(false);
    ui->lineEdit_currentPage->setText("0");
    productsmodel->setModelData();

    ui->comboBox_brand->clear();
//    pmainWindow.deleteLater();
    productsmodel->deleteLater();
    qHttpTool->deleteLater();
    pQwait->deleteLater();
    emit logout();
}
void MainWindow::show_main()
{
    ui->tV_products->show();
    init();
    if(loginDialog::isAdmin==0)
    {
        ui->pB_manage->hide();
        ui->pB_batchImport->hide();
        ui->pB_editAdminPassword->hide();
        ui->cB_showDeleted->hide();
        ui->pushButton_adjustPrice->hide();
        ui->tV_products->setColumnHidden(6,true);//来源
        ui->tV_products->setColumnHidden(11,true);//操作
        ui->tV_products->setColumnHidden(15,true);//操作
    }
    else
    {
        ui->pB_manage->show();
        ui->pB_batchImport->show();
        ui->pB_editAdminPassword->show();
        ui->cB_showDeleted->show();
        ui->tV_products->setColumnHidden(6,false);//来源
        ui->tV_products->setColumnHidden(11,false);//操作
        ui->tV_products->setColumnHidden(15,false);//操作
    }
    this->show();
}

void MainWindow::on_pB_DeleteCache_clicked()
{
    QString cachePath="./Cache";
    QString imgPath="./img";
    QString configPath="./config.txt";
    if(!QDir().exists(cachePath)||!QDir().exists(configPath))
        qDebug()<<"cache path is not exits!";
    QFileInfo fi(configPath);
    QFile::remove(configPath);
    QDir qImgDir(imgPath);
    qImgDir.removeRecursively();
    QDir qDir(cachePath);
    qDir.removeRecursively();
//    qExcelTool.getProductsAndBlocksSize(blockSize,productsSize);
//    blockSize=0;
//    productsSize=0;
//    pmainWindow=NULL;
    pproducttable=NULL;
    pmainWindow=NULL;
    isShowByID=false;
    ptv_products=NULL;
    blockSize=0;
    productsSize=0;
    currentBlock=0;
    totalpages=0;
    currentPage=0;
    resultSize=0;
    idSize=0;
    pPageOn=NULL;
    pPageUp=NULL;
    pPageTo=NULL;
    pTotalPages=NULL;
    pproduct_data=NULL;
    result_data=NULL;
    pproduct_ids=NULL;
    ui->label_totalPages->setText(QString::number(blockSize));
    product_data.clear();
    ui->pB_PageOn->setEnabled(false);
    ui->pB_PageUp->setEnabled(false);
    ui->lineEdit_currentPage->setText("0");
    productsmodel->setModelData();
    MainWindow::pmainWindow->setHide();

    ui->comboBox_brand->clear();
//    pmainWindow.deleteLater();
    productsmodel->deleteLater();
    qHttpTool->deleteLater();
    pQwait->deleteLater();
    this->close();
    emit logout();
}

void MainWindow::jump(QList<QString> brand_list)
{
    ui->comboBox_brand->clear();
    ui->comboBox_brand->addItem("全部品牌");
    ui->comboBox_brand->addItems(brand_list);
}

void MainWindow::on_comboBox_brand_activated(const QString &arg1)
{

}

void MainWindow::on_pushButton_showAll_clicked()
{
    qExcelTool->getProducts(0,product_data);
    qExcelTool->getProductsAndBlocksSize(blockSize,productsSize);
    isShowByID=false;
    setData(0);
    //获取全部品牌
    if(productsSize>0)
    {
        QJsonObject js;
        std::unique_lock<std::mutex> lk(qHttpTool->mSendingMutex);
        qHttpTool->setIP(csIP);
        qHttpTool->setPort(csPort);
        qHttpTool->setData(js);
        qHttpTool->setRouting("/product/findAllBrand");
        qHttpTool->bindCallback(interfaceJump);
        qHttpTool->GET(1);
        lk.unlock();
    }
}

void MainWindow::on_pushButton_adjustPrice_clicked()
{
    adjustPriceDialog *adjustPriceW=new adjustPriceDialog(this);
    adjustPriceW->show();
}

void MainWindow::on_cB_showDeleted_clicked()
{

}
