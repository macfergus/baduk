#include "board.h"

namespace baduk {

Board::Board(unsigned int num_rows, unsigned int num_cols) :
        num_rows_(num_rows),
        num_cols_(num_cols),
        grid_(num_rows * num_cols, nullptr) {
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
        grid_[index(point)] = nullptr;
    }
}

void GoString::removeLiberty(Point p) {
    liberties_.erase(std::find(liberties_.begin(), liberties_.end(), p));
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
