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
    zobrist::ZobristCodes const& zobrist_;

    unsigned int num_rows_;
    unsigned int num_cols_;

    NeighborTable const* neighbors_;

    zobrist::hashcode hashcode_;

    std::array<std::shared_ptr<const GoString>, MAX_POINTS> grid_;

    unsigned int index(Point p) const;
    void replace(std::shared_ptr<const GoString> new_string);
    void remove(GoString const* old_string);

    void validate() const;
};

std::ostream& operator<<(std::ostream&, Board const&);

}

#endif
