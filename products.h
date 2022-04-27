#ifndef PRODUCTS_H
#define PRODUCTS_H
#include "productitem.h"
#include <QList>
#include <QImage>

class products
{
public:
    products();
    products(int id,QString imgpath,QImage image,QString brand,QString name,
                       QList<productItem> items,QString hole,QString fitting,
                       QString remark,bool isDelete);

    int getp_id() const;
    QString getp_brand() const;
    QString getp_name() const;
    QString getp_imagePath() const;
    QImage getp_image() const;
    QString getp_hole() const;
    QString getp_fitting() const;
    QList<productItem> getp_productItem() const;
    QList<productItem> getp_productItemNotConst();
    QString getp_remark() const;
    bool getp_isDelete() const;
    int getItemsSize() const;

    void setp_id(int num);
    void setp_brand(QString brand);
    void setp_name(QString name);
    void setp_imagePath(QString imgpath);
    void setp_image(QImage image);
    void setp_hole(QString hole);
    void setp_fitting(QString fitting);
    void setp_productItem(QList<productItem> items);
    void setp_remark(QString remark);
    void setp_isDelete(bool isDelete);
    void insertItem(productItem item);
    productItem& at(int i);

    void replaceItems(products dst);

    void saveImage(QString path);

private:
    int p_ID;            //编号
    QString p_brand;    //品牌
    QString p_name;     //中文名
    QString p_imagePath; //图片路径
    QImage p_image;     //图片
    QString p_hole;         //孔位
    QString p_fitting;  //配套件的型号
    QString p_remark;   //备注
    bool p_isDelete;  //是否被删除
    //productItem p_item;
    QList<productItem> p_items; //型号 价格 来源 材料 颜色
};

#endif // PRODUCTS_H
