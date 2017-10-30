#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(DexDB *db, QDialog *parent) : QDialog(parent), ui(new Ui::SettingsDialog), db(db)
{
    ui->setupUi(this);

    common = new QWidget();
    countries = new TableCountries(db);
    currencies = new QWidget();
    filters = new QWidget();

    ui->stackedWidget->addWidget(common);
    ui->stackedWidget->addWidget(countries);
    ui->stackedWidget->addWidget(currencies);
    ui->stackedWidget->addWidget(filters);

    connect(ui->btnCommon, &QPushButton::clicked, this, &SettingsDialog::currentCommon);
    connect(ui->btnCountries, &QPushButton::clicked, this, &SettingsDialog::currentCountries);
    connect(ui->btnCurrencies, &QPushButton::clicked, this, &SettingsDialog::currentCurrencies);
    connect(ui->btnFilters, &QPushButton::clicked, this, &SettingsDialog::currentFilters);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::currentCommon()
{
    ui->stackedWidget->setCurrentWidget(common);
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

