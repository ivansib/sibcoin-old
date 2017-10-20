#include "combobox.h"

ComboBox::ComboBox(QWidget *parent) : QComboBox(parent)
{

}

void ComboBox::setCurrentData(const QString &data) {
    int index = findData(data);
    setCurrentIndex(index);
}

void ComboBox::addData(const std::list<PaymentMethodInfo> &data)
{
    auto it = data.begin();

    QMap<QString, QString> sort;
    while (it != data.end()) {
        sort[tr(it->name.c_str())] = toString(it->type);
        ++it;
    }

    auto itSort = sort.begin();
    addItem(tr("All"));
    while (itSort != sort.end()) {
        addItem(itSort.key(), itSort.value());
        ++itSort;
    }
}

void ComboBox::addData(const std::list<CountryInfo> &data)
{
    auto it = data.begin();

    QMap<QString, QString> sort;
    while (it != data.end()) {
        sort[tr(it->name.c_str())] = toString(it->iso);
        ++it;
    }

    auto itSort = sort.begin();
    addItem(tr("All"));
    while (itSort != sort.end()) {
        addItem(itSort.key(), itSort.value());
        ++itSort;
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

void ComboBox::addData(const std::list<CurrencyInfo> &data)
{
    auto it = data.begin();

    addItem(tr("All"));
    while (it != data.end()) {
        QString viewStr = toString(it->symbol) + " " + toString(it->iso);
        QString key = toString(it->iso);
        QString toolTip = tr(it->name.c_str());
        addItem(viewStr, key);
        setItemData(count() - 1, toolTip, Qt::ToolTipRole);
        ++it;
    }
}
