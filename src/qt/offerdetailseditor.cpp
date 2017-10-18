#include "offerdetailseditor.h"
#include "ui_offerdetailseditor.h"

OfferDetailsEditor::OfferDetailsEditor(QDialog *parent) : QDialog(parent), ui(new Ui::OfferDetailsEditor)
{
    ui->setupUi(this);
}

OfferDetailsEditor::~OfferDetailsEditor()
{
    delete ui;
}

void OfferDetailsEditor::setOfferInfo(const QtOfferInfo &info)
{

}
