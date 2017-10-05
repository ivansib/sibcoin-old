#include "paymentmethodtype.h"
#include "defaultdatafordb.h"

namespace dex {

PaymentMethodType::PaymentMethodType()
{
    initListType();
}

bool PaymentMethodType::isValid(unsigned char &type)
{
    if (setType.count(type) > 0) {
        return true;
    }

    return false;
}

void PaymentMethodType::initListType()
{
    DefaultDataForDb def;

    std::list<DefaultPaymentMethod> payments = def.dataPaymentMethods();

    setType.clear();
    for (auto item : payments) {
        setType.insert(item.type);
    }
}

}
