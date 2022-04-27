#include "buttondelegate.h"
#include <QApplication>
#include <QMouseEvent>
#include <QMessageBox>
#include <QPainter>
#include <QStyleOption>
#include <QDesktopWidget>
#include "addproductitemdialog.h"
#include "mainwindow.h"
#include "addproductdialog.h"
#include "editproductdialog.h"
#include "logindialog.h"

QObject * buttonDelegate::pbuttonDelegate=NULL;
int buttonDelegate::editIndexRow=0;
int buttonDelegate::editItemIndexRow=0;

buttonDelegate::buttonDelegate(QObject *parent): QItemDelegate(parent)
{
    buttonDelegate::pbuttonDelegate=parent;
    productsList=producttable::pproducttable->getModelData();
    qDebug()<<productsList[0].getp_image().width();
}
buttonDelegate::~buttonDelegate()
{
//    delete pbuttonDelegate;
}
void buttonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QList<products> buttonListPaint=producttable::pproducttable->getModelData();
    QList<QStyleOptionButton*>* buttons = m_btns.value(index);
    if (1) {
        const int row=index.row();
        int productId = 0;
        int productItemId = 0;
        int count = 0;
        for(int i=0;i<buttonListPaint.size();i++){
            int size;
            size=buttonListPaint[i].getItemsSize();
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
        bool temp=buttonListPaint[productId].getp_isDelete();
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
        (const_cast<buttonDelegate *>(this))->m_btns.insert(index, buttons);
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
    if(loginDialog::isAdmin==1)
    {
        QApplication::style()->drawControl(QStyle::CE_PushButton, buttons->at(0), painter);
        QApplication::style()->drawControl(QStyle::CE_PushButton, buttons->at(1), painter);
        QApplication::style()->drawControl(QStyle::CE_PushButton, buttons->at(2), painter);
    }
}

bool buttonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
//    int row = pbuttonDelegate->ui->currentIndex().row();
    if (event->type() == QEvent::MouseButtonPress) {

        QMouseEvent* e =(QMouseEvent*)event;

        if (m_btns.contains(index)) {
            QList<QStyleOptionButton*>* btns = m_btns.value(index);
            if (btns->at(0)->rect.contains(e->x(), e->y())) {
                btns->at(0)->state |= QStyle::State_Sunken;
            }
            else if(btns->at(1)->rect.contains(e->x(), e->y())) {
                btns->at(1)->state |= QStyle::State_Sunken;
            }
            else if(btns->at(2)->rect.contains(e->x(), e->y())) {
                btns->at(2)->state |= QStyle::State_Sunken;
            }
        }
    }
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* e =(QMouseEvent*)event;
//        QApplication::setQuitOnLastWindowClosed(false);
        if (m_btns.contains(index))
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
            QList<QStyleOptionButton*>* btns = m_btns.value(index);
            //修改
            if (btns->at(0)->rect.contains(e->x(), e->y()))
            {
                editIndexRow=productId;
                editItemIndexRow=productItemId;
                btns->at(0)->state &= (~QStyle::State_Sunken);
                //showMsg(tr("btn3 row %1").arg(index.row()));
                editProductDialog *editproduct=new editProductDialog();
                editproduct->show();
            }
            //删除
            else if(btns->at(1)->rect.contains(e->x(), e->y()))
            {
                btns->at(1)->state &= (~QStyle::State_Sunken);
                int row=index.row();
                if(nullptr == model || row < 0 || row > MainWindow::pproducttable->rowCount()-1)
                {
                    QMessageBox::information(NULL,"提示","找不到操作项",QStringLiteral("确定")
                                             ,QStringLiteral("取消"),"");
                    return -1;
                }
                if(nullptr == model || row < 0 || row > MainWindow::pproducttable->rowCount()-1)
                    return -1;
//                QApplication::setQuitOnLastWindowClosed(false);
                int res;
                bool isDelete=false;
                if(btns->at(1)->text==QString("删除"))
                {
                    isDelete=true;
                    QMessageBox *msgBox = new QMessageBox(QMessageBox::Question, tr("提示"), tr("产品确定要删除吗?"), QMessageBox::Yes | QMessageBox::No);
                    msgBox->button(QMessageBox::Yes)->setText("是");
                    msgBox->button(QMessageBox::No)->setText("否");
                    res = msgBox->exec();
                }
                if(btns->at(1)->text==QString("还原"))
                {
                    isDelete=false;
                    QMessageBox *msgBox = new QMessageBox(QMessageBox::Question, tr("提示"), tr("产品确定要还原吗?"), QMessageBox::Yes | QMessageBox::No);
                    msgBox->button(QMessageBox::Yes)->setText("是");
                    msgBox->button(QMessageBox::No)->setText("否");
                    res = msgBox->exec();
                }
                if(res==QMessageBox::Yes)
                {
                    //producttable::pproducttable->setData(index,QColor(Qt::red),Qt::BackgroundColorRole);
                    QList<productItem> productItemList;
                    if(isDelete)
                    {
                        productItemList=productsList[productId].getp_productItemNotConst();
                        productsList[productId].setp_isDelete(true);
                        //删除大项，则小项都被删除
                        for(int i=0;i<productsList[productId].getItemsSize();i++)
                        {
                            if(!productItemList[i].getp_itemIsDelete())
                            {
                                productItemList[i].setp_itemIsDelete(true);
                                qDebug()<<"button_item删除成功"<<productId<<" "<<i;
                            }
                        }
                        productsList[productId].setp_productItem(productItemList);
                        products p=productsList[productId];
                        int row=producttable::pproducttable->getModelRows();
                        btns->at(1)->text=QString("还原");
                        emit producttable::pproducttable->deleteP(p,productsList,row);
                    }
                    else if(!isDelete)
                    {
                        productItemList=productsList[productId].getp_productItemNotConst();
                        productsList[productId].setp_isDelete(false);
                        //还原大项，则小项都被还原
                        for(int i=0;i<productsList[productId].getItemsSize();i++)
                        {
                            if(productItemList[i].getp_itemIsDelete())
                            {
                                productItemList[i].setp_itemIsDelete(false);
                                qDebug()<<"button_item还原成功"<<productId<<" "<<i;
                            }
//                            QList<QStyleOptionButton*>* btnsTemp = m_btns.value(index);
                        }
                        productsList[productId].setp_productItem(productItemList);
                        products p=productsList[productId];
                        int row=producttable::pproducttable->getModelRows();
                        btns->at(1)->text=QString("删除");
                        emit producttable::pproducttable->deleteP(p,productsList,row);
                    }
                }
//                showMsg(tr("btn2 row %1").arg(index.row()));
            }
            //增加
            else if(btns->at(2)->rect.contains(e->x(), e->y()))
            {
                btns->at(2)->state &= (~QStyle::State_Sunken);
                //showMsg(tr("btn3 row %1").arg(index.row()));
                addProductDialog *addproduct=new addProductDialog();
                addproduct->show();
                //connect(addproduct,SIGNAL(addproductitem()),this,SLOT(showaddproductitem()));
                //this->hide();
            }
        }
    }
    return 0;
}

void buttonDelegate::showMsg(QString str)
{
    QMessageBox msg;
    msg.setText(str);
    msg.exec();
}
