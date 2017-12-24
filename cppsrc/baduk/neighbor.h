#ifndef incl_BADUK_NEIGHBOR_H__
#define incl_BADUK_NEIGHBOR_H__

#include <unordered_map>

#include "point.h"

namespace baduk {

class NeighborTable {
public:
    NeighborTable(unsigned int board_size);

    std::vector<Point> const& get(Point const& p) const;

private:
    std::unordered_map<Point, std::vector<Point>> neighbors_;
};

NeighborTable const* getNeighborTable(unsigned int board_size);

}

#endif
