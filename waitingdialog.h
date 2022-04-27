#ifndef WAITINGDIALOG_H
#define WAITINGDIALOG_H

#include <QDialog>

namespace Ui {
class WaitingDialog;
}

class WaitingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WaitingDialog(QWidget *parent = nullptr);
    ~WaitingDialog();

private:
    Ui::WaitingDialog *ui;
    QMovie* m_Move;
};

#endif // WAITINGDIALOG_H
