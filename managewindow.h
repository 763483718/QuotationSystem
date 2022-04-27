#ifndef MANAGEWINDOW_H
#define MANAGEWINDOW_H

#include <QDialog>
#include "salesmantable.h"
#include "qhttptool.h"
#include <QMessageBox>
#include <QApplication>

namespace Ui {
class managewindow;
}

class managewindow : public QDialog
{
    Q_OBJECT

public:
    explicit managewindow(QWidget *parent = nullptr);
    ~managewindow();
    void setData();
    QList<salesman> getData();
    void addRow();
    void delRow();
    static void interfaceJump(QJsonObject info,int requestCode, QJsonValue data)
    {
        if(info.value("statusCode").toInt() == 200){//right
            switch (requestCode)
            {
            case 0:
                if(data.toArray().size()>=0)
                {
                    QJsonArray salesmanlist=data.toArray();
                    const int row_count=salesmanlist.size();//salesmanlist.size()
                    QList<salesman> new_data;
//                    cout<<"row_count"<<row_count<<endl;
                    for(int row=0;row<row_count;row++)
                    {
                        int id=salesmanlist.at(row)["id"].toInt();
//                        cout<<"id:"<<id<<endl;
                        QString account=salesmanlist.at(row)["account"].toString();
//                        cout<<"acc:"<<account.toStdString()<<endl;
                        QString password=salesmanlist.at(row)["password"].toString();
//                        cout<<"pwd:"<<password.toStdString()<<endl;
                        new_data.push_back({id,account,password});
                    }
                   emit pmanageWindow->DisplayData(new_data);
                }
                else
                {
                }
                break;
            case 1:
                cout<<"datadatadat"<<data.toBool()<<endl;
                if(data.toBool())
                {
//                    pmanageWindow->setData();
                    pmanageWindow->jump1();
                }

                else if(!data.toBool())
                {
                    QMessageBox::warning(NULL , tr("提示"), tr("添加失败"), QMessageBox::Yes);
                }
                break;
            case 2:
                if(data.toBool())
                {
                    pmanageWindow->jump2();
                }
                else if(!data.toBool())
                {
                    QMessageBox::warning(NULL , tr("提示"), tr("删除失败"), QMessageBox::Yes);
                }
                break;
            default:
                break;
            }
        }
        else
        {
//            QApplication::setQuitOnLastWindowClosed(false);
            cout<<"lProductserrorerrorerror"<<endl;
            QMessageBox::warning(NULL , tr("提示"), tr("连接服务器失败！"), QMessageBox::Yes);
        }
        return;
    }
    void jump1();
    void jump2();

signals:
    void manage();

public slots:


private slots:
    void on_pB_homepage_clicked();

    void on_pB_addsalesman_clicked();

    void on_pB_editsalesman_clicked();

    void on_pB_deletesalesman_clicked();
    void DisplayData(QList<salesman> new_data);

private:
    QList<salesman> salesmanVec;
    Ui::managewindow *ui;
    salesmantable *mymodel;
    static managewindow* pmanageWindow;
    QHttpTool qHttpTool;
};

#endif // MANAGEWINDOW_H
