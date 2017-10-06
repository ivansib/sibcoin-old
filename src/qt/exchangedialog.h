#ifndef EXCHANGEDIALOG_H
#define EXCHANGEDIALOG_H

#include <QDialog>

namespace Ui {
    class ExchangeDialog;
}

class ExchangeDialog : public QDialog
{
    Q_OBJECT

public:
    ExchangeDialog(QDialog *parent = nullptr);
    ~ExchangeDialog();

private:
    Ui::ExchangeDialog *ui;
};

#endif
