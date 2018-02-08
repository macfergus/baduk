#include <algorithm>
#include <cassert>
#include <iostream>

#include "board.h"

namespace baduk {

// I is excluded on purpose
const std::string COLS = "ABCDEFGHJKLMNOPQRSTUVWXYZ";

Board::Board() :
        zobrist_(zobrist::ZobristCodes::get()),
        num_rows_(19),
        num_cols_(19),
        neighbors_(getNeighborTable(19, 19)),
        hashcode_(zobrist_.emptyBoard()),
        grid_(19 * 19, nullptr) {
}

Board::Board(unsigned int num_rows, unsigned int num_cols) :
        zobrist_(zobrist::ZobristCodes::get()),
        num_rows_(num_rows),
        num_cols_(num_cols),
        neighbors_(getNeighborTable(num_rows, num_cols)),
        hashcode_(zobrist_.emptyBoard()),
        grid_(num_rows * num_cols, nullptr) {
}

Board::Board(Board const& b) :
        zobrist_(zobrist::ZobristCodes::get()),
        num_rows_(b.num_rows_),
        num_cols_(b.num_cols_),
        neighbors_(b.neighbors_),
        hashcode_(b.hashcode_),
        grid_(b.grid_) {
}

bool Board::isEmpty(Point p) const {
    return grid_[index(p)] == nullptr;
}

template<typename Container, typename T>
bool contains(Container container, T elem) {
    return std::find(container.begin(), container.end(), elem) !=
        container.end();
}

void Board::place(Point point, Stone player) {
    assert(isEmpty(point));
    std::vector<GoString const*> adjacent_same_color;
    std::vector<GoString const*> adjacent_other_color;
    PointSet liberties;

    for (Point neighbor : neighbors_->get(point)) {
        auto neighbor_string = grid_[index(neighbor)].get();
        if (neighbor_string == nullptr) {
            liberties.add(neighbor);
        } else if (neighbor_string->color() == player) {
            if (!contains(adjacent_same_color, neighbor_string)) {
                adjacent_same_color.push_back(neighbor_string);
            }
        } else {
            if (!contains(adjacent_other_color, neighbor_string)) {
                adjacent_other_color.push_back(neighbor_string);
            }
        }
    }
    auto new_string = std::make_shared<const GoString>(
        player, point, liberties, adjacent_same_color);
    replace(new_string);

    // Update hash code.
    hashcode_ ^= zobrist_.getEmpty(point);
    hashcode_ ^= zobrist_.getStone(player, point);

    for (auto const& other_color_string : adjacent_other_color) {
        auto replacement_string = std::make_shared<GoString>(
            *other_color_string, point, false /* add liberties */);
        if (replacement_string->numLiberties() == 0) {
            remove(other_color_string);
        } else {
            replace(replacement_string);
        }
    }
}

bool Board::willCapture(Point point, Stone player) const {
    for (Point neighbor : neighbors_->get(point)) {
        const auto neighbor_string = grid_[index(neighbor)].get();
        if (neighbor_string == nullptr) {
            continue;
        }
        if (neighbor_string->color() == player) {
            continue;
        }
        if (neighbor_string->numLiberties() == 1) {
            return true;
        }
    }
    return false;
}

bool Board::willHaveNoLiberties(Point point, Stone player) const {
    // Does NOT check if it is a capture! Call willCapture first.
    for (Point neighbor : neighbors_->get(point)) {
        const auto neighbor_string = grid_[index(neighbor)].get();
        if (neighbor_string == nullptr) {
            // This point will be a liberty.
            return false;
        } else if (neighbor_string->color() == player) {
            if (neighbor_string->numLiberties() > 1) {
                // This string will still have a liberty after placing the
                // stone.
                return false;
            }
        }
    }
    return true;
}

void Board::replace(std::shared_ptr<const GoString> new_string) {
    for (auto point : new_string->stones()) {
        grid_[index(point)] = new_string;
    }
}

Stone Board::at(Point p) const {
    return grid_[index(p)]->color();
}

std::shared_ptr<const GoString> Board::stringAt(Point p) const {
    return grid_[index(p)];
}

std::vector<Point> const& Board::neighbors(Point p) const {
    return neighbors_->get(p);
}

unsigned int Board::index(Point p) const {
    return num_cols_ * p.row() + p.col();
}

void Board::remove(GoString const* old_string) {
    for (auto const& point : old_string->stones()) {
        std::vector<GoString const*> strings_to_update;
        for (Point neighbor : neighbors_->get(point)) {
            auto neighbor_string = grid_[index(neighbor)].get();
            if (neighbor_string == nullptr) {
                continue;
            }
            if (contains(strings_to_update, neighbor_string)) {
                continue;
            }
            if (neighbor_string != old_string) {
                strings_to_update.push_back(neighbor_string);
            }
        }
        for (auto string_to_update : strings_to_update) {
            replace(std::make_shared<GoString>(
                *string_to_update, point, true /* add liberty */
            ));
        }

        hashcode_ ^= zobrist_.getStone(old_string->color(), point);
        hashcode_ ^= zobrist_.getEmpty(point);

        grid_[index(point)] = nullptr;
    }
}

void Board::validate() const {
    for (const auto& go_string : grid_) {
        if (go_string.get() == nullptr) {
            continue;
        }
        for (const auto point : go_string->stones()) {
            assert(!isEmpty(point));
            assert(at(point) == go_string->color());
        }
        for (const auto point : go_string->liberties()) {
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
        const auto neighbor_string = grid_[index(neighbor)].get();
        if (neighbor_string == nullptr) {
            continue;
        }
        if (neighbor_string->color() == stone) {
            continue;
        }
        if (neighbor_string->numLiberties() == 1) {
            for (const auto& p : neighbor_string->stones()) {
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

GoString::GoString(
        Stone c, Point p,
        PointSet const& liberties,
        std::vector<GoString const*> neighbors) :
    color_(c) {
    stones_.add(p);
    liberties_.add(liberties);
    for (auto neighbor : neighbors) {
        stones_.add(neighbor->stones());
        liberties_.add(neighbor->liberties_);
    }
    liberties_.remove(p);
    for (auto neighbor : neighbors) {
        liberties_.remove(neighbor->stones());
    }
}

int gindex(Point const& p) {
    return 25 * p.row() + p.col();
}

GoString::GoString(
        GoString const& original,
        Point const& liberty_to_change,
        bool add_liberty) :
    color_(original.color_),
    stones_(original.stones_),
    liberties_(original.liberties_) {

    if (add_liberty) {
        liberties_.add(liberty_to_change);
    } else {
        liberties_.remove(liberty_to_change);
    }
}

GoString GoString::withoutLiberty(Point p) const {
    return GoString(color_, stones_, liberties_.without(p));
}

GoString GoString::withLiberty(Point p) const {
    return GoString(color_, stones_, liberties_.unionWith(p));
}

GoString GoString::mergedWith(GoString const& other) const {
    auto new_liberties = liberties_.unionWith(other.liberties_);
    new_liberties.remove(stones_);
    new_liberties.remove(other.stones_);
    return GoString(color_, stones_.unionWith(other.stones_), new_liberties);
}

}
