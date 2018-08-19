#ifndef incl_BADUK_COUNTER_H__
#define incl_BADUK_COUNTER_H__

#include <map>

#include "point.h"

namespace baduk {

class BoardCounter {
public:
    BoardCounter(unsigned int num_rows, unsigned int num_cols);

    void increment(Point p);
    unsigned int get(Point p) const;

private:
    std::map<Point, unsigned int> map_;
};

}

#endif
