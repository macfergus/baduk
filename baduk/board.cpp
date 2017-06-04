#include <algorithm>
#include <cassert>

#include "board.h"

namespace baduk {

Stone other(Stone stone) {
    if (stone == Stone::black) {
        return Stone::white;
    }
    return Stone::black;
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
        grid_(num_rows_ * num_cols_, nullptr) {
    auto p = grid_.begin();
    for (auto const& p_string : b.grid_) {
        if (p->get() == nullptr && p_string.get() != nullptr) {
            auto new_string = std::make_shared<GoString>(*p_string);
            replace(new_string);
        }
        ++p;
    }
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
    auto new_string = std::make_shared<GoString>(player, point, liberties);
    for (auto const& same_color_string : adjacent_same_color) {
        new_string->merge(*same_color_string);
    }
    replace(new_string);
    for (auto const& other_color_string : adjacent_other_color) {
        other_color_string->removeLiberty(point);
        if (other_color_string->numLiberties() == 0) {
            remove(other_color_string);
        }
    }
}

void Board::replace(std::shared_ptr<GoString> const& new_string) {
    for (auto const& point : new_string->stones()) {
        grid_[index(point)] = new_string;
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
        for (Point neighbor : neighbors(point)) {
            auto neighbor_string = grid_[index(neighbor)].get();
            if (neighbor_string == nullptr) {
                continue;
            }
            if (neighbor_string != old_string) {
                neighbor_string->addLiberty(point);
            }
        }
        grid_[index(point)] = nullptr;
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
    return out;
}

void GoString::removeLiberty(Point p) {
    liberties_.erase(std::find(liberties_.begin(), liberties_.end(), p));
}

void GoString::addLiberty(Point p) {
    assert(!contains(liberties_, p));
    liberties_.push_back(p);
}

void GoString::merge(GoString const& other) {
    for (auto const& new_point : other.stones()) {
        if (!contains(stones_, new_point)) {
            stones_.push_back(new_point);
        }
        if (contains(liberties_, new_point)) {
            removeLiberty(new_point);
        }
    }
    for (auto const& new_liberty : other.liberties_) {
        if (!contains(stones_, new_liberty)) {
            liberties_.push_back(new_liberty);
        }
    }
}

}
