#include "counter.h"

namespace baduk {

BoardCounter::BoardCounter(unsigned int, unsigned int) {
}

void BoardCounter::increment(Point p) {
    const auto ptr = map_.find(p);
    if (ptr == map_.end()) {
        map_[p] = 1;
    } else {
        ptr->second += 1;
    }
}

unsigned int BoardCounter::get(Point p) const {
    const auto ptr = map_.find(p);
    if (ptr == map_.end()) {
        return 0;
    }
    return ptr->second;
}

}
