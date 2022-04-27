#ifndef ADDPRODUCTDIALOG_H
#define ADDPRODUCTDIALOG_H

#include <QDialog>
#include "products.h"

namespace Ui {
class addProductDialog;
}

class addProductDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addProductDialog(QWidget *parent = nullptr);
    ~addProductDialog();

signals:
    void addproduct();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_pB_imgPath_clicked();
    QPixmap loadImageFunction(const QString &imagePath,const QSize &targetSize);
    void showMsg(QString title,QString str);

private:
    Ui::addProductDialog *ui;
    QList<products> productsList;
    QList<productItem> productItemList;
    QString curPath;
    QString newPath;
    products newdata;
};

#endif // ADDPRODUCTDIALOG_H
