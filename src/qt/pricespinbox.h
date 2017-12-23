#ifndef PRICESPINBOX_H
#define PRICESPINBOX_H

#include <QAbstractSpinBox>
#include <QLineEdit>
#include "convertdata.h"

class PriceSpinBox : public QAbstractSpinBox
{
    Q_OBJECT

public:
    PriceSpinBox(QWidget *parent = nullptr);

    void setValue(const quint64 &val);
    quint64 value() const;

    virtual void stepBy(int steps);

protected:
    virtual StepEnabled stepEnabled() const {
        StepEnabled s = StepNone;

        quint64 num = value();
        if (num <= 0) {
            s = StepUpEnabled;
        } else if (num >= 9999999000000000000ULL) {
            s = StepDownEnabled;
        } else {
            s = StepUpEnabled | StepDownEnabled;
        }

        return s;
    }

private:
    QString toText(const quint64 &val) const;
    quint64 fromText(const QString &str) const;
};

#endif
