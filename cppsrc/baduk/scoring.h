#ifndef incl_BADUK_SCORING_H__
#define incl_BADUK_SCORING_H__

#include <map>

#include "board.h"
#include "game.h"

namespace baduk {

enum class PointStatus { black, white, neutral };

class TerritoryMap {
public:
    PointStatus at(Point) const;
    void set(Point, PointStatus);

private:
    std::map<Point, PointStatus> map_;
};

TerritoryMap evaluateTerritory(Board const&);

Board removeDeadStones(std::shared_ptr<const GameState> game);

}

#endif
