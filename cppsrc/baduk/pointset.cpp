#include "pointset.h"

namespace baduk {

unsigned int index(Point const& p) {
    return p.col() + MAX_BOARD_SIZE * p.row();
}

Point point(unsigned int index) {
    return Point(index / MAX_BOARD_SIZE, index % MAX_BOARD_SIZE);
}

PointIter::PointIter(PointSet const& pointset, unsigned int i) :
    pointset_(pointset),
    i_(i) {}

PointIter& PointIter::operator++() {
    ++i_;
    while (i_ < MAX_POINTS && !pointset_.points_.test(i_)) {
        ++i_;
    }
    return *this;
}

Point PointIter::operator*() const {
    return point(i_);
}

bool PointIter::operator!=(PointIter const& p) const {
    return i_ != p.i_;
}

void PointSet::add(Point const& p) {
    points_.set(index(p));
}

void PointSet::add(PointSet const& ps) {
    points_ |= ps.points_;
}

void PointSet::remove(Point const& p) {
    points_.reset(index(p));
}

void PointSet::remove(PointSet const& ps) {
    points_ &= (~ps.points_);
}

PointSet PointSet::without(Point const& p) const {
    PointSet rv(*this);
    rv.remove(p);
    return rv;
}

PointSet PointSet::unionWith(Point const& p) const {
    PointSet rv(*this);
    rv.add(p);
    return rv;
}

PointSet PointSet::unionWith(PointSet const& ps) const {
    PointSet rv(*this);
    rv.add(ps);
    return rv;
}

PointIter PointSet::begin() const {
    auto tmp = PointIter(*this, -1);
    ++tmp;
    return tmp;
}

PointIter PointSet::end() const {
    return PointIter(*this, MAX_POINTS);
}

}
