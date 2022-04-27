#include "editproductdialog.h"
#include "ui_editproductdialog.h"
#include <QFileDialog>
#include <QDebug>
#include <QImageReader>
#include "products.h"
#include <QMessageBox>
#include "mainwindow.h"
#include "QStandardPaths"

editProductDialog::editProductDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::editProductDialog)
{
    ui->setupUi(this);
    setWindowTitle("修改产品");
    QWidget::setTabOrder(ui->pB_imgPath,ui->lineEdit_name);
    QWidget::setTabOrder(ui->lineEdit_name,ui->lineEdit_brand);
    QWidget::setTabOrder(ui->lineEdit_brand,ui->lineEdit_hole);
    QWidget::setTabOrder(ui->lineEdit_hole,ui->lineEdit_fitting);
    QWidget::setTabOrder(ui->lineEdit_fitting,ui->lineEdit_remark);

    productsList=producttable::pproducttable->getModelData();
//    qDebug()<<buttonDelegate::editIndexRow<<" "<<buttonDelegate::editItemIndexRow;
    productItemList=productsList[buttonDelegate::editIndexRow].getp_productItemNotConst();
    productId=buttonDelegate::editIndexRow;
    productItemId=buttonDelegate::editItemIndexRow;
    ui->pB_imgPath->setText(productsList[productId].getp_imagePath());
    ui->lineEdit_name->setText(productsList[productId].getp_name());
    ui->lineEdit_brand->setText(productsList[productId].getp_brand());
    ui->lineEdit_hole->setText((productsList[productId].getp_hole()));
    ui->lineEdit_remark->setText(productsList[productId].getp_remark());
    ui->lineEdit_fitting->setText(productsList[productId].getp_fitting());
    qDebug()<<productsList[productId].getp_imagePath();
    QPixmap pix(QPixmap(productsList[productId].getp_imagePath()));
    pix = pix.scaled(300, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);  // 按比例缩放
    ui->label_showimg->setPixmap(pix);
//    qHttpTool=new QHttpTool();
    MainWindow::qExcelTool->setHttpTool(MainWindow::qHttpTool);
    connect(MainWindow::qExcelTool, SIGNAL(signals_finished(int,int)), this, SLOT(dealSignals(int,int)));
}

editProductDialog::~editProductDialog()
{
    delete ui;
}
void editProductDialog::on_buttonBox_accepted()
{
    QString name=ui->lineEdit_name->text().trimmed();
    QString brand=ui->lineEdit_brand->text().trimmed();
    QString hole=ui->lineEdit_hole->text().trimmed();
    QString remark=ui->lineEdit_remark->text().trimmed();
    QString fitting=ui->lineEdit_fitting->text().trimmed();

    productsList[productId].setp_name(name);
    productsList[productId].setp_brand(brand);
    productsList[productId].setp_hole(hole);
    productsList[productId].setp_remark(remark);
    productsList[productId].setp_fitting(fitting);
    QProduct p=productsList[productId];
    //update data
    int row=producttable::pproducttable->getModelRows();
//    if(!curPath.isNull())

    //单独发img str productid
    emit producttable::pproducttable->editP(p,productsList,row);
}

void editProductDialog::on_buttonBox_rejected()
{
    this->close();
}

void editProductDialog::on_pB_imgPath_clicked()
{
    oldPath=ui->pB_imgPath->text();
//    QString str=QDir::currentPath();//获取当前路径
//    curPath=QFileDialog::getOpenFileName(this,"打开文件",str);
    QString location = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    curPath=QFileDialog::getOpenFileName(this,"打开文件",location,
                                         tr("images(*.png *jpeg *bmp *jpg)"));
    if(curPath.isNull())
    {
        ui->pB_imgPath->setText(oldPath);
        ui->label_showimg->setPixmap(loadImageFunction(oldPath,ui->label_showimg->size()));
//        QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
//        newPath=".\\img\\"+timestamp+".jpg";
    }
    else
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

        //删掉原来的图片
//        if(!curPath.isNull())
//        {

//        }
        QFileInfo fi(curPath);
        if(fi.exists()){
            QFile::copy(curPath,newPath);
        }
        QString imgPath=newPath;//ui->pB_imgPath->text().trimmed();
        QImage image;
        if(!newPath.isEmpty())
            image.load(newPath);
        productsList[productId].setp_imagePath(newPath);
        productsList[productId].setp_image(image);
        qDebug()<<"oldpath"<<oldPath;
        qDebug()<<"newpath"<<newPath;

        MainWindow::qExcelTool->replaceImage(productsList[productId].getp_id(), oldPath,newPath);
//        cout << __func__ << "\tproductId:\t" << productsList[productId].getp_id() + 1 << endl;
        imgPathDelegate::pImg_list->replace(productId,newPath);
    }
}

void editProductDialog::dealSignals(int OperationNumber,int block)
{
    //替换图片错误
    if(OperationNumber== 0x304)
    {
        ui->buttonBox->setEnabled(false);
    }
    //替换图片成功
    else if(OperationNumber== 0x305)
    {
        ui->buttonBox->setEnabled(true);
    }
    MainWindow::pQwait->close();
}
QPixmap editProductDialog::loadImageFunction(const QString &imagePath,const QSize &targetSize)
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

void editProductDialog::showMsg(QString title,QString str)
{
    QMessageBox msg;
    msg.setWindowTitle(title);
    msg.setText(str);
    msg.exec();
}
