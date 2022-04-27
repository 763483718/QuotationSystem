#include "addproductitemdialog.h"
#include "ui_addproductitemdialog.h"
#include <QMessageBox>
#include "buttonitemdelegate.h"
#include "mainwindow.h"
#include <iostream>
using namespace std;

addProductItemDialog::addProductItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addProductItemDialog)
{
    ui->setupUi(this);
    setWindowTitle("添加产品item");
//    setMinimumSize(300,300);
    QWidget::setTabOrder(ui->lineEdit_type, ui->lineEdit_price);
    QWidget::setTabOrder(ui->lineEdit_price,ui->lineEdit_source);
    QWidget::setTabOrder(ui->lineEdit_source,ui->lineEdit_metetial);
    QWidget::setTabOrder(ui->lineEdit_metetial,ui->lineEdit_color);
    QWidget::setTabOrder(ui->lineEdit_color,ui->lineEdit_MOQ);
    QWidget::setTabOrder(ui->lineEdit_MOQ,ui->lineEdit_MPQ);
}

addProductItemDialog::~addProductItemDialog()
{
    delete ui;
}

void addProductItemDialog::on_buttonBox_accepted()
{
    QString type=ui->lineEdit_type->text().trimmed();
    QString price=ui->lineEdit_price->text().trimmed();
    QString source=ui->lineEdit_source->text().trimmed();
    QString metetial=ui->lineEdit_metetial->text().trimmed();
    QString color=ui->lineEdit_color->text().trimmed();
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
        //传数据
        productsList=producttable::pproducttable->getModelData();
//        int row=buttonItemDelegate::newItemIndex->data();
        int row=buttonItemDelegate::newItemIndexRow;
        cout<<"rororow="<<row<<endl;
        int productId = 0;
        int productItemId = 0;
        int count = 0;
        for(int i=0;i<productsList.size();i++){
            int size = productsList.at(i).getItemsSize();
            if(count + size >= row+1){
                productId = i;
                productItemId = row - count;
                break;
            }
            count += size;
        }
        //push back到相应的id的位置
        productItemList=productsList[productId].getp_productItemNotConst();
//        cout<<"SOURCE\tsize:\t"<<productsList[productId].getp_productItemTest().size()<<endl;
        if(productsList[productId].getp_isDelete()==true)
            productItemList.push_back({type,metetial,color,source,price.toFloat(),moq,mpq,true});
        else
            productItemList.push_back({type,metetial,color,source,price.toFloat(),moq,mpq,false});
        productsList[productId].setp_productItem(productItemList);
        count++;
        products p=productsList[productId];
        emit producttable::pproducttable->AddNewItem(p,productsList,count);
//        cout<<"result\tsize:\t"<<productsList[productId].getp_productItemTest().size()<<endl;
    }
}

void addProductItemDialog::on_buttonBox_rejected()
{
    emit addproductitem();
    this->close();
}
void addProductItemDialog::showMsg(QString title,QString str)
{
    QMessageBox msg;
    msg.setWindowTitle(title);
    msg.setText(str);
    msg.exec();
}

