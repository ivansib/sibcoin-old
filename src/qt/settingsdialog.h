#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include "tablecountries.h"
#include "tablecurrencies.h"
#include "filterlist.h"
#include "commonsettingsforofferswidget.h"
#include "dex/db/dexdb.h"

using namespace dex;

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(DexDB *db, QDialog *parent = nullptr);
    ~SettingsDialog();

    void update();

private:
    Ui::SettingsDialog *ui;
    DexDB *db;

    CommonSettingsForOffersWidget *common;
    TableCountries *countries;
    TableCurrencies *currencies;
    FilterList *filters;

private Q_SLOTS:
    void currentCommon();
    void currentCountries();
    void currentCurrencies();
    void currentFilters();
    void changedData();
    void saveData();
    void cancel();

Q_SIGNALS:
    void dataChanged();
};

#endif
