#include <math.h>
#include "pricespinbox.h"

PriceSpinBox::PriceSpinBox(QWidget *parent) : QAbstractSpinBox(parent)
{
    QString integer = "11";
    QString decimals = "6";

    QString reg = QString("^[0-9]{0,%1}(\\.)[0-9]{%2}").arg(integer).arg(decimals);
    lineEdit()->setValidator(new QRegExpValidator(QRegExp(reg), this));
}

void PriceSpinBox::setValue(const quint64 &val)
{
    QString text = toText(val);
    lineEdit()->setText(text);
    Q_EMIT valueChanged(val);
}

quint64 PriceSpinBox::value() const
{
    QString str = lineEdit()->text();
    return fromText(str);
}

void PriceSpinBox::stepBy(int steps)
{
    quint64 num = value();
    int s = steps * pow(10, decimals);
    num += s;
    setValue(num);
}

QString PriceSpinBox::toText(const quint64 &val) const
{
    return ConvertData::toUiPrice(val);
}

quint64 PriceSpinBox::fromText(const QString &str) const
{
    return ConvertData::fromUiPrice(str);
}
