#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(DexDB *db, QDialog *parent) : QDialog(parent), ui(new Ui::SettingsDialog), db(db)
{
    ui->setupUi(this);

    common = new CommonSettingsForOffersWidget(db);
    countries = new TableCountries(db);
    currencies = new TableCurrencies(db);
    filters = new FilterList(db);

    ui->stackedWidget->addWidget(common);
    ui->stackedWidget->addWidget(countries);
    ui->stackedWidget->addWidget(currencies);
    ui->stackedWidget->addWidget(filters);

    ui->btnBox->setEnabled(false);

    connect(ui->btnCommon, SIGNAL(clicked()), this, SLOT(currentCommon()));
    connect(ui->btnCountries, SIGNAL(clicked()), this, SLOT(currentCountries()));
    connect(ui->btnCurrencies, SIGNAL(clicked()), this, SLOT(currentCurrencies()));
    connect(ui->btnFilters, SIGNAL(clicked()), this, SLOT(currentFilters()));

    connect(common, SIGNAL(dataChanged()), this, SLOT(changedData()));
    connect(countries, SIGNAL(dataChanged()), this, SLOT(changedData()));
    connect(currencies, SIGNAL(dataChanged()), this, SLOT(changedData()));
    connect(filters, SIGNAL(dataChanged()), this, SLOT(changedData()));

    QPushButton* btnApply = ui->btnBox->button(QDialogButtonBox::Apply);
    connect(btnApply, SIGNAL(clicked()), this, SLOT(saveData()));
    connect(ui->btnBox, SIGNAL(rejected()), this, SLOT(cancel()));

    currentCommon();
    ui->btnCommon->setChecked(true);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::update()
{
    common->update();
}

void SettingsDialog::currentCommon()
{
    ui->stackedWidget->setCurrentWidget(common);
    common->update();
}

void SettingsDialog::currentCountries()
{
    ui->stackedWidget->setCurrentWidget(countries);
}

void SettingsDialog::currentCurrencies()
{
    ui->stackedWidget->setCurrentWidget(currencies);
}

void SettingsDialog::currentFilters()
{
    ui->stackedWidget->setCurrentWidget(filters);
}

void SettingsDialog::changedData()
{
    ui->btnBox->setEnabled(true);
}

void SettingsDialog::saveData()
{
    common->saveData();
    countries->saveData();
    currencies->saveData();
    filters->saveData();
    ui->btnBox->setEnabled(false);

    Q_EMIT dataChanged();
}

void SettingsDialog::cancel()
{
    common->cancel();
    countries->cancel();
    currencies->cancel();
    filters->cancel();
    ui->btnBox->setEnabled(false);
}

