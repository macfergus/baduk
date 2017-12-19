#ifndef incl_BADUK_BOARD_H__
#define incl_BADUK_BOARD_H__

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "point.h"
#include "zobrist/codes.h"

namespace baduk {

class GoString {
public:
    GoString(Stone c, Point p, std::vector<Point> const& liberties) :
        color_(c),
        stones_(1, p),
        liberties_(liberties) {}

    GoString(
        Stone c,
        std::vector<Point> const& points,
        std::vector<Point> const& liberties) :
        color_(c),
        stones_(points),
        liberties_(liberties) {}

    Stone color() const { return color_; }
    auto numLiberties() const { return liberties_.size(); }
    std::vector<Point> stones() const { return stones_; }
    std::vector<Point> liberties() const { return liberties_; }

    GoString mergedWith(GoString const& other) const;
    GoString withLiberty(Point p) const;
    GoString withoutLiberty(Point p) const;

public:
    const Stone color_;
    const std::vector<Point> stones_;
    const std::vector<Point> liberties_;
};

class Board {
public:
    Board();
    Board(unsigned int num_rows, unsigned int num_cols);
    Board(Board const& b);

    Board& operator=(Board const &b);

    unsigned int numRows() const { return num_rows_; }
    unsigned int numCols() const { return num_cols_; }

    void place(Point point, Stone stone);
    bool willCapture(Point point, Stone stone) const;
    bool willHaveNoLiberties(Point point, Stone stone) const;
    bool isEmpty(Point point) const;
    Stone at(Point point) const;
    std::shared_ptr<GoString> stringAt(Point point) const;

    std::vector<Point> neighbors(Point p) const;

    bool operator==(Board const& b) const;

    zobrist::hashcode hash() const;

private:
    unsigned int num_rows_;
    unsigned int num_cols_;

    zobrist::hashcode hashcode_;

    std::vector<std::shared_ptr<GoString>> grid_;

    unsigned int index(Point p) const;
    void replace(GoString const& new_string);
    void remove(GoString const* old_string);

    void validate() const;
};

std::ostream& operator<<(std::ostream&, Board const&);

}

#endif
