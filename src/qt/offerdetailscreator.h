#ifndef OFFERDETAILSCREATOR_H
#define OFFERDETAILSCREATOR_H

#include "offerdetails.h"
#include "walletmodel.h"

class OfferDetailsCreator : public OfferDetails
{
    Q_OBJECT

public:
    OfferDetailsCreator(DexDB *db, QDialog *parent = nullptr);

    void initData();
    void setModel(WalletModel *model);

private:
    WalletModel *model;

    QtMyOfferInfo getMyOffer() const;

protected Q_SLOTS:
    virtual void saveData();
    virtual void sendData();
};

#endif
