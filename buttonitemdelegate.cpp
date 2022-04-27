#include "buttonitemdelegate.h"
#include <QApplication>
#include <QMouseEvent>
#include <QMessageBox>
#include <QPainter>
#include <QStyleOption>
#include <QDesktopWidget>
#include "addproductitemdialog.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <iostream>
#include "producttable.h"
#include "editproductitemdialog.h"
using namespace std;

int buttonItemDelegate::newItemIndexRow=0;
int buttonItemDelegate::editIndexRow=0;
int buttonItemDelegate::editItemIndexRow=0;

buttonItemDelegate::buttonItemDelegate(QObject *parent): QItemDelegate(parent)
{
    productsList=producttable::pproducttable->getModelData();

}
void buttonItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QList<products> buttonItemListPaint=producttable::pproducttable->getModelData();
    QList<QStyleOptionButton*>* buttons = m_btnitem.value(index);
    if (1) {//!buttons
        const int row=index.row();
        int productId = 0;
        int productItemId = 0;
        int count = 0;
        for(int i=0;i<buttonItemListPaint.size();i++){
            int size;
            size=buttonItemListPaint[i].getItemsSize();
            if(count + size >= row+1){
                productId = i;
                productItemId = row - count;
                break;
            }
            count += size;
        }
        QStyleOptionButton* button1 = new QStyleOptionButton();
        button1->text = "修改";
        button1->state |= QStyle::State_Enabled;

        QStyleOptionButton* button2 = new QStyleOptionButton();
        bool temp=buttonItemListPaint[productId].getp_productItemNotConst()[productItemId].getp_itemIsDelete();
        if(temp)
            button2->text = "还原";
        else
            button2->text = "删除";
        button2->state |= QStyle::State_Enabled;

        QStyleOptionButton* button3 = new QStyleOptionButton();
        button3->text = "新增";
        button3->state |= QStyle::State_Enabled;

        buttons =new  QList<QStyleOptionButton*>;
        buttons->push_back(button1);
        buttons->push_back(button2);
        buttons->push_back(button3);
        (const_cast<buttonItemDelegate *>(this))->m_btnitem.insert(index, buttons);
    }
    buttons->at(0)->rect = option.rect.adjusted(8, 8, -8 , -(option.rect.height()/3*2 + 8)); //
    buttons->at(1)->rect = option.rect.adjusted(8, buttons->at(0)->rect.height() + 16 , -8, -(option.rect.height()/3 + 8));
    buttons->at(2)->rect = option.rect.adjusted(8, 2*buttons->at(0)->rect.height() + 28, -8, -8);
    buttons->at(0)->rect.setHeight(40);
    buttons->at(1)->rect.setHeight(40);
    buttons->at(2)->rect.setHeight(40);
    painter->save();

    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    }
    painter->restore();
    QApplication::style()->drawControl(QStyle::CE_PushButton, buttons->at(0), painter);
    QApplication::style()->drawControl(QStyle::CE_PushButton, buttons->at(1), painter);
    QApplication::style()->drawControl(QStyle::CE_PushButton, buttons->at(2), painter);

}

bool buttonItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress) {

        QMouseEvent* e =(QMouseEvent*)event;

        if (m_btnitem.contains(index)) {
            QList<QStyleOptionButton*>* btnitem = m_btnitem.value(index);
            if (btnitem->at(0)->rect.contains(e->x(), e->y())) {
                btnitem->at(0)->state |= QStyle::State_Sunken;
            }
            else if(btnitem->at(1)->rect.contains(e->x(), e->y())) {
                btnitem->at(1)->state |= QStyle::State_Sunken;
            }
            else if(btnitem->at(2)->rect.contains(e->x(), e->y())) {
                btnitem->at(2)->state |= QStyle::State_Sunken;
            }
        }
    }
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* e =(QMouseEvent*)event;
//        QApplication::setQuitOnLastWindowClosed(false);
        if (m_btnitem.contains(index))
        {
            productsList=producttable::pproducttable->getModelData();
            int row=index.row();
            int productId = 0;
            int productItemId = 0;
            int count = 0;
            for(int i=0;i<productsList.size();i++){
                int size = productsList[i].getp_productItem().size();
                if(count + size >= row+1){
                    productId = i;
                    productItemId = row - count;
                    break;
                }
                count += size;
            }
            QList<QStyleOptionButton*>* btnitem = m_btnitem.value(index);
            //修改
            if (btnitem->at(0)->rect.contains(e->x(), e->y()))
            {
                editIndexRow=productId;
                editItemIndexRow=productItemId;
                btnitem->at(0)->state &= (~QStyle::State_Sunken);
                //showMsg(tr("btn1 row %1").arg(index.row()));
                editProductItemDialog *editproductitem=new editProductItemDialog();//this
//                qDebug()<<productId<<" "<<productItemId;
                editproductitem->show();
            }
            //删除
            else if(btnitem->at(1)->rect.contains(e->x(), e->y()))
            {
                btnitem->at(1)->state &= (~QStyle::State_Sunken);
                int row=index.row();
                if(nullptr == MainWindow::pproducttable || row < 0 || row > MainWindow::pproducttable->rowCount()-1)
                {
                    QMessageBox::information(NULL,"提示","找不到操作项",QStringLiteral("确定")
                                             ,QStringLiteral("取消"),"");
                    return -1;
                }
                if(nullptr == MainWindow::pproducttable || row < 0 || row > MainWindow::pproducttable->rowCount()-1)
                    return -1;
//                QApplication::setQuitOnLastWindowClosed(false);
                QMessageBox::StandardButton box;
                bool isDelete=false;
                int res;
                if(btnitem->at(1)->text==QString("删除"))
                {
                    isDelete=true;
                    QMessageBox *msgBox = new QMessageBox(QMessageBox::Question, tr("提示"), tr("项目确定要删除吗?"), QMessageBox::Yes | QMessageBox::No);
                    msgBox->button(QMessageBox::Yes)->setText("是");
                    msgBox->button(QMessageBox::No)->setText("否");
                    res = msgBox->exec();
                }
                if(btnitem->at(1)->text==QString("还原"))
                {
                    isDelete=false;
                    QMessageBox *msgBox = new QMessageBox(QMessageBox::Question, tr("提示"), tr("项目确定要还原吗?"), QMessageBox::Yes | QMessageBox::No);
                    msgBox->button(QMessageBox::Yes)->setText("是");
                    msgBox->button(QMessageBox::No)->setText("否");
                    res = msgBox->exec();
                }
                if(res==QMessageBox::Yes)
                {
//                    producttable::pproducttable->setData(index,QColor(Qt::red),Qt::BackgroundColorRole);
                    if(producttable::pproducttable==NULL)
                    {
                        QMessageBox::information(NULL,"提示","指针为空",QStringLiteral("确定")
                                                 ,QStringLiteral("取消"),"");
                        return -1;
                    }
                    else
                    {
                        //删除
                        if(isDelete)
                        {
                            qDebug()<<"item删除"<<productId<<" "<<productItemId;
                            productItemList=productsList[productId].getp_productItem();
                            productItemList[productItemId].setp_itemIsDelete(true);
                            productsList[productId].setp_productItem(productItemList);
                            //当一个产品内的项目都delete了，这个产品也应该是delete
                            if(!productsList[productId].getp_isDelete())
                            {
                                bool isAllDelete=true;
                                for(int i=0;i<productItemList.size();i++)
                                {
                                    //存在一个没有删除
                                    if(!productItemList[i].getp_itemIsDelete())
                                    {
                                        isAllDelete=false;
                                        break;
                                    }
                                }
                                if(isAllDelete)
                                    productsList[productId].setp_isDelete(true);
                            }
                            products p=productsList[productId];
                            int row=producttable::pproducttable->getModelRows();
                            cout<<row<<"sssssitem"<<productItemList[productItemId].getp_itemIsDelete()<<endl;
                            btnitem->at(1)->text=QString("还原");
                            emit producttable::pproducttable->deleteP(p,productsList,row);
                        }
                        //还原
                        else if(!isDelete)
                        {
                            qDebug()<<"item还原"<<productId<<" "<<productItemId;                          
                            productItemList=productsList[productId].getp_productItem();
                            cout<<"sssssitem"<<productItemList[productItemId].getp_itemIsDelete()<<endl;
                            productItemList[productItemId].setp_itemIsDelete(false);
                            productsList[productId].setp_productItem(productItemList);
                            //当delete的产品有一个项目还原时，就是isnotdelete，但是该产品的其他项目的这个值不变
                            if(productsList[productId].getp_isDelete())
                                productsList[productId].setp_isDelete(false);
                            products p=productsList[productId];
                            int row=producttable::pproducttable->getModelRows();
                            btnitem->at(1)->text=QString("删除");
                            emit producttable::pproducttable->deleteP(p,productsList,row);
                        }
                    }
                }
//                showMsg(tr("btn2 row %1").arg(index.row()));
            }
            //新增
            else if(btnitem->at(2)->rect.contains(e->x(), e->y()))
            {
                btnitem->at(2)->state &= (~QStyle::State_Sunken);
//                showMsg(tr("btn3 row %1").arg(index.row()));
                addProductItemDialog *addproductitem=new addProductItemDialog();//this
                int num=index.row();
                newItemIndexRow=num;
                addproductitem->show();
            }
        }
    }
    return 0;
}

void buttonItemDelegate::showMsg(QString str)
{
    QMessageBox msg;
    msg.setText(str);
    msg.exec();
}
