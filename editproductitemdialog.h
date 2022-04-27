#ifndef EDITPRODUCTITEMDIALOG_H
#define EDITPRODUCTITEMDIALOG_H

#include <QDialog>
#include "products.h"

namespace Ui {
class editProductItemDialog;
}

class editProductItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit editProductItemDialog(QWidget *parent = nullptr);
    ~editProductItemDialog();

signals:
    //自定义信号
    void editproductitem();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();
    void showMsg(QString title,QString str);signals:
    //自定义信号

private:
    Ui::editProductItemDialog *ui;
    QList<products> productsList;
    QList<productItem> productItemList;
    int productId;
    int productItemId;
};

#endif // EDITPRODUCTITEMDIALOG_H
