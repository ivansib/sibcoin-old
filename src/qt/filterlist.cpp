#include "filterlist.h"
#include "ui_filterlist.h"

FilterList::FilterList(DexDB *db, QWidget *parent) : QWidget(parent), ui(new Ui::FilterList), db(db)
{
    ui->setupUi(this);
}

FilterList::~FilterList()
{
    delete ui;
}
