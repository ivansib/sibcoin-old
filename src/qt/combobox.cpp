#include "combobox.h"

ComboBox::ComboBox(QWidget *parent) : QComboBox(parent)
{

}

QString ComboBox::toString(const std::string &str)
{
    return QString::fromUtf8(str.c_str());
}

QString ComboBox::toString(const uint8_t &i)
{
    return QString::number(i);
}
