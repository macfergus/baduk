#include "gostring.h"

namespace baduk {

GoString::GoString(
        Stone c, Point p,
        PointSet const& liberties,
        std::vector<GoString const*> neighbors) :
    color_(c) {
    stones_.add(p);
    liberties_.add(liberties);
    for (auto neighbor : neighbors) {
        stones_.add(neighbor->stones());
        liberties_.add(neighbor->liberties_);
    }
    liberties_.remove(p);
    for (auto neighbor : neighbors) {
        liberties_.remove(neighbor->stones());
    }
}

GoString::GoString(
        GoString const& original,
        Point const& liberty_to_change,
        bool add_liberty) :
    color_(original.color_),
    stones_(original.stones_),
    liberties_(original.liberties_) {

    if (add_liberty) {
        liberties_.add(liberty_to_change);
    } else {
        liberties_.remove(liberty_to_change);
    }
}

GoString GoString::withoutLiberty(Point p) const {
    return GoString(color_, stones_, liberties_.without(p));
}

GoString GoString::withLiberty(Point p) const {
    return GoString(color_, stones_, liberties_.unionWith(p));
}

GoString GoString::mergedWith(GoString const& other) const {
    auto new_liberties = liberties_.unionWith(other.liberties_);
    new_liberties.remove(stones_);
    new_liberties.remove(other.stones_);
    return GoString(color_, stones_.unionWith(other.stones_), new_liberties);
}

}
