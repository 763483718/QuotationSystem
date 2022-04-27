#include "adjustpricedialog.h"
#include "ui_adjustpricedialog.h"
#include "mainwindow.h"
#include "producttable.h"
#include <iostream>
using namespace std;

adjustPriceDialog::adjustPriceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::adjustPriceDialog)
{
    ui->setupUi(this);
    setWindowTitle("产品调价");
    ui->spinBox_adjustValue->setSuffix("%");
    ui->spinBox_adjustValue->setMaximum(10000);
    MainWindow::qExcelTool->setHttpTool(MainWindow::qHttpTool);
    connect(MainWindow::qExcelTool, SIGNAL(signals_finished(int,int)), this, SLOT(dealSignals(int,int)));
}

adjustPriceDialog::~adjustPriceDialog()
{
    delete ui;
}

//上调
void adjustPriceDialog::on_radioButton_up_clicked(bool checked)
{
    if(checked==true)
    {
        priceUp=true;
    }
    else
    {
        priceUp=false;
    }
}

//下调
void adjustPriceDialog::on_radioButton_down_clicked(bool checked)
{
    if(checked==true)
    {
        priceDown=true;
    }
    else
    {
        priceDown=false;
    }
}

void adjustPriceDialog::on_buttonBox_accepted()
{
    adjustValue=ui->spinBox_adjustValue->value();
    if(adjustValue!=0)
    {
        if(priceUp==true&&priceDown==false)//上调
            adjustValue=adjustValue;
        else if(priceDown==true&&priceUp==false)//下调 不能超过100%
        {
            if(adjustValue>100)
            {
                QMessageBox::warning(NULL , tr("提示"), tr("下调不能超过100%，请重新选择！"), QMessageBox::Yes);
                return;
            }
            else
                adjustValue=0-adjustValue;
        }
        else if(priceUp==false&&priceDown==false)
        {
            QMessageBox::warning(NULL , tr("提示"), tr("未选择调价方式，请重新选择！"), QMessageBox::Yes);
            return;
        }
        emit producttable::pproducttable->adjustP(adjustValue);
    }
    else
    {
        QMessageBox::warning(NULL , tr("提示"), tr("未填写调价幅度，请重新选择！"), QMessageBox::Yes);
    }
}

void adjustPriceDialog::dealSignals(int OperationNumber,int block)
{

    MainWindow::pQwait->close();
}
