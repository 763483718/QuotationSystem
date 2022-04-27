#include "editproductitemdialog.h"
#include "ui_editproductitemdialog.h"
#include <QMessageBox>
#include "buttonitemdelegate.h"
#include "mainwindow.h"
#include <iostream>

editProductItemDialog::editProductItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::editProductItemDialog)
{
    ui->setupUi(this);
    setWindowTitle("修改产品item");
//    setMinimumSize(300,300);
    QWidget::setTabOrder(ui->lineEdit_type, ui->lineEdit_price);
    QWidget::setTabOrder(ui->lineEdit_price,ui->lineEdit_source);
    QWidget::setTabOrder(ui->lineEdit_source,ui->lineEdit_metetial);
    QWidget::setTabOrder(ui->lineEdit_metetial,ui->lineEdit_color);
    QWidget::setTabOrder(ui->lineEdit_color,ui->lineEdit_MOQ);
    QWidget::setTabOrder(ui->lineEdit_MOQ,ui->lineEdit_MPQ);
    productsList=producttable::pproducttable->getModelData();
//    qDebug()<<buttonItemDelegate::editIndexRow<<" "<<buttonItemDelegate::editItemIndexRow;
    productItemList=productsList[buttonItemDelegate::editIndexRow].getp_productItemNotConst();
    productId=buttonItemDelegate::editIndexRow;
    productItemId=buttonItemDelegate::editItemIndexRow;
    ui->lineEdit_type->setText(productItemList[productItemId].getp_type());
    ui->lineEdit_price->setText(QString::number(productItemList[productItemId].getp_price()));
    ui->lineEdit_source->setText(productItemList[productItemId].getp_source());
    ui->lineEdit_metetial->setText(productItemList[productItemId].getp_material());
    ui->lineEdit_color->setText(productItemList[productItemId].getp_color());
    ui->lineEdit_MOQ->setText(productItemList[productItemId].getp_MOQ());
    ui->lineEdit_MPQ->setText(productItemList[productItemId].getp_MPQ());
//    qDebug()<<productId<<" "<<productItemId;
}

editProductItemDialog::~editProductItemDialog()
{
    delete ui;
}
void editProductItemDialog::on_buttonBox_accepted()
{
    QString type=ui->lineEdit_type->text().trimmed();
    QString price=ui->lineEdit_price->text().trimmed();
    QString source=ui->lineEdit_source->text().trimmed();
    QString matetial=ui->lineEdit_metetial->text().trimmed();
    QString color=ui->lineEdit_color->text().trimmed();
    QString moq=ui->lineEdit_MOQ->text().trimmed();
    QString mpq=ui->lineEdit_MPQ->text().trimmed();

    productItemList[productItemId].setp_type(type);
    productItemList[productItemId].setp_price(price.toFloat());
    productItemList[productItemId].setp_source(source);
    productItemList[productItemId].setp_material(matetial);
    productItemList[productItemId].setp_color(color);
    productItemList[productItemId].setp_MOQ(moq);
    productItemList[productItemId].setp_MPQ(mpq);

    productsList[productId].setp_productItem(productItemList);
    products p=productsList[productId];
    //update data
    int row=producttable::pproducttable->getModelRows();
    emit producttable::pproducttable->editP(p,productsList,row);
}

void editProductItemDialog::on_buttonBox_rejected()
{
//    emit editproductitem();
    this->close();
}
void editProductItemDialog::showMsg(QString title,QString str)
{
    QMessageBox msg;
    msg.setWindowTitle(title);
    msg.setText(str);
    msg.exec();
}

