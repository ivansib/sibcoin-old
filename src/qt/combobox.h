#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include "dex/dexdto.h"

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

    void setCurrentData(const QString &data);

    void addData(const std::list<PaymentMethodInfo> &data, const ComboType &type);
    void addData(const std::list<CountryInfo> &data, const ComboType &type);
    void addData(const std::list<CurrencyInfo> &data, const ComboType &type);

private:
    QString toString(const std::string &str) const;
    QString toString(const uint8_t &i) const;
};

#endif
