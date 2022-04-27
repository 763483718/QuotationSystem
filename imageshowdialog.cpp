#include "imageshowdialog.h"
#include "ui_imageshowdialog.h"
#include <QImage>
#include <QLabel>

imageShowDialog::imageShowDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::imageShowDialog)
{
    ui->setupUi(this);
//    this->setFixedSize(422,272);
    this->setWindowFlags(Qt::Popup);
}
void imageShowDialog::setImage(QString imgstr)
{
    label=new QLabel();
    QImage Image;
    imgstr=imgstr.replace("\\","/");
    Image.load(imgstr);
    QPixmap pixmap = QPixmap::fromImage(Image);
    int with = 400;
    int height = 300;
    QPixmap fitpixmap = pixmap.scaled(1.7*with, 1.7*height, Qt::KeepAspectRatio, Qt::SmoothTransformation); // 饱满填充
    ui->label->setPixmap(fitpixmap);
}

imageShowDialog::~imageShowDialog()
{
    delete ui;
    delete label;
}
