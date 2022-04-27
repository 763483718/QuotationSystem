#ifndef IMAGESHOWDIALOG_H
#define IMAGESHOWDIALOG_H

#include <QDialog>
#include <QLabel>

namespace Ui {
class imageShowDialog;
}

class imageShowDialog : public QDialog
{
    Q_OBJECT

public:
    explicit imageShowDialog(QWidget *parent = nullptr);
    ~imageShowDialog();
    void setImage(QString imgstr);


private:
    Ui::imageShowDialog *ui;
    QLabel *label;
};

#endif // IMAGESHOWDIALOG_H
