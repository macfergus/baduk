#ifndef incl_BADUK_BOARD_H__
#define incl_BADUK_BOARD_H__

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "neighbor.h"
#include "point.h"
#include "zobrist/codes.h"

namespace baduk {

class GoString {
public:
    GoString(
            Stone c, Point p,
            std::vector<Point> const& liberties,
            std::vector<GoString const*> neighbors);

    GoString(
            GoString const& original,
            Point const& liberty_to_change,
            bool add_liberty);

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
    std::vector<Point> stones_;
    std::vector<Point> liberties_;
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
    std::shared_ptr<const GoString> stringAt(Point point) const;

    std::vector<Point> const& neighbors(Point p) const;

    bool operator==(Board const& b) const;

    zobrist::hashcode hash() const;
    /** Compute what the hash would be, after placing this stone. */
    zobrist::hashcode hashAfter(Point point, Stone stone) const;

private:
    unsigned int num_rows_;
    unsigned int num_cols_;

    NeighborTable const* neighbors_;

    zobrist::hashcode hashcode_;

    std::vector<std::shared_ptr<const GoString>> grid_;

    unsigned int index(Point p) const;
    void replace(std::shared_ptr<const GoString> new_string);
    void remove(GoString const* old_string);

    void validate() const;
};

std::ostream& operator<<(std::ostream&, Board const&);

}

#endif
