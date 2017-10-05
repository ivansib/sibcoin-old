#ifndef PAYMENTMETHODTYPE_H
#define PAYMENTMETHODTYPE_H

#include <set>

namespace dex {

class PaymentMethodType {
public:
    PaymentMethodType();

    bool isValid(unsigned char &type);

private:
    void initListType();
    std::set<unsigned char> setType;
};

}

#endif
