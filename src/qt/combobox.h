#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include "dex/db/dexdto.h"

using namespace dex;

class ComboBox : public QComboBox
{
    Q_OBJECT
public:
    ComboBox(QWidget *parent = nullptr);

    enum ComboType {
        View,
        Editor
    };

    bool setCurrentData(const QString &data);

#if QT_VERSION < 0x050200
    QVariant currentData() const;
#endif

    void addData(const std::list<PaymentMethodInfo> &data, const ComboType &type);
    void addData(const std::list<CountryInfo> &data, const ComboType &type);
    void addData(const std::list<CurrencyInfo> &data, const ComboType &type);
    void addData(const CountryInfo &data);
    void addData(const CurrencyInfo &data);

private:
    QString toString(const std::string &str) const;
    QString toString(const uint8_t &i) const;
};

#endif
