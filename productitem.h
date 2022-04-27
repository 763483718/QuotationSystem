#ifndef PRODUCTITEM_H
#define PRODUCTITEM_H
#include <QString>

class productItem
{
public:
    productItem();
    productItem(QString type,QString material,QString color,QString source,
                float price,QString moq,QString mpq,bool isItemDelete);

    QString getp_type() const;
    float getp_price() const;
    QString getp_material() const;
    QString getp_color() const;
    QString getp_source() const;
    bool getp_itemIsDelete() const;
    QString getp_MOQ() const;
    QString getp_MPQ() const;

    void setp_type(QString ac);
    void setp_price(float price);
    void setp_material(QString material);
    void setp_color(QString color);
    void setp_source(QString source);
    void setp_itemIsDelete(const bool itemIsDelete);
    void setp_MOQ(QString moq);
    void setp_MPQ(QString mpq);

private:
    QString p_type;     //型号
    float p_price;     //价格
    QString p_material; //材料
    QString p_color;    //颜色
    QString p_source;    //来源
    bool p_itemIsDelete;  //是否被删除
    QString p_MOQ;          //最小起订量
    QString p_MPQ;          //最小包装量
};

#endif // PRODUCTITEM_H
