#ifndef OFFERDETAILSCREATOR_H
#define OFFERDETAILSCREATOR_H

#include <QDialog>
#include <QDateTime>
#include "offerdetails.h"
#include "walletmodel.h"
#include "ui_offerdetailscreator.h"

class OfferDetailsCreator : public OfferDetails, public Ui::OfferDetailsCreator
{
    Q_OBJECT

public:
    OfferDetailsCreator(DexDB *db, QDialog *parent = nullptr);

    void initData();

private:

    QtMyOfferInfo getMyOffer() const;

protected Q_SLOTS:
    virtual void saveData();
    virtual void sendData();
};

#endif
