#ifndef OFFERDETAILSEDITOR_H
#define OFFERDETAILSEDITOR_H

#include <QDialog>
#include <QMessageBox>
#include "offerdetails.h"
#include "ui_offerdetailseditor.h"

const int maxNumEditsMyOffer = 11;

class OfferDetailsEditor : public OfferDetails, public Ui::OfferDetailsEditor
{
    Q_OBJECT

public:
    OfferDetailsEditor(DexDB *db, QDialog *parent = nullptr);

    void setOfferInfo(const QtMyOfferInfo &info);
    QMessageBox::StandardButton isDeleteOffer(const StatusOffer &status);

private:
    QString status(const StatusOffer &s) const;
    QString offerType(const TypeOffer &s) const;

    void enabledHashEditLines(const bool &b);
    void turnLines(const StatusOffer &status);
    void turnButtons(const StatusOffer &status);

    void updateMyOffer();
    void isApproximateExpiration(const bool &b);

    QtMyOfferInfo offerInfo;

protected Q_SLOTS:
    virtual void saveData();
    virtual void sendData();

    void deleteData();

Q_SIGNALS:
    void offerDelete(QtMyOfferInfo);
};

#endif
