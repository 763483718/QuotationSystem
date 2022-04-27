#ifndef BUTTONDELEGATE_H
#define BUTTONDELEGATE_H

#include <QItemDelegate>
#include "products.h"

class buttonDelegate:public QItemDelegate
{
public:
    explicit buttonDelegate(QObject *parent = 0);
    ~buttonDelegate();
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
    static int editIndexRow;
    static int editItemIndexRow;

signals:

public slots:

private:
    void showMsg(QString str);

private:

    typedef QMap<QModelIndex, QList<QStyleOptionButton*>* >  collButtons;
    collButtons m_btns;
    static QObject * pbuttonDelegate;
    QList<products> productsList;


};

#endif // BUTTONDELEGATE_H
