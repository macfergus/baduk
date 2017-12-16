#include <algorithm>
#include <cassert>
#include <iostream>

#include "board.h"

namespace baduk {

Stone other(Stone stone) {
    if (stone == Stone::black) {
        return Stone::white;
    }
    return Stone::black;
}

std::ostream& operator<<(std::ostream& out, Stone stone) {
    if (stone == Stone::black) {
        return out << "b";
    }
    return out << "w";
}

// I is excluded on purpose
const std::string COLS = "ABCDEFGHJKLMNOPQRSTUVWXYZ";

Point::Point(std::string const& name) {
    const auto col_s = name.substr(0, 1);
    const auto row_s = name.substr(1);
    col_ = COLS.find(col_s);
    row_ = std::stoi(row_s) - 1;
}

std::string Point::name() const {
    return COLS.substr(col_, 1) + std::to_string(row_ + 1);
}

Board::Board(unsigned int num_rows, unsigned int num_cols) :
        num_rows_(num_rows),
        num_cols_(num_cols),
        grid_(num_rows * num_cols, nullptr) {
}

Board::Board(Board const& b) :
        num_rows_(b.num_rows_),
        num_cols_(b.num_cols_),
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
    std::vector<GoString*> adjacent_same_color;
    std::vector<GoString*> adjacent_other_color;
    std::vector<Point> liberties;
    for (Point neighbor: neighbors(point)) {
        GoString* neighbor_string = grid_[index(neighbor)].get();
        if (neighbor_string == nullptr) {
            liberties.push_back(neighbor);
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
    auto new_string = std::make_unique<GoString>(player, point, liberties);
    for (auto const& same_color_string : adjacent_same_color) {
        new_string = std::make_unique<GoString>(
            new_string->mergedWith(*same_color_string));
    }
    replace(*new_string);
    for (auto const& other_color_string : adjacent_other_color) {
        auto replacement_string = other_color_string->withoutLiberty(point);
        if (replacement_string.numLiberties() == 0) {
            remove(other_color_string);
        } else {
            replace(replacement_string);
        }
    }
}

void Board::replace(GoString const& new_string) {
    auto p_new_string = std::make_shared<GoString>(new_string);
    for (auto const& point : p_new_string->stones()) {
        grid_[index(point)] = p_new_string;
    }
}

Stone Board::at(Point p) const {
    return grid_[index(p)]->color();
}

GoString Board::stringAt(Point p) const {
    return *grid_[index(p)];
}

std::vector<Point> Board::neighbors(Point p) const {
    std::vector<Point> rv;
    if (p.row() > 0) {
        rv.push_back(Point(p.row() - 1, p.col()));
    }
    if (p.row() < num_rows_ - 1) {
        rv.push_back(Point(p.row() + 1, p.col()));
    }
    if (p.col() > 0) {
        rv.push_back(Point(p.row(), p.col() - 1));
    }
    if (p.col() < num_cols_ - 1) {
        rv.push_back(Point(p.row(), p.col() + 1));
    }
    return rv;
}

unsigned int Board::index(Point p) const {
    return num_cols_ * p.row() + p.col();
}

void Board::remove(GoString const* old_string) {
    for (auto const& point : old_string->stones()) {
        std::vector<GoString const*> strings_to_update;
        for (Point neighbor : neighbors(point)) {
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
            replace(string_to_update->withLiberty(point));
        }
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

GoString GoString::withoutLiberty(Point p) const {
    std::vector<Point> new_liberties;
    for (const auto old_point : liberties_) {
        if (!(old_point == p)) {
            new_liberties.push_back(old_point);
        }
    }
    return GoString(color_, stones_, new_liberties);
}

GoString GoString::withLiberty(Point p) const {
    std::vector<Point> new_liberties(liberties_);
    new_liberties.push_back(p);
    return GoString(color_, stones_, new_liberties);
}

GoString GoString::mergedWith(GoString const& other) const {
    std::vector<Point> new_stones(stones_);
    std::vector<Point> new_liberties;
    for (auto const& new_point : other.stones()) {
        if (!contains(new_stones, new_point)) {
            new_stones.push_back(new_point);
        }
    }
    for (auto const& old_liberty : liberties_) {
        if (!contains(new_stones, old_liberty)) {
            new_liberties.push_back(old_liberty);
        }
    }
    for (auto const& new_liberty : other.liberties_) {
        if (!contains(new_stones, new_liberty) &&
                !contains(new_liberties, new_liberty)) {
            new_liberties.push_back(new_liberty);
        }
    }
    return GoString(color_, new_stones, new_liberties);
}

}
