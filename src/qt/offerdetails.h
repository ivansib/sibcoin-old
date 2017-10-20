#ifndef OFFERDETAILS_H
#define OFFERDETAILS_H

#include <QDialog>
#include "dto.h"
#include "dex/dexdb.h"

namespace Ui {
    class OfferDetails;
}

using namespace dex;

class OfferDetails : public QDialog
{
    Q_OBJECT

public:
    OfferDetails(DexDB *db, QDialog *parent = nullptr);
    ~OfferDetails();

    void setOfferInfo(const QtOfferInfo &info);

private:
    Ui::OfferDetails *ui;
    DexDB *db;
};

#endif
