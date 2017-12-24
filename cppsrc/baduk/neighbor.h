#ifndef incl_BADUK_NEIGHBOR_H__
#define incl_BADUK_NEIGHBOR_H__

#include <vector>

#include "point.h"

namespace baduk {

class NeighborTable {
public:
    NeighborTable(unsigned int num_rows, unsigned int num_cols);

    std::vector<Point> const& get(Point const& p) const;

private:
    unsigned int num_rows_;
    unsigned int num_cols_;
    std::vector<std::vector<Point>> neighbors_;
};

NeighborTable const* getNeighborTable(
    unsigned int num_rows, unsigned int num_cols);


}

#endif
