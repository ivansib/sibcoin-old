#ifndef OFFERDETAILSEDITOR_H
#define OFFERDETAILSEDITOR_H

#include <QDialog>
#include "dex/dexdb.h"
#include "dto.h"

using namespace dex;

namespace Ui {
    class OfferDetailsEditor;
}

class OfferDetailsEditor : public QDialog
{
    Q_OBJECT

public:
    OfferDetailsEditor(DexDB *db, QDialog *parent = nullptr);
    ~OfferDetailsEditor();

    void setOfferInfo(const QtOfferInfo &info);

private:
    Ui::OfferDetailsEditor *ui;
    DexDB *db;

private Q_SLOTS:
    void saveData();

Q_SIGNALS:
    void dataChanged(const QtOfferInfo &info);
};

#endif
