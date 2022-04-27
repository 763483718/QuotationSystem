#ifndef ADJUSTPRICEDIALOG_H
#define ADJUSTPRICEDIALOG_H

#include <QDialog>

namespace Ui {
class adjustPriceDialog;
}

class adjustPriceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit adjustPriceDialog(QWidget *parent = nullptr);
    ~adjustPriceDialog();

public slots:
    void dealSignals(int,int);//处理信号

private slots:
    void on_radioButton_up_clicked(bool checked);

    void on_radioButton_down_clicked(bool checked);

    void on_buttonBox_accepted();

private:
    Ui::adjustPriceDialog *ui;
    bool priceUp;
    bool priceDown;
    int adjustValue;
};

#endif // ADJUSTPRICEDIALOG_H
