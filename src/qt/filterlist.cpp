#include "filterlist.h"
#include "ui_filterlist.h"
#include "filteritemeditor.h"

FilterList::FilterList(DexDB *db, QWidget *parent) : QWidget(parent), ui(new Ui::FilterList), db(db)
{
    ui->setupUi(this);

    connect(ui->btnAdd, &QPushButton::clicked, this, &FilterList::clikedAdd);
    connect(ui->btnDelete, &QPushButton::clicked, this, &FilterList::clickedDelete);
}

FilterList::~FilterList()
{
    delete ui;
}

void FilterList::clikedAdd()
{
    FilterItemEditor f;
    connect(&f, &FilterItemEditor::newItem, this, &FilterList::addItem);
    f.exec();
}

void FilterList::clickedDelete()
{
    int current = ui->listWidget->currentRow();
    ui->listWidget->takeItem(current);
}

void FilterList::addItem(const QString &item)
{
    ui->listWidget->addItem(item);
}
