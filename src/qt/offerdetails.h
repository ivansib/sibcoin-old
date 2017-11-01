#ifndef OFFERDETAILS_H
#define OFFERDETAILS_H

#include <QDialog>
#include <QDateTime>
#include "dex/dexdb.h"
#include "dto.h"
#include "ui_offerdetailseditor.h"

using namespace dex;

class OfferDetails : public QDialog, public Ui::OfferDetailsEditor
{
    Q_OBJECT

public:
    enum Type {
        Edit,
        Create
    };

    OfferDetails(DexDB *db, const Type &type, QDialog *parent = nullptr);
    virtual ~OfferDetails();

private:
    DexDB *db;
    const Type type;

    const void initMode();

public Q_SLOTS:
    void updateNavigationData();

protected Q_SLOTS:
    virtual void saveData() = 0;

private Q_SLOTS:
    void changedTimeToExpiration(const int &i);
    void changedShortInfo();

Q_SIGNALS:
    void dataChanged(const QtMyOfferInfo &info);
};


#endif
