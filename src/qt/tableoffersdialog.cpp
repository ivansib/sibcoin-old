#include "tableoffersdialog.h"

template<class Offer>
TableOffersDialog<Offer>::TableOffersDialog(DexDB *db, OfferModel<Offer> *model, const int &columnBtn,
                                            const CommonSettingsForOffers::TypeSettings &typeSettings, QDialog *parent)
    : TableOffersSubDialog(db, columnBtn, typeSettings, parent)
{
    pModel = model;
    tableView->setModel(pModel);
}

template<class Offer>
TableOffersDialog<Offer>::~TableOffersDialog()
{
    delete pModel;
}

template<class Offer>
void TableOffersDialog<Offer>::changedFilterCountryIso(const int &) {
    QString iso = cBoxCountry->currentData().toString();
    settings->setCountryIso(typeSettings, iso);
    pModel->setFilterCountryIso(iso);
}

template<class Offer>
void TableOffersDialog<Offer>::changedFilterCurrencyIso(const int &) {
    QString iso = cBoxCurrency->currentData().toString();
    settings->setCurrencyIso(typeSettings, iso);
    pModel->setFilterCurrencyIso(iso);
}

template<class Offer>
void TableOffersDialog<Offer>::changedFilterPaymentMethod(const int &)
{
    quint8 payment = cBoxPayment->currentData().toInt();
    settings->setPaymentMethodType(typeSettings, payment);
    pModel->setFilterPaymentMethod(payment);
}

template<class Offer>
void TableOffersDialog<Offer>::changedFilterOfferType(const int &)
{
    int type = cBoxOffer->currentIndex();
    settings->setTypeOffer(typeSettings, type);
    pModel->setFilterTypeOffer(type);
}

template class TableOffersDialog<QtMyOfferInfo>;
template class TableOffersDialog<QtOfferInfo>;
