#ifndef incl_BADUK_POINT_H__
#define incl_BADUK_POINT_H__

#include <functional>
#include <iostream>

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

}

namespace std {
    template<>
    struct hash<baduk::Point> {
        std::size_t operator()(const baduk::Point& p) const {
            return std::hash<unsigned int>{}(p.row()) ^
                (std::hash<unsigned int>{}(p.col()) << 1);
        }
    };
}

#endif
