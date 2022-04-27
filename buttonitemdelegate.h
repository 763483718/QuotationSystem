#ifndef BUTTONITEMDELEGATE_H
#define BUTTONITEMDELEGATE_H
#include <QItemDelegate>
#include "producttable.h"

class buttonItemDelegate:public QItemDelegate
{
public:
    buttonItemDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
//    QList<products> productsList;
    static int newItemIndexRow;
    static int editItemIndexRow;
    static int editIndexRow;

signals:

public slots:

private:
    void showMsg(QString str);

private:

    typedef QMap<QModelIndex, QList<QStyleOptionButton*>* >  collButtonitem;
    collButtonitem m_btnitem;
    QList<products> productsList;
    QList<productItem> productItemList;
};

#endif // BUTTONITEMDELEGATE_H
