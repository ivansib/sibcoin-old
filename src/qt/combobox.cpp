#include "combobox.h"
#include "convertdata.h"

ComboBox::ComboBox(QWidget *parent) : QComboBox(parent)
{

}

void ComboBox::setCurrentData(const QString &data) {
    int index = findData(data);
    setCurrentIndex(index);
}

void ComboBox::addData(const std::list<PaymentMethodInfo> &data, const ComboType &type)
{
    clear();

    auto it = data.begin();

    if (type == View) {
        addItem(tr("All"));
    }

    while (it != data.end()) {
        addItem(ConvertData::toTr(it->name.c_str()), toString(it->type));
        ++it;
    }

    if (type == View) {
        setCurrentIndex(1);
    }
}

void ComboBox::addData(const std::list<CountryInfo> &data, const ComboType &type)
{
    clear();

    if (type == View) {
        addItem(tr("All"));
    }

    auto it = data.begin();

    while (it != data.end()) {
        if (it->enabled) {
            QString name = ConvertData::toTr(it->name.c_str());
            QString iso = toString(it->iso);

            addItem(name, iso);
        }
        ++it;
    }
}

QString ComboBox::toString(const std::string &str) const
{
    return QString::fromUtf8(str.c_str());
}

QString ComboBox::toString(const uint8_t &i) const
{
    return QString::number(i);
}

void ComboBox::addData(const std::list<CurrencyInfo> &data, const ComboType &type)
{
    clear();

    QFont f("FontAwesome");
    setFont(f);

    auto it = data.begin();

    if (type == View) {
        addItem(tr("All"));
    }

    while (it != data.end()) {
        if (it->enabled) {
            QString viewStr = toString(it->symbol) + " " + toString(it->iso);
            QString key = toString(it->iso);
            QString toolTip = ConvertData::toTr(it->name.c_str());
            addItem(viewStr, key);
            setItemData(count() - 1, toolTip, Qt::ToolTipRole);
        }
        ++it;
    }
}
