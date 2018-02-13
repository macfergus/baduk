#include <algorithm>
#include <cassert>
#include <iostream>

#include "board.h"

namespace baduk {

// I is excluded on purpose
const std::string COLS = "ABCDEFGHJKLMNOPQRSTUVWXYZ";

class StringPoolEmpty : public std::exception {};

Board::Board() :
        zobrist_(zobrist::ZobristCodes::get()),
        num_rows_(19),
        num_cols_(19),
        neighbors_(getNeighborTable(19, 19)),
        hashcode_(zobrist_.emptyBoard()) {
    grid_.fill(EMPTY);
}

Board::Board(unsigned int num_rows, unsigned int num_cols) :
        zobrist_(zobrist::ZobristCodes::get()),
        num_rows_(num_rows),
        num_cols_(num_cols),
        neighbors_(getNeighborTable(num_rows, num_cols)),
        hashcode_(zobrist_.emptyBoard()) {
    grid_.fill(EMPTY);
}

bool Board::isEmpty(Point p) const {
    return grid_[index(p)] == EMPTY;
}

template<typename Container, typename T>
bool contains(Container container, T elem) {
    return std::find(container.begin(), container.end(), elem) !=
        container.end();
}

void Board::place(Point point, Stone player) {
    assert(isEmpty(point));
    std::vector<StringIdx> adjacent_other_color;

    const auto new_string_idx = getUnusedString();
    auto& new_string = strings_[new_string_idx];
    new_string.setColor(player);
    new_string.addPoint(point);
    for (Point neighbor : neighbors_->get(point)) {
        auto neighbor_string = grid_[index(neighbor)];
        if (neighbor_string == EMPTY) {
            new_string.addLiberty(neighbor);
        } else if (strings_[neighbor_string].color() == player) {
            new_string.merge(strings_[neighbor_string]);
        } else {
            if (!contains(adjacent_other_color, neighbor_string)) {
                adjacent_other_color.push_back(neighbor_string);
            }
        }
    }
    replace(new_string_idx);

    // Update hash code.
    hashcode_ ^= zobrist_.getEmpty(point);
    hashcode_ ^= zobrist_.getStone(player, point);

    for (auto const& other_color_string : adjacent_other_color) {
        auto& enemy_string = strings_[other_color_string];
        enemy_string.removeLiberty(point);
        if (enemy_string.numLiberties() == 0) {
            remove(other_color_string);
        }
    }
}

bool Board::willCapture(Point point, Stone player) const {
    for (Point neighbor : neighbors_->get(point)) {
        const auto neighbor_string_idx = grid_[index(neighbor)];
        if (neighbor_string_idx == EMPTY) {
            continue;
        }
        const auto& neighbor_string = strings_[neighbor_string_idx];
        if (neighbor_string.color() == player) {
            continue;
        }
        if (neighbor_string.numLiberties() == 1) {
            return true;
        }
    }
    return false;
}

bool Board::willHaveNoLiberties(Point point, Stone player) const {
    // Does NOT check if it is a capture! Call willCapture first.
    for (Point neighbor : neighbors_->get(point)) {
        const auto neighbor_string_idx = grid_[index(neighbor)];
        if (neighbor_string_idx == EMPTY) {
            // This point will be a liberty.
            return false;
        } else {
            const auto& neighbor_string = strings_[neighbor_string_idx];
            if (neighbor_string.color() == player) {
                if (neighbor_string.numLiberties() > 1) {
                    // This string will still have a liberty after placing the
                    // stone.
                    return false;
                }
            }
        }
    }
    return true;
}

void Board::replace(StringIdx new_string_idx) {
    const GoString& new_string = strings_[new_string_idx];
    for (auto point : new_string.stones()) {
        if (grid_[index(point)] != EMPTY) {
            recycle(grid_[index(point)]);
        }
        grid_[index(point)] = new_string_idx;
    }
}

Stone Board::at(Point p) const {
    return strings_[grid_[index(p)]].color();
}

GoString Board::stringAt(Point p) const {
    return strings_[grid_[index(p)]];
}

std::vector<Point> const& Board::neighbors(Point p) const {
    return neighbors_->get(p);
}

unsigned int Board::index(Point p) const {
    return num_cols_ * p.row() + p.col();
}

void Board::remove(StringIdx old_string_idx) {
    const GoString& old_string = strings_[old_string_idx];
    for (auto const& point : old_string.stones()) {
        std::vector<StringIdx> strings_to_update;
        for (Point neighbor : neighbors_->get(point)) {
            auto neighbor_string_idx = grid_[index(neighbor)];
            if (neighbor_string_idx == EMPTY) {
                continue;
            }
            if (contains(strings_to_update, neighbor_string_idx)) {
                continue;
            }
            if (neighbor_string_idx != old_string_idx) {
                strings_to_update.push_back(neighbor_string_idx);
            }
        }
        for (auto string_to_update : strings_to_update) {
            strings_[string_to_update].addLiberty(point);
        }

        hashcode_ ^= zobrist_.getStone(old_string.color(), point);
        hashcode_ ^= zobrist_.getEmpty(point);

        grid_[index(point)] = EMPTY;
    }
    recycle(old_string_idx);
}

void Board::validate() const {
    for (const auto& go_string_idx : grid_) {
        if (go_string_idx == EMPTY) {
            continue;
        }
        const GoString& go_string = strings_[go_string_idx];
        for (const auto point : go_string.stones()) {
            assert(!isEmpty(point));
            assert(at(point) == go_string.color());
        }
        for (const auto point : go_string.liberties()) {
            assert(isEmpty(point));
        }
    }
}

bool Board::operator==(Board const& b) const {
    if (num_rows_ != b.num_rows_ || num_cols_ != b.num_cols_) {
        return false;
    }
    for (unsigned int r = 0; r < num_rows_; ++r) {
        for (unsigned int c = 0; c < num_cols_; ++c) {
            Point p(r, c);
            if (isEmpty(p) != b.isEmpty(p)) {
                return false;
            }
            if (!isEmpty(p)) {
                if (at(p) != b.at(p)) {
                    return false;
                }
            }
        }
    }
    return true;
}

zobrist::hashcode Board::hash() const {
    return hashcode_;
}

zobrist::hashcode Board::hashAfter(Point point, Stone stone) const {
    auto hashcode = hash();
    const auto opponent = other(stone);

    // Place the new stone.
    hashcode ^= zobrist_.getEmpty(point);
    hashcode ^= zobrist_.getStone(stone, point);

    for (Point neighbor : neighbors_->get(point)) {
        const auto neighbor_string_idx = grid_[index(neighbor)];
        if (neighbor_string_idx == EMPTY) {
            continue;
        }
        const GoString& neighbor_string = strings_[neighbor_string_idx];
        if (neighbor_string.color() == stone) {
            continue;
        }
        if (neighbor_string.numLiberties() == 1) {
            for (const auto& p : neighbor_string.stones()) {
                hashcode ^= zobrist_.getStone(opponent, p);
                hashcode ^= zobrist_.getEmpty(p);
            }
        }
    }

    return hashcode;
}

std::ostream& operator<<(std::ostream& out, Board const& board) {
    for (int r = board.numRows() - 1; r >= 0; --r) {
        if (r + 1 < 10) {
            out << " ";
        }
        out << r + 1 << " ";
        for (unsigned int c = 0; c < board.numCols(); ++c) {
            baduk::Point p(static_cast<unsigned int>(r), c);
            if (board.isEmpty(p)) {
                out << ".";
            } else {
                const auto stone = board.at(p);
                out << (stone == baduk::Stone::black ? "x" : "o");
            }
        }
        out << "\n";
    }
    out << "   ";
    for (unsigned int c = 0; c < board.numCols(); ++c) {
        out << COLS.at(c);
    }
    out << "\n";

    return out;
}

StringIdx Board::getUnusedString() {
    for (unsigned int i = 0; i < strings_.size(); ++i) {
        if (!used_[i]) {
            used_.set(i);
            strings_[i].clear();
            return static_cast<StringIdx>(i);
        }
    }
    throw StringPoolEmpty();
}

void Board::recycle(StringIdx string_idx) {
    used_.reset(string_idx);
}

}
