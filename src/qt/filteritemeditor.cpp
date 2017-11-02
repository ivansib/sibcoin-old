#include "filteritemeditor.h"
#include "ui_filteritemeditor.h"

FilterItemEditor::FilterItemEditor(QDialog *parent) : QDialog(parent), ui(new Ui::FilterItemEditor)
{
    ui->setupUi(this);

    connect(ui->btnBox, &QDialogButtonBox::accepted, this, &FilterItemEditor::clickedOk);
    connect(ui->btnBox, &QDialogButtonBox::rejected, this, &FilterItemEditor::close);
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
