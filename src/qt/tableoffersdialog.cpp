#include <math.h>
#include "tableoffersdialog.h"

template<class Offer>
TableOffersDialog<Offer>::TableOffersDialog(DexDB *db, OfferModel<Offer> *model, const int &columnBtn,
                                            const CommonSettingsForOffers::TypeSettings &typeSettings, QDialog *parent)
    : TableOffersSubDialog(db, columnBtn, typeSettings, parent)
{
    pModel = model;
    tableView->setModel(pModel);
    page = 0;
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

    if (iso == "all") {
        countryIso = "";
    } else {
        countryIso = iso.toUtf8().constData();
    }

    page = 0;
    updateData();
}

template<class Offer>
void TableOffersDialog<Offer>::changedFilterCurrencyIso(const int &) {
    QString iso = cBoxCurrency->currentData().toString();
    settings->setCurrencyIso(typeSettings, iso);

    if (iso == "all") {
        currencyIso = "";
    } else {
        currencyIso = iso.toUtf8().constData();
    }

    page = 0;
    updateData();
}

template<class Offer>
void TableOffersDialog<Offer>::changedFilterPaymentMethod(const int &)
{
    quint8 payment = cBoxPayment->currentData().toInt();
    paymentMethod = payment;
    settings->setPaymentMethodType(typeSettings, payment);

    page = 0;
    updateData();
}

template<class Offer>
void TableOffersDialog<Offer>::changedFilterOfferType(const int &)
{
    int type = cBoxOffer->currentIndex();
    offerType = type-1;
    settings->setTypeOffer(typeSettings, type);

    page = 0;
    updateData();
}

template<class Offer>
void TableOffersDialog<Offer>::firstPage()
{
    if (page > 0) {
        page = 0;
        updateData();
    }
}

template<class Offer>
void TableOffersDialog<Offer>::prevPage()
{
    if (page > 0) {
        page--;
        updateData();
    }
}

template<class Offer>
void TableOffersDialog<Offer>::nextPage()
{
    if (rowOnPage() * (page+1) < countOffers()) {
        page++;
        updateData();
    }
}

template<class Offer>
void TableOffersDialog<Offer>::lastPage()
{
    int p = std::floor(static_cast<float>(countOffers()) / rowOnPage());

    if (rowOnPage() * p == countOffers()) {
        p--;
    }

    if (p != page) {
        page = p;
        updateData();
    }
}

template<class Offer>
void TableOffersDialog<Offer>::updatePageInfo()
{
    int first = page * rowOnPage() + 1;
    int last = (page+1) * rowOnPage();

    int all = countOffers();

    if (all == 0) {
        first = 0;
    }

    if (last > all) {
        last = all;
    }

    QString info = QString("%1-%2/%3").arg(QString::number(first)).arg(QString::number(last)).arg(QString::number(all));

    labelCounts->setText(info);

    if (all == 0 || all <= rowOnPage() || rowOnPage() == 0) {
        btnFirstPage->setEnabled(false);
        btnPrevPage->setEnabled(false);
        btnNextPage->setEnabled(false);
        btnLastPage->setEnabled(false);
    } else if (first == 1) {
        btnFirstPage->setEnabled(false);
        btnPrevPage->setEnabled(false);
        btnNextPage->setEnabled(true);
        btnLastPage->setEnabled(true);
    } else if (last == all) {
        btnFirstPage->setEnabled(true);
        btnPrevPage->setEnabled(true);
        btnNextPage->setEnabled(false);
        btnLastPage->setEnabled(false);
    } else {
        btnFirstPage->setEnabled(true);
        btnPrevPage->setEnabled(true);
        btnNextPage->setEnabled(true);
        btnLastPage->setEnabled(true);
    }
}

template<class Offer>
int TableOffersDialog<Offer>::rowOnPage() const
{
    if (settings->getShowMaxRowsTables()) {
        return 0;
    }

    return settings->getNumRowsTables();
}


template class TableOffersDialog<QtMyOfferInfo>;
template class TableOffersDialog<QtOfferInfo>;
