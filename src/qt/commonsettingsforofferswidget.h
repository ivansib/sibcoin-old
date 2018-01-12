#ifndef COMMONSETTINGSFOROFFERSWIDGET_H
#define COMMONSETTINGSFOROFFERSWIDGET_H

#include <QWidget>
#include "ui_commonsettingsforofferswidget.h"
#include "dex/dexdb.h"

class CommonSettingsForOffersWidget : public QWidget, public Ui::CommonSettingsForOffersWidget
{
    Q_OBJECT

public:
    CommonSettingsForOffersWidget(DexDB *db, QWidget *parent = nullptr);

private:
    DexDB *db;
};

#endif
