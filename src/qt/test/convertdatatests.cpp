#include "convertdatatests.h"
#include "../convertdata.h"

void ConvertDataTests::toUiPriceTests_data()
{
    QTest::addColumn<quint64>("price");
    QTest::addColumn<QString>("strPrice");
    QTest::addColumn<bool>("status");

    quint64 price = 0;
    QString strPrice = "0";
    bool status = true;

    QTest::newRow("compare1") << price << strPrice << status;

    price = 100;
    strPrice = "0.000001";
    status = true;

    QTest::newRow("compare2") << price << strPrice << status;

    price = 10000000000;
    strPrice = "100";
    status = true;

    QTest::newRow("compare3") << price << strPrice << status;

    price = 10000000023;
    strPrice = "100.00000023";
    status = true;

    QTest::newRow("compare4") << price << strPrice << status;

    price = 10000004500;
    strPrice = "100.000045";
    status = true;

    QTest::newRow("compare5") << price << strPrice << status;

    price = 123000000000073;
    strPrice = "1230000.00000073";
    status = true;

    QTest::newRow("compare6") << price << strPrice << status;

    price = 123000000000073;
    strPrice = "123000000.00000073";
    status = false;

    QTest::newRow("compare7") << price << strPrice << status;
}

void ConvertDataTests::toUiPriceTests()
{
    QFETCH(quint64, price);
    QFETCH(QString, strPrice);
    QFETCH(bool, status);

    QString calcStrPrice = ConvertData::toUiPrice(price);
    bool b = calcStrPrice == strPrice;

    QCOMPARE(b, status);
}

void ConvertDataTests::fromUiPriceTests_data()
{
    QTest::addColumn<quint64>("price");
    QTest::addColumn<QString>("strPrice");
    QTest::addColumn<bool>("status");

    quint64 price = 0;
    QString strPrice = "0";
    bool status = true;

    QTest::newRow("compare1") << price << strPrice << status;

    price = 100;
    strPrice = "0.000001";
    status = true;

    QTest::newRow("compare2") << price << strPrice << status;

    price = 10000000000;
    strPrice = "100";
    status = true;

    QTest::newRow("compare3") << price << strPrice << status;

    price = 10000000023;
    strPrice = "100.00000023";
    status = true;

    QTest::newRow("compare4") << price << strPrice << status;

    price = 10000004500;
    strPrice = "100.000045";
    status = true;

    QTest::newRow("compare5") << price << strPrice << status;

    price = 123000000000073;
    strPrice = "1230000.00000073";
    status = true;

    QTest::newRow("compare6") << price << strPrice << status;

    price = 123000000000073;
    strPrice = "12300700.00000073";
    status = false;

    QTest::newRow("compare7") << price << strPrice << status;
}

void ConvertDataTests::fromUiPriceTests()
{
    QFETCH(quint64, price);
    QFETCH(QString, strPrice);
    QFETCH(bool, status);

    quint64 calcPrice = ConvertData::fromUiPrice(strPrice);
    bool b = calcPrice == price;
    QCOMPARE(b, status);
}
