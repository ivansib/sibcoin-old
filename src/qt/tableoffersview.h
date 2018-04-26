#ifndef TABLEOFFERSVIEW_H
#define TABLEOFFERSVIEW_H

#include "tableoffersdialog.h"
#include "offerdetailsview.h"

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

private:
    OfferDetailsView *details;
    const TypeOffer type;

protected Q_SLOTS:
   virtual void clickedButton(const int &index) override;
   virtual void updateTables(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status) override;
   virtual void updateData() override;
   virtual void resizeTable() override;
};

#endif
