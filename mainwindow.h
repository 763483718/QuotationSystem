#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include "producttable.h"
#include "buttondelegate.h"
#include "buttonitemdelegate.h"
#include "imgpathdelegate.h"
#include "qhttptool.h"
#include <QMessageBox>
#include <QTableView>
#include "qexceltool.h"
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include "waitingdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static bool isShowByID;
    static bool image_load_error;
    bool importError;
    static bool isShowDeleted;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setHide();
    void setData(int blockDisplay);
//    void setData(QList<products> productsList);
    QList<products> getData();
    void addRow();
    void delRow();
    void init();
    static void interfaceJump(QJsonObject info,int requestCode, QJsonValue data)
    {
        if(info.value("statusCode").toInt() == 200){//right
            switch (requestCode)
            {
            case 0:
                cout<<"right"<<endl;
                if(data.toArray().size()>=0)
                {
                    QJsonArray productlist=data.toArray();
                    const int row_count=productlist.size();
                    QList<int> product_ids;
                    int lastnumber;
                    if(row_count>0)
                    {
                        if(!productlist.at(0)["number"].isNull())
                        {
                            lastnumber=productlist.at(0)["number"].toInt();
                            product_ids.push_back(lastnumber);
                        }
                        for(int row=1;row<row_count;row++)
                        {
                            if(!productlist.at(row)["number"].isNull())
                            {
                                int number=productlist.at(row)["number"].toInt();
                                if(lastnumber!=number)
                                {
                                    product_ids.push_back(number);
                                    lastnumber=number;
                                }
                            }
                            else
                                qDebug()<<"Error: number is NULL!";

                        }

                    }
                    pmainWindow->ids2Products(product_ids);

//                    QList<products> new_product;
//                    QList<productItem> new_item;
//                    cout<<"row_count:"<<row_count<<endl;
//                    int index=0;
//                    for(int row=0;row<row_count;row++)
//                    {
//                        int id=productlist.at(row)["id"].toInt();
//                        qDebug()<<"id"<<id;
//                        int number=productlist.at(row)["number"].toInt();
//                        int lastNumber=0;
//                        if(row>0)
//                            lastNumber=productlist.at(row-1)["number"].toInt();
//                        QString imgPath=productlist.at(row)["picturePath"].toString().toUtf8();
//                        QImage image;
//                        if(!imgPath.isEmpty())
//                        {
//                            imgPath.replace("/","\\");
//                            image.load(imgPath);
//                        }

//                        QString type=productlist.at(row)["model"].toString().toUtf8();
//                        QString brand=productlist.at(row)["brand"].toString().toUtf8();
//                        QString name=productlist.at(row)["chineseName"].toString().toUtf8();
//                        float price=productlist.at(row)["price"].toDouble();
//                        QString source=productlist.at(row)["source"].toString().toUtf8();
//                        QString material=productlist.at(row)["material"].toString().toUtf8();
//                        int hole=productlist.at(row)["holeNum"].toInt();
//                        QString color=productlist.at(row)["color"].toString().toUtf8();
//                        int moq=productlist.at(row)["moq"].toInt();
//                        int mpq=productlist.at(row)["mpq"].toInt();
//                        QString remark=productlist.at(row)["notes"].toString().toUtf8();
//                        QString fitting=productlist.at(row)["matches"].toString().toUtf8();
//                        bool itemisdelete=productlist.at(row)["isDelete"].toBool();

//                        if(row==0)
//                        {
//                            new_item.push_back({type,material,color,source,price,moq,mpq,itemisdelete});
//                            new_product.push_back({number,imgPath,image,brand,name,new_item,hole,fitting,remark,itemisdelete});
//                        }
//                        else if(number==lastNumber)
//                        {
//                            new_item.push_back({type,material,color,source,price,moq,mpq,itemisdelete});
//                            new_product[index].setp_productItem(new_item);
//                            index++;
//                        }
//                        else if(number!=lastNumber)
//                        {
//                            new_item.clear();
//                            new_item.push_back({type,material,color,source,price,moq,mpq,itemisdelete});
//                            new_product.push_back({number,imgPath,image,brand,name,new_item,hole,fitting,remark,itemisdelete});
//                        }
//                    }
//                    qDebug()<<"*pproduct_data="<<new_product.size();
//                    pmainWindow->displayData(new_product);
//                    qDebug()<<"*pproduct_data="<<pproduct_data->size();
                }

                break;
            case 1:
                if(data.toArray().size()>=0)
                {
                    QJsonArray brandlist=data.toArray();
                    const int brand_count=brandlist.size();//salesmanlist.size()
                    QList<QString> brand_list;
//                    cout<<"row_count"<<brand_count<<endl;
                    for(int row=0;row<brand_count;row++)
                    {
                        QString brand=brandlist.at(row).toString();
//                        cout<<"acc:"<<account.toStdString()<<endl;
                        brand_list.push_back(brand);
                    }
                    pmainWindow->jump(brand_list);
                }
                break;
            default:
                break;
            }
        }
        //statusCode!=200
        else
        {
//            if(image_load_error==false)
                QMessageBox::warning(NULL , tr("提示"), tr("连接服务器失败！"), QMessageBox::Yes);
        }
        return;
    }

//    void displayData(int currentPage);
    void jump(QList<QString>);
    void ids2Products(QList<int> product_ids);

    void displayData(QList<products> resultList);


signals:
    void logout();

public slots:
    void dealSignals(int,int);//处理信号

private slots:
    void showmanage();
    void showeditpassw();

    void on_pB_manage_clicked();

    void on_pB_editAdminPassword_clicked();

    void on_pB_batchImport_clicked();

    void on_pB_search_clicked();
//    void DisplayData(QList<products> new_products);

    void on_pB_PageUp_clicked();

    void on_pB_PageOn_clicked();

    void on_lineEdit_currentPage_editingFinished();

    void on_pB_batchQuotation_clicked();


    void on_pB_exportProduct_clicked();

    void on_pB_LogOut_clicked();

    void show_main();

    void on_pB_DeleteCache_clicked();

    void on_cB_showDeleted_stateChanged(int arg1);

    void on_comboBox_brand_activated(const QString &arg1);

    void on_pushButton_showAll_clicked();

    void on_pushButton_adjustPrice_clicked();

    void on_cB_showDeleted_clicked();

private:
    Ui::MainWindow *ui;
    producttable *productsmodel;
    buttonDelegate *p_buttons;
    buttonItemDelegate *p_buttonitem;
    imgPathDelegate *p_imgpath_buttons;
    QScrollBar *verScrollBar;
    QList<int> idsList;
    QList<products> resultList;

//    int totalRow;


public:
    static WaitingDialog *pQwait;
    static producttable* pproducttable;
    static MainWindow* pmainWindow;
    static QPushButton* pPageOn;
    static QPushButton* pPageUp;
    static QLineEdit* pPageTo;
    static QLabel* pTotalPages;
    static QHttpTool* qHttpToolforset;
    static QHttpTool* qHttpTool;
    static QExcelTool* qExcelTool;
    static QTableView* ptv_products;
    static QList<QProduct>* result_data;
    QList<QProduct> product_data;
    static QList<QProduct>* pproduct_data;
    static QList<int>* pproduct_ids;
    QList<int> product_ids_record;
    static int blockSize;
    static int currentBlock;
    static int productsSize;
    static int totalpages;
    static int currentPage;
    static int resultSize;
    static int idSize;
    //QStandardItemModel *standItemModel;

};
#endif // MAINWINDOW_H
