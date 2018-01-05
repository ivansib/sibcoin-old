#ifndef OFFERDETAILSEDITOR_H
#define OFFERDETAILSEDITOR_H

#include <QDialog>
#include "offerdetails.h"
#include "ui_offerdetailseditor.h"

class OfferDetailsEditor : public OfferDetails, public Ui::OfferDetailsEditor
{
    Q_OBJECT

public:
    OfferDetailsEditor(DexDB *db, QDialog *parent = nullptr);

    void setOfferInfo(const QtMyOfferInfo &info);

private:
    QString status(const StatusOffer &s) const;
    QString offerType(const TypeOffer &s) const;

    void enabledHashEditData(const bool &b);

    void updateMyOffer();
    void isApproximateExpiration(const bool &b);

    QtMyOfferInfo offerInfo;

protected Q_SLOTS:
    virtual void saveData();
    virtual void sendData();
};

#endif
