#include "filterlist.h"
#include "ui_filterlist.h"
#include "filteritemeditor.h"
#include "convertdata.h"

FilterList::FilterList(DexDB *db, QWidget *parent) : QWidget(parent), ui(new Ui::FilterList), db(db)
{
    ui->setupUi(this);

    connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(clikedAdd()));
    connect(ui->btnDelete, SIGNAL(clicked()), this, SLOT(clickedDelete()));

    initList();
}

FilterList::~FilterList()
{
    delete ui;
}

void FilterList::saveData()
{
    for (auto item : addItems) {
        db->addFilter(ConvertData::fromQString(item));
    }

    for (auto item : deleteItems) {
        db->deleteFilter(ConvertData::fromQString(item));
    }
}

void FilterList::cancel()
{
    initList();
}

void FilterList::initList()
{
    addItems.clear();
    deleteItems.clear();

    auto list = db->getFilters();

    for (auto item : list) {
        ui->listWidget->addItem(ConvertData::toQString(item));
    }
}

void FilterList::clikedAdd()
{
    FilterItemEditor f;
    connect(&f, SIGNAL(newItem(QString)), this, SLOT(addItem(QString)));
    int i = f.exec();

    if (i == 1) {
        Q_EMIT dataChanged();
    }
}

void FilterList::clickedDelete()
{
    int current = ui->listWidget->currentRow();
    auto item = ui->listWidget->takeItem(current);

    if (item != nullptr) {
        QString str = item->text();
        addItems.remove(str);
        deleteItems << str;

        Q_EMIT dataChanged();
    }
}

void FilterList::addItem(const QString &item)
{
    ui->listWidget->addItem(item);

    deleteItems.remove(item);
    addItems << item;
}
