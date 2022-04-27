#ifndef IMGPATHDELEGATE_H
#define IMGPATHDELEGATE_H

#include <QItemDelegate>
#include "products.h"

class imgPathDelegate:public QItemDelegate
{
public:
    explicit imgPathDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
    void setImg_List();
    static QList<QString>* pImg_list;

signals:

public slots:

private:
    void showMsg(QString str);

private:

    typedef QMap<QModelIndex, QPair<QStyleOptionButton*, QStyleOptionButton*>* >  imgPathButtons;
    imgPathButtons m_btns;
    QList<QString> img_list;
    QList<products> productsList;
};

#endif // IMGPATHDELEGATE_H
