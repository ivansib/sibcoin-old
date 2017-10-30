#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(QDialog *parent = nullptr);
    ~SettingsDialog();

private:
    Ui::SettingsDialog *ui;

private Q_SLOTS:
    void currentCommon();
    void currentCountries();
    void currentCurrencies();
    void currentFilters();
};

#endif
