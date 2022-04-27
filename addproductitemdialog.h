#ifndef ADDPRODUCTITEMDIALOG_H
#define ADDPRODUCTITEMDIALOG_H

#include <QDialog>
#include "products.h"

namespace Ui {
class addProductItemDialog;
}

class addProductItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addProductItemDialog(QWidget *parent = nullptr);
    ~addProductItemDialog();
    void display();

signals:
    //自定义信号
    void addproductitem();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();
    void showMsg(QString title,QString str);

private:
    Ui::addProductItemDialog *ui;
    QList<products> productsList;
    QList<productItem> productItemList;
};

#endif // ADDPRODUCTITEMDIALOG_H
