#ifndef incl_BADUK_GAME_H__
#define incl_BADUK_GAME_H__

#include <memory>
#include <variant>

#include "board.h"

namespace baduk {

class Play {
public:
    Play(Point p) : point_(p) {}

    Point point() const { return point_; }

private:
    Point point_;
};

class Pass {};

class Resign {};

using Move = std::variant<Play, Pass, Resign>;
bool isPass(Move const& move);
bool isResign(Move const& move);

class GameState {
public:
    virtual Board board() const = 0;
    virtual Stone nextPlayer() const = 0;
};

class Game {
public:
    virtual void applyMove(Move const& move) = 0;
    virtual bool isPlayLegal(Play const& play) const = 0;
    virtual bool isOver() const = 0;
    virtual GameState const* currentState() const = 0;
};

std::unique_ptr<Game> newGame(unsigned int board_size);

}

#endif
