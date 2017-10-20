#include "combobox.h"

ComboBox::ComboBox(QWidget *parent) : QComboBox(parent)
{

}

void ComboBox::setCurrentData(const QString &data) {
    int index = findData(data);
    setCurrentIndex(index);
}

QString ComboBox::toString(const std::string &str) const
{
    return QString::fromUtf8(str.c_str());
}

QString ComboBox::toString(const uint8_t &i) const
{
    return QString::number(i);
}

void ComboBox::addData(const std::map<std::string, CurrencyInfo> &data)
{
    auto it = data.begin();

    addItem(tr("All"));
    while (it != data.end()) {
        QString viewStr = toString(it->second.symbol) + " " + toString(it->first);
        QString key = toString(it->first);
        QString toolTip = tr(it->second.name.c_str());
        addItem(viewStr, key);
        setItemData(count() - 1, toolTip, Qt::ToolTipRole);
        ++it;
    }
}
