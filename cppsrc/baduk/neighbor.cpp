#include "neighbor.h"

namespace baduk {

NeighborTable::NeighborTable(unsigned int board_size) :
    neighbors_([board_size]() {
        std::unordered_map<Point, std::vector<Point>> table;
        for (unsigned int r = 0; r < board_size; ++r) {
            for (unsigned int c = 0; c < board_size; ++c) {
                std::vector<Point> neighbors;
                if (r > 0) {
                    neighbors.push_back(Point(r - 1, c));
                }
                if (r < board_size - 1) {
                    neighbors.push_back(Point(r + 1, c));
                }
                if (c > 0) {
                    neighbors.push_back(Point(r, c - 1));
                }
                if (c < board_size - 1) {
                    neighbors.push_back(Point(r, c + 1));
                }
                table.emplace(Point(r, c), neighbors);
            }
        }
        return table;
    }()) {
}

std::vector<Point> const& NeighborTable::get(Point const& p) const {
    return neighbors_.at(p);
}

NeighborTable const* getNeighborTable(unsigned int board_size) {
    static std::unordered_map<unsigned int, NeighborTable> tables;

    if (tables.find(board_size) == tables.end()) {
        tables.emplace(board_size, board_size);
    }

    return &tables.at(board_size);
}

}
