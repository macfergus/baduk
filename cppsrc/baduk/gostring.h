#ifndef incl_BADUK_GOSTRING_H__
#define incl_BADUK_GOSTRING_H__

#include "point.h"
#include "pointset.h"

namespace baduk {

class GoString {
public:
    GoString(
            Stone c, Point p,
            PointSet const& liberties,
            std::vector<GoString const*> neighbors);

    GoString(
            GoString const& original,
            Point const& liberty_to_change,
            bool add_liberty);

    GoString(
        Stone c,
        PointSet const& points,
        PointSet const& liberties) :
        color_(c),
        stones_(points),
        liberties_(liberties) {}

    Stone color() const { return color_; }
    auto numLiberties() const { return liberties_.size(); }
    PointSet stones() const { return stones_; }
    PointSet liberties() const { return liberties_; }

    GoString mergedWith(GoString const& other) const;
    GoString withLiberty(Point p) const;
    GoString withoutLiberty(Point p) const;

public:
    const Stone color_;
    PointSet stones_;
    PointSet liberties_;
};

}

#endif
