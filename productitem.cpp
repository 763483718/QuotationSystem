#include "productitem.h"
#include <iostream>
using namespace std;

productItem::productItem()
{
    p_itemIsDelete = false;
    p_MOQ="";
    p_MPQ="";
    p_price = -1;
    p_material = "";
    p_source = "";
    p_color = "";
//    p_MOQ = QString("").toInt();
//    p_MPQ = QString("").toInt();
}
productItem::productItem(QString type,QString material,QString color,QString source,
                         float price,QString moq,QString mpq,bool isItemDelete)
{
    this->p_type=type;
    this->p_color=color;
    this->p_price=price;
    this->p_source=source;
    this->p_material=material;
    this->p_itemIsDelete=isItemDelete;
    this->p_MOQ=moq;
    this->p_MPQ=mpq;
}

QString productItem::getp_type() const
{
    return this->p_type;
}
float productItem::getp_price() const
{
    return this->p_price;
}
QString productItem::getp_material() const
{
    return this->p_material;
}
QString productItem::getp_color() const
{
    return this->p_color;
}
QString productItem::getp_source() const
{
    return p_source;
}

bool productItem::getp_itemIsDelete() const
{
    return p_itemIsDelete;
}
QString productItem::getp_MOQ() const
{
    return this->p_MOQ;
}
QString productItem::getp_MPQ() const
{
    return this->p_MPQ;
}
void productItem::setp_type(QString type)
{
    this->p_type=type;
}
void productItem::setp_source(QString source)
{
    this->p_source=source;
}
void productItem::setp_material(QString material)
{
    this->p_material=material;
}
void productItem::setp_color(QString color)
{
    this->p_color=color;
}
void productItem::setp_price(float price)
{
    this->p_price=price;
}
void productItem::setp_itemIsDelete(const bool itemIsDelete)
{
    this->p_itemIsDelete=itemIsDelete;
}
void productItem::setp_MOQ(QString moq)
{
    this->p_MOQ=moq;
}
void productItem::setp_MPQ(QString mpq)
{
    this->p_MPQ=mpq;
}
