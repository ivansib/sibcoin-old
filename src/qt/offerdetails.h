#ifndef OFFERDETAILS_H
#define OFFERDETAILS_H

#include <QDialog>
#include "dto.h"

namespace Ui {
    class OfferDetails;
}

class OfferDetails : public QDialog
{
    Q_OBJECT

public:
    OfferDetails(QDialog *parent = nullptr);
    ~OfferDetails();

    void setOfferInfo(const QtOfferInfo &info, const QString &country, const QString &currency, const QString &payment);

private:
    Ui::OfferDetails *ui;
};

#endif
