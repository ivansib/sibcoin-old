#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QDialog *parent) : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

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
//    ui->stackedWidget->setCurrentWidget();
}

void SettingsDialog::currentCountries()
{
//    ui->stackedWidget->setCurrentWidget();
}

void SettingsDialog::currentCurrencies()
{
//    ui->stackedWidget->setCurrentWidget();
}

void SettingsDialog::currentFilters()
{
//    ui->stackedWidget->setCurrentWidget();
}

