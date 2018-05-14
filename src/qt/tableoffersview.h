#ifndef TABLEOFFERSVIEW_H
#define TABLEOFFERSVIEW_H

#include "tableoffersdialog.h"
#include "offerdetailsview.h"
#include "tableofferviewdelegate.h"

class TableOffersView : public TableOffersDialog<QtOfferInfo>
{
    Q_OBJECT

public:
    enum TypeOffer {
        Buy,
        Sell
    };

    TableOffersView(DexDB *db, const TypeOffer &type, const CommonSettingsForOffers::TypeSettings &typeSettings, QDialog *parent = nullptr);
    ~TableOffersView();

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual int countOffers() const override;

private:
    OfferDetailsView *details;
    TableOfferViewDelegate *pDelegate;
    const TypeOffer type;

protected Q_SLOTS:
    virtual void updateTables(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status) override;
    virtual void updateData() override;
    virtual void resizeTable() override;

    void clickedButton(const int &index);
};

#endif
