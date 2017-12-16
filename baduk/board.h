#ifndef incl_BADUK_BOARD_H__
#define incl_BADUK_BOARD_H__

#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace baduk {

enum class Stone { black, white };

Stone other(Stone stone);
std::ostream& operator<<(std::ostream& out, Stone s);

class Point {
public:
    Point(unsigned int r, unsigned int c) :
        row_(r),
        col_(c) {}
    /** Construct from D4 style coordinates. */
    Point(std::string const& name);
    Point(char const* name) : Point(std::string(name)) {}

    /** Format as D4 style coordinates. */
    std::string name() const;

    unsigned int row() const { return row_; }
    unsigned int col() const { return col_; }

    bool operator==(Point q) const {
        return row_ == q.row() && col_ == q.col();
    }

private:
    unsigned int row_;
    unsigned int col_;
};

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
    Board(unsigned int num_rows, unsigned int num_cols);
    Board(Board const& b);

    Board& operator=(Board const &b);

    unsigned int numRows() const { return num_rows_; }
    unsigned int numCols() const { return num_cols_; }

    void place(Point point, Stone stone);
    bool isEmpty(Point point) const;
    Stone at(Point point) const;
    GoString stringAt(Point point) const;

    std::vector<Point> neighbors(Point p) const;

    bool operator==(Board const& b) const;

public:
    unsigned int num_rows_;
    unsigned int num_cols_;

    std::vector<std::shared_ptr<GoString>> grid_;

    unsigned int index(Point p) const;
    void replace(GoString const& new_string);
    void remove(GoString const* old_string);

    void validate() const;
};

std::ostream& operator<<(std::ostream&, Board const&);

}

#endif
