#ifndef COMMONSETTINGSFOROFFERSWIDGET_H
#define COMMONSETTINGSFOROFFERSWIDGET_H

#include <QWidget>
#include "ui_commonsettingsforofferswidget.h"
#include "commonsettingsforoffers.h"
#include "commonsettingsforoffers.h"
#include "dex/db/dexdb.h"

class CommonSettingsForOffersWidget : public QWidget, public Ui::CommonSettingsForOffersWidget
{
    Q_OBJECT

public:
    CommonSettingsForOffersWidget(DexDB *db, QWidget *parent = nullptr);
    ~CommonSettingsForOffersWidget();

    void saveData();
    void cancel();

private:
    DexDB *db;
    CommonSettingsForOffers *settings;

    void setData();

private Q_SLOTS:
    void changedData();
    void changedData(int);
    void changedData(quint64);
    void changedShowMaxRows(bool isCheked);

Q_SIGNALS:
    void dataChanged();
};

#endif
