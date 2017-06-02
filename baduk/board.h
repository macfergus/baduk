#ifndef incl_BADUK_BOARD_H__
#define incl_BADUK_BOARD_H__

#include <memory>
#include <vector>

namespace baduk {

enum class Stone { black, white };

class Point {
public:
    Point(unsigned int r, unsigned int c) :
        row_(r),
        col_(c) {}

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

    Stone color() const { return color_; }
    auto numLiberties() { return liberties_.size(); }
    std::vector<Point> stones() const { return stones_; }

    void merge(GoString const& other);
    void removeLiberty(Point p);

private:
    Stone color_;
    std::vector<Point> stones_;
    std::vector<Point> liberties_;
};

class Board {
public:
    Board(unsigned int num_rows, unsigned int num_cols);

    unsigned int numRows() const { return num_rows_; }
    unsigned int numCols() const { return num_cols_; }

    void place(Point point, Stone stone);
    bool isEmpty(Point point) const;
    Stone at(Point point) const;

private:
    unsigned int num_rows_;
    unsigned int num_cols_;

    std::vector<std::shared_ptr<GoString>> grid_;

    std::vector<Point> neighbors(Point p) const;
    unsigned int index(Point p) const;
    void replace(std::shared_ptr<GoString> const& new_string);
    void remove(GoString const* old_string);
};

}

#endif
