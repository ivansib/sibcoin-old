/* 
 * File:   unconfirmedoffers_comparator
 * Author: ivan
 *
 */

#ifndef UNCONFIRMEDOFFERS_COMPARATOR_H
#define UNCONFIRMEDOFFERS_COMPARATOR_H

#include "dexoffer.h"


namespace dex {

struct UnconfirmedOffersComparator {

    bool operator()(const CDexOffer& a, const CDexOffer& b) const {
        return a.timeCreate < b.timeCreate || a.hash < b.hash || a.editingVersion < b.editingVersion;
    }    

};

}

#endif /* UNCONFIRMEDOFFERS_COMPARATOR_H */

