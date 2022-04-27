#ifndef EDITPRODUCTDIALOG_H
#define EDITPRODUCTDIALOG_H

#include <QDialog>
#include "products.h"
#include "qexceltool.h"
#include "qhttptool.h"

namespace Ui {
class editProductDialog;
}

class editProductDialog : public QDialog
{
    Q_OBJECT

public:
    explicit editProductDialog(QWidget *parent = nullptr);
    ~editProductDialog();

public slots:
    void dealSignals(int,int);//处理信号

signals:
    void editproduct();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_pB_imgPath_clicked();
    QPixmap loadImageFunction(const QString &imagePath,const QSize &targetSize);
    void showMsg(QString title,QString str);

private:
    Ui::editProductDialog *ui;
    QList<products> productsList;
    QList<productItem> productItemList;
    QString curPath;
    int productId;
    int productItemId;
    QString oldPath;
    QString newPath;
};

#endif // EDITPRODUCTDIALOG_H
