#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(DexDB *db, QDialog *parent) : QDialog(parent), ui(new Ui::SettingsDialog), db(db)
{
    ui->setupUi(this);

    common = new QWidget();
    countries = new TableCountries(db);
    currencies = new TableCurrencies(db);
    filters = new FilterList(db);

    ui->stackedWidget->addWidget(common);
    ui->stackedWidget->addWidget(countries);
    ui->stackedWidget->addWidget(currencies);
    ui->stackedWidget->addWidget(filters);

    ui->btnBox->setEnabled(false);

    connect(ui->btnCommon, &QPushButton::clicked, this, &SettingsDialog::currentCommon);
    connect(ui->btnCountries, &QPushButton::clicked, this, &SettingsDialog::currentCountries);
    connect(ui->btnCurrencies, &QPushButton::clicked, this, &SettingsDialog::currentCurrencies);
    connect(ui->btnFilters, &QPushButton::clicked, this, &SettingsDialog::currentFilters);

    connect(countries, &TableCountries::dataChanged, this, &SettingsDialog::changedData);
    connect(currencies, &TableCurrencies::dataChanged, this, &SettingsDialog::changedData);

    QPushButton* btnApply = ui->btnBox->button(QDialogButtonBox::Apply);
    connect(btnApply, &QPushButton::clicked, this, &SettingsDialog::saveData);
    connect(ui->btnBox, &QDialogButtonBox::rejected, this, &SettingsDialog::cancel);
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

void SettingsDialog::changedData()
{
    ui->btnBox->setEnabled(true);
}

void SettingsDialog::saveData()
{
    countries->saveData();
    currencies->saveData();
    ui->btnBox->setEnabled(false);

    Q_EMIT dataChanged();
}

void SettingsDialog::cancel()
{
    countries->cancel();
    currencies->cancel();
    ui->btnBox->setEnabled(false);
}

