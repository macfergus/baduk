#ifndef incl_BADUK_POINTSET_H__
#define incl_BADUK_POINTSET_H__

#include <bitset>
#include <vector>

#include "dim.h"
#include "point.h"

namespace baduk {

const unsigned int MAX_POINTS = MAX_BOARD_SIZE * MAX_BOARD_SIZE;

class PointIter;

class PointSet {
public:
    auto size() const { return points_.count(); }

    // These return a copy
    PointSet unionWith(Point const&) const;
    PointSet unionWith(PointSet const&) const;
    PointSet without(Point const&) const;

    // These mutate in-place
    void add(Point const&);
    void add(PointSet const&);
    void remove(Point const&);
    void remove(PointSet const&);

    std::vector<Point> asPoints() const;

    PointIter begin() const;
    PointIter end() const;

private:
    std::bitset<MAX_POINTS> points_;

    friend class PointIter;
};

class PointIter{
public:
    PointIter(PointSet const& pointset, unsigned int i);

    bool operator!=(PointIter const& p) const;
    PointIter& operator++();
    Point operator*() const;

private:
    PointSet const& pointset_;
    unsigned int i_;
};


}

#endif
