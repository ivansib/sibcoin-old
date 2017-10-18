#ifndef OFFERDETAILS_H
#define OFFERDETAILS_H

#include <QDialog>
#include "dex/dexdto.h"

namespace Ui {
    class OfferDetails;
}

using namespace dex;

class OfferDetails : public QDialog
{
    Q_OBJECT

public:
    OfferDetails(QDialog *parent = nullptr);
    ~OfferDetails();

    void setOfferInfo(const OfferInfo &info, const QString &country, const QString &currency, const QString &payment);

private:
    Ui::OfferDetails *ui;
    OfferInfo info;
};

#endif
