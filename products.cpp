#include "products.h"

products::products()
{
    p_hole = "";
    p_isDelete = false;
}

products::products(int id,QString imgpath,QImage image,QString brand,QString name,
                   QList<productItem> items,QString hole,QString fitting,
                   QString remark,bool isDelete)
{
    this->p_ID=id;
    this->p_imagePath = imgpath;
    this->p_image = image.copy();
    this->p_brand=brand;
    this->p_name=name;
    this->p_items=items;
    this->p_hole=hole;
    this->p_fitting=fitting;
    this->p_remark=remark;
    this->p_isDelete=isDelete;
}

int products::getp_id() const
{

    return this->p_ID;
}
QString products::getp_brand() const
{
    return this->p_brand;
}
QString products::getp_name() const
{
    return this->p_name;
}
QString products::getp_imagePath() const
{
    return this->p_imagePath;
}
QImage products::getp_image() const
{
    return this->p_image;
}

QList<productItem> products::getp_productItem() const
{
    return this->p_items;
}
QList<productItem> products::getp_productItemNotConst()
{
    return this->p_items;
}

QString products::getp_remark() const
{
    return this->p_remark;
}
QString products::getp_hole() const
{
    return this->p_hole;
}
bool products::getp_isDelete() const
{
    return this->p_isDelete;
}
int products::getItemsSize() const
{
    return p_items.size();
}
QString products::getp_fitting() const
{
    return p_fitting;
}
void products::setp_id(int num)
{
    this->p_ID=num;
}
void products::setp_brand(QString brand)
{
    this->p_brand=brand;
}
void products::setp_name(QString name)
{
    this->p_name=name;
}
void products::setp_imagePath(QString imgpath)
{
    this->p_imagePath=imgpath;
}
void products::setp_image(QImage image)
{
    this->p_image = image.copy();
}
void products::setp_hole(QString hole)
{
    this->p_hole=hole;
}
void products::setp_fitting(QString fitting)
{
    this->p_fitting=fitting;
}
void products::setp_productItem(QList<productItem> items)
{
    this->p_items=items;
}
void products::setp_remark(QString remark)
{
    this->p_remark=remark;
}
void products::setp_isDelete(bool isDelete)
{
    this->p_isDelete=isDelete;
}
void products::insertItem(productItem item)
{
    this->p_items.append(item);
}
productItem& products::at(int i)
{
    return p_items[i];
}
void products::saveImage(QString path)
{
    if(p_image.isNull()){
        return;
    }
    p_image.save(path);
}

void products::replaceItems(products dst)
{
    for(int i = 0;i < dst.getItemsSize();i++){
        QString type = dst.at(i).getp_type();
        bool flag = false;
        for(int j = 0;j < p_items.size();j++){
            if(p_items.at(j).getp_type() == type){
                p_items.replace(j,dst.at(i));
                flag = true;
            }
        }
        if(!flag){
            p_items.append(dst.at(i));
        }
    }
}
