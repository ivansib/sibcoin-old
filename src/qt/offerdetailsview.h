#ifndef OFFERDETAILSVIEW_H
#define OFFERDETAILSVIEW_H

#include <QDialog>
#include "dto.h"
#include "dex/db/dexdb.h"

namespace Ui {
    class OfferDetailsView;
}

using namespace dex;

class OfferDetailsView : public QDialog
{
    Q_OBJECT

public:
    OfferDetailsView(DexDB *db, QDialog *parent = nullptr);
    ~OfferDetailsView();

    void setOfferInfo(const QtOfferInfo &info);

private:
    Ui::OfferDetailsView *ui;
    DexDB *db;
};

#endif
