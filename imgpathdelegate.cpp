#include "imgpathdelegate.h"
#include <QApplication>
#include <QMouseEvent>
#include <QMessageBox>
#include <QPainter>
#include <QStyleOption>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QDebug>
#include "addproductitemdialog.h"
#include "mainwindow.h"
#include "logindialog.h"
#include <iostream>
#include "producttable.h"
#include "imageshowdialog.h"
using namespace std;

QList<QString>* imgPathDelegate::pImg_list=NULL;
imgPathDelegate::imgPathDelegate(QObject *parent): QItemDelegate(parent)
{
    setImg_List();
//    m_list.push_back("C:/Users/Aaron/Pictures/1.jpg");
//    m_list.push_back("C:/Users/Aaron/Pictures/0.jpg");
}

void imgPathDelegate::setImg_List()
{
    productsList=producttable::pproducttable->getModelData();
    for(int i=0;i<productsList.size();i++)
    {
        if(!productsList.at(i).getp_imagePath().isNull())
        {
            img_list.push_back(productsList.at(i).getp_imagePath());
//            qDebug()<<productsList.size()<<productsList.at(i).getp_imagePath();
        }
    }
    pImg_list=&img_list;
}

void imgPathDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QList<products> productsListPaint=producttable::pproducttable->getModelData();
//    cout<<"imgpath:"<<productsListPaint.at(productsListPaint.size()-1).getp_imagePath().toStdString()<<endl;
    QPair<QStyleOptionButton*, QStyleOptionButton*>* buttons = m_btns.value(index);
    if (1) //!button
    {
        const int row=index.row();
        int* const productId = new int(0);
        int* const productItemId = new int(0);
        int count = 0;
        for(int i=0;i<productsListPaint.size();i++){
            int size;
            size=productsListPaint[i].getItemsSize();
            if(count + size >= row+1){
                *productId = i;
                *productItemId = row - count;
                break;
            }
            count += size;
        }
        QStyleOptionButton* button1 = new QStyleOptionButton();
        button1->state |= QStyle::State_Enabled;
        button1->features=QStyleOptionButton::Flat;
        button1->iconSize = QSize(option.rect.width()/3*2, option.rect.height()/3*2);
        QImage img1;
        img1.load(QString(productsListPaint.at(*productId).getp_imagePath()));//imgPathDelegate::pImg_list

        if(imgPathDelegate::pImg_list->size()>0)
            button1->icon = QIcon(QPixmap::fromImage(img1)); //
        else
            qDebug()<<"没有图片";
        QStyleOptionButton* button2 = new QStyleOptionButton();
        button2->text = "";//查看大图
        button2->state |= QStyle::State_Enabled;
        button2->features=QStyleOptionButton::Flat;
        buttons =new  QPair<QStyleOptionButton*, QStyleOptionButton*>(button1, button2);
        (const_cast<imgPathDelegate *>(this))->m_btns.insert(index, buttons);
    }
    buttons->first->rect = option.rect.adjusted(8, 8, -8 , -(option.rect.height()/4 + 8));
    buttons->second->rect = option.rect.adjusted(8, buttons->first->rect.height() + 16, -8, -8);
    buttons->second->rect.setHeight(30);
    painter->save();

    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());

    }
    painter->restore();
    QApplication::style()->drawControl(QStyle::CE_PushButton, buttons->first, painter);
    if(loginDialog::isAdmin==1)
    {
        QApplication::style()->drawControl(QStyle::CE_PushButton, buttons->second, painter);
    }
}

bool imgPathDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress) {

        QMouseEvent* e =(QMouseEvent*)event;

        if (m_btns.contains(index)) {
            QPair<QStyleOptionButton*, QStyleOptionButton*>* btns = m_btns.value(index);
            if (btns->first->rect.contains(e->x(), e->y())) {
                btns->first->state |= QStyle::State_Sunken;
            }
            else if(btns->second->rect.contains(e->x(), e->y())) {
                btns->second->state |= QStyle::State_Sunken;
            }
        }
    }
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* e =(QMouseEvent*)event;
//        QApplication::setQuitOnLastWindowClosed(true);
        if (m_btns.contains(index)) {
            QList<products> productsListPaint=producttable::pproducttable->getModelData();
            const int row=index.row();
            int* const productId = new int(0);
            int* const productItemId = new int(0);
            int count = 0;
            for(int i=0;i<productsListPaint.size();i++){
                int size;
                size=productsListPaint[i].getItemsSize();
                if(count + size >= row+1){
                    *productId = i;
                    *productItemId = row - count;
                    break;
                }
                count += size;
            }
            QPair<QStyleOptionButton*, QStyleOptionButton*>* btns = m_btns.value(index);
            if (btns->first->rect.contains(e->x(), e->y())) {
                btns->first->state &= (~QStyle::State_Sunken);
//                showMsg(tr("btn1 column %1").arg(index.column()));
//                QString str=QString("C:/Users/Aaron/Pictures/0.jpg");
                imageShowDialog *imageShow=new imageShowDialog(MainWindow::pmainWindow);
                imageShow->setImage(QString(productsListPaint.at(*productId).getp_imagePath()));//imgPathDelegate::pImg_list->at(*productId)
                imageShow->show();
            } else if(btns->second->rect.contains(e->x(), e->y())) {
                btns->second->state &= (~QStyle::State_Sunken);
//                QString curPath=QDir::currentPath();//获取当前路径
//                QString str=QFileDialog::getOpenFileName(NULL,"打开文件",curPath);
//                qDebug()<<str;
//                QImage imgNew;
//                imgNew.load(str);
//                btns->first->icon = QIcon(QPixmap::fromImage(imgNew)); //
////                ui->label_showimg->setPixmap(loadImageFunction(str,ui->label_showimg->size()));
//                showMsg(tr("btn2 row %1").arg(index.row()));
            }
        }
    }
    return 0;
}

void imgPathDelegate::showMsg(QString str)
{
    QMessageBox msg;
    msg.setText(str);
    msg.exec();
}
