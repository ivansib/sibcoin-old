#include "filteritemeditor.h"
#include "ui_filteritemeditor.h"

FilterItemEditor::FilterItemEditor(QDialog *parent) : QDialog(parent), ui(new Ui::FilterItemEditor)
{
    ui->setupUi(this);

    connect(ui->btnBox, SIGNAL(accepted()), this, SLOT(clickedOk()));
    connect(ui->btnBox, SIGNAL(rejected()), this, SLOT(close()));
}

FilterItemEditor::~FilterItemEditor()
{
    delete ui;
}

void FilterItemEditor::clickedOk()
{
    Q_EMIT newItem(ui->lEdit->text().toLower());
    close();
}
