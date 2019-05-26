#ifndef incl_BADUK_BOARD_H__
#define incl_BADUK_BOARD_H__

#include <array>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "gostring.h"
#include "neighbor.h"
#include "point.h"
#include "zobrist/zobrist.h"

namespace baduk {

using StringIdx = int;
const StringIdx EMPTY = -1;

class StringIter;

class Board {
public:
    Board();
    Board(unsigned int num_rows, unsigned int num_cols);

    unsigned int numRows() const { return num_rows_; }
    unsigned int numCols() const { return num_cols_; }

    void place(Point point, Stone stone);
    bool willCapture(Point point, Stone stone) const;
    bool willHaveNoLiberties(Point point, Stone stone) const;
    bool isEmpty(Point point) const;
    Stone at(Point point) const;
    GoString stringAt(Point point) const;

    std::vector<Point> const& neighbors(Point p) const;

    bool operator==(Board const& b) const;

    zobrist::hashcode hash() const;
    /** Compute what the hash would be, after placing this stone. */
    zobrist::hashcode hashAfter(Point point, Stone stone) const;

    /** Iterate over all the strings on the board. */
    StringIter stringsBegin() const;
    StringIter stringsEnd() const;

private:
    zobrist::ZobristCodes const& zobrist_;

    unsigned int num_rows_;
    unsigned int num_cols_;

    NeighborTable const* neighbors_;

    zobrist::hashcode hashcode_;

    std::array<StringIdx, MAX_POINTS> grid_;
    std::array<GoString, MAX_STRINGS> strings_;
    std::bitset<MAX_STRINGS> used_;

    unsigned int index(Point p) const;
    void replace(StringIdx new_string);
    void remove(StringIdx old_string);

    StringIdx getUnusedString();
    void recycle(StringIdx);

    void validate() const;

    friend class StringIter;
};

std::ostream& operator<<(std::ostream&, Board const&);

class StringIter {
public:
    StringIter() : board_(nullptr), i_(0) {}
    StringIter(Board const* board, unsigned int i);

    GoString operator*() const;
    GoString const* operator->() const;
    StringIter& operator++();
    bool operator!=(StringIter const&) const;

private:
    Board const* board_;
    unsigned int i_;
};

}

#endif
