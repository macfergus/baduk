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
    while (i_ < MAX_POINTS && i_ <= pointset_.max_ && !pointset_.points_[i_]) {
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
    const auto pindex = index(p);
    points_.set(pindex);
    if (pindex < min_) {
        min_ = pindex;
    }
    if (pindex > max_) {
        max_ = pindex;
    }
}

void PointSet::add(PointSet const& ps) {
    points_ |= ps.points_;
    min_ = std::min(min_, ps.min_);
    max_ = std::max(max_, ps.max_);
}

void PointSet::remove(Point const& p) {
    points_.reset(index(p));
}

void PointSet::remove(PointSet const& ps) {
    points_ &= (~ps.points_);
}

void PointSet::clear() {
    points_.reset();
    min_ = MAX_POINTS - 1;
    max_ = 0;
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
    auto tmp = PointIter(*this, min_ - 1);
    ++tmp;
    return tmp;
}

PointIter PointSet::end() const {
    return PointIter(*this, max_ + 1);
}

}
