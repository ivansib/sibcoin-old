#ifndef CONVERTDATATESTS_H
#define CONVERTDATATESTS_H

#include <QObject>
#include <QTest>

class ConvertDataTests : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void toUiPriceTests_data();
    void toUiPriceTests();

    void fromUiPriceTests_data();
    void fromUiPriceTests();
};

#endif
