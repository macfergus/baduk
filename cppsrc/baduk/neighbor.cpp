#include <memory>

#include "neighbor.h"

namespace baduk {

NeighborTable::NeighborTable(unsigned int num_rows, unsigned int num_cols) :
    num_cols_(num_cols),
    neighbors_([num_rows, num_cols]() {
        std::vector<std::vector<Point>> table;
        for (unsigned int r = 0; r < num_rows; ++r) {
            for (unsigned int c = 0; c < num_cols; ++c) {
                std::vector<Point> neighbors;
                if (r > 0) {
                    neighbors.push_back(Point(r - 1, c));
                }
                if (r < num_rows - 1) {
                    neighbors.push_back(Point(r + 1, c));
                }
                if (c > 0) {
                    neighbors.push_back(Point(r, c - 1));
                }
                if (c < num_cols - 1) {
                    neighbors.push_back(Point(r, c + 1));
                }
                table.push_back(neighbors);
            }
        }
        return table;
    }()) {
}

std::vector<Point> const& NeighborTable::get(Point const& p) const {
    unsigned int index = num_cols_ * p.row() + p.col();
    return neighbors_.at(index);
}

NeighborTable const* getNeighborTable(
        unsigned int num_rows, unsigned int num_cols) {
    // Max supported board size is 25x25.
    static std::vector<std::unique_ptr<NeighborTable>> tables(25 * 25 + 1);

    const auto index = num_rows * 25 + num_cols;
    NeighborTable const* rv = tables[index].get();
    if (rv == nullptr) {
        tables[index] = std::make_unique<NeighborTable>(num_rows, num_cols);
        rv = tables[index].get();
    }

    return rv;
}

}
