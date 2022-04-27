#include "addproductdialog.h"
#include "ui_addproductdialog.h"
#include <QFileDialog>
#include <QDebug>
#include <QImageReader>
#include "products.h"
#include <QMessageBox>
#include "mainwindow.h"
#include "QStandardPaths"

addProductDialog::addProductDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addProductDialog)
{
    ui->setupUi(this);
    setWindowTitle("添加产品");
    QWidget::setTabOrder(ui->pB_imgPath,ui->lineEdit_name);
    QWidget::setTabOrder(ui->lineEdit_name,ui->lineEdit_brand);
    QWidget::setTabOrder(ui->lineEdit_brand,ui->lineEdit_type);
    QWidget::setTabOrder(ui->lineEdit_type,ui->lineEdit_price);
    QWidget::setTabOrder(ui->lineEdit_price,ui->lineEdit_source);
    QWidget::setTabOrder(ui->lineEdit_source,ui->lineEdit_metetial);
    QWidget::setTabOrder(ui->lineEdit_metetial,ui->lineEdit_hole);
    QWidget::setTabOrder(ui->lineEdit_hole,ui->lineEdit_color);
    QWidget::setTabOrder(ui->lineEdit_color,ui->lineEdit_fitting);
    QWidget::setTabOrder(ui->lineEdit_fitting,ui->lineEdit_MOQ);
    QWidget::setTabOrder(ui->lineEdit_MOQ,ui->lineEdit_MPQ);
    QWidget::setTabOrder(ui->lineEdit_MPQ,ui->textEdit_remark);
    productsList=producttable::pproducttable->getModelData();
    MainWindow::productsSize=productsList.size();
}

addProductDialog::~addProductDialog()
{
    delete ui;
}

void addProductDialog::on_buttonBox_accepted()
{
    QString imgPath=curPath;
    QImage image;
    if(!curPath.isEmpty())
        image.load(curPath);
    QString name=ui->lineEdit_name->text().trimmed();
    QString brand=ui->lineEdit_brand->text().trimmed();
    QString type=ui->lineEdit_type->text().trimmed();
    QString price=ui->lineEdit_price->text().trimmed();
    QString source=ui->lineEdit_source->text().trimmed();
    QString metetial=ui->lineEdit_metetial->text().trimmed();
    QString hole=ui->lineEdit_hole->text().trimmed();
    QString color=ui->lineEdit_color->text().trimmed();
    QString remark=ui->textEdit_remark->toPlainText().trimmed();
    QString fitting=ui->lineEdit_fitting->text().trimmed();
    QString moq=ui->lineEdit_MOQ->text().trimmed();
    QString mpq=ui->lineEdit_MPQ->text().trimmed();
    if(ui->lineEdit_type->text().trimmed().isEmpty() ||
            ui->lineEdit_price->text().trimmed().isEmpty() ||
            ui->lineEdit_source->text().trimmed().isEmpty())
    {
//        QApplication::setQuitOnLastWindowClosed(true);
        this->show();
        showMsg(tr("提示"),tr("带星号的内容不能为空"));
    }
    else
    {
        QFile::copy(curPath,newPath);
        //传数据
        productsList=producttable::pproducttable->getModelData();
        int count = 0;
        for(int i=0;i<productsList.size();i++)
        {
            int size = productsList.at(i).getItemsSize();
            count += size;
        }
        //push back到最后
        cout<<"push back"<<endl;

        productItemList.push_back({type,metetial,color,source,price.toFloat(),moq,mpq,false});

        int newId;
        if(MainWindow::isShowByID) //showbyid
            newId=productsList.size()+MainWindow::currentPage*50;
        else //showbyblock
            newId=productsList.size()+MainWindow::currentBlock*50;
//        qDebug()<<str;
        newdata={newId+1,newPath,image,brand,name,productItemList,hole,fitting,remark,false};
        productsList.push_back({newdata});
        newId=productsList.size();
        count++;

//        cout<<"result:"<<productsList.size()<<endl;
//        MainWindow::pmainWindow->setData();
//        producttable::pproducttable->setModelData(productsList,count);
//        emit producttable::pproducttable->layoutChanged();
//        qDebug()<<*imgPathDelegate::pImg_list;

        imgPathDelegate::pImg_list->push_back(newPath);
//        qDebug()<<*imgPathDelegate::pImg_list;
        emit producttable::pproducttable->AddNewP(productsList,count,productsList[newId-1]);
    }
}

void addProductDialog::on_buttonBox_rejected()
{
    emit addproduct();
    this->close();
}

void addProductDialog::on_pB_imgPath_clicked()
{
//    QString str=QDir::currentPath();//获取当前路径
    QString location = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    curPath=QFileDialog::getOpenFileName(this,"打开文件",location,
                                         tr("images(*.png *jpeg *bmp *jpg)"));
    if(!curPath.isEmpty())
    {
        QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
        if(curPath.contains(".jpg"))
            newPath=".\\img\\"+timestamp+".jpg";
        else if(curPath.contains(".JPG"))
            newPath=".\\img\\"+timestamp+".JPG";
        else if(curPath.contains(".png"))
            newPath=".\\img\\"+timestamp+".png";
        else if(curPath.contains(".PNG"))
            newPath=".\\img\\"+timestamp+".PNG";
        else if(curPath.contains(".bmp"))
            newPath=".\\img\\"+timestamp+".bmp";
        else if(curPath.contains(".BMP"))
            newPath=".\\img\\"+timestamp+".BMP";
        //QFile copy到相对路径，命名为img+ts
        ui->pB_imgPath->setText(newPath);
        ui->label_showimg->setPixmap(loadImageFunction(curPath,ui->label_showimg->size()));
    }
}

QPixmap addProductDialog::loadImageFunction(const QString &imagePath,const QSize &targetSize)
{
    QImageReader imageReader;
    imageReader.setFileName(imagePath);
    imageReader.setAutoTransform(true);
    //原始大小
    auto imageSize=imageReader.size();
    //尺寸变换
    auto targetScaledSize=imageSize.scaled(targetSize,Qt::KeepAspectRatio);
    imageReader.setScaledSize(targetScaledSize);
    QPixmap pixmap=QPixmap::fromImageReader(&imageReader);
    return pixmap;
}

void addProductDialog::showMsg(QString title,QString str)
{
    QMessageBox msg;
    msg.setWindowTitle(title);
    msg.setText(str);
    msg.exec();
}
