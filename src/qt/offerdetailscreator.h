#ifndef OFFERDETAILSCREATOR_H
#define OFFERDETAILSCREATOR_H

#include <QDialog>
#include <QDateTime>
#include "offerdetails.h"
#include "walletmodel.h"
#include "commonsettingsforoffers.h"
#include "ui_offerdetailscreator.h"

class OfferDetailsCreator : public OfferDetails, public Ui::OfferDetailsCreator
{
    Q_OBJECT

public:
    OfferDetailsCreator(DexDB *db, QDialog *parent = nullptr);
    ~OfferDetailsCreator();

    void initData();

private:
    QtMyOfferInfo getMyOffer(bool &isError) const;

    CommonSettingsForOffers *settings;

protected Q_SLOTS:
    virtual void saveData();
    virtual void sendData();
};

#endif
