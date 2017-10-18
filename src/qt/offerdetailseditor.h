#ifndef OFFERDETAILSEDITOR_H
#define OFFERDETAILSEDITOR_H

#include <QDialog>
#include "dto.h"

namespace Ui {
    class OfferDetailsEditor;
}

class OfferDetailsEditor : public QDialog
{
    Q_OBJECT

public:
    OfferDetailsEditor(QDialog *parent = nullptr);
    ~OfferDetailsEditor();

    void setOfferInfo(const QtOfferInfo &info);

private:
    Ui::OfferDetailsEditor *ui;
};

#endif
