#ifndef incl_BADUK_GAME_H__
#define incl_BADUK_GAME_H__

#include <memory>
#include <variant>

#include "board.h"
#include "zobrist/zobrist.h"

namespace baduk {

class Play {
public:
    Play() : point_(0, 0) {}
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
// Throws an exception if the Move is not a Play
Point getPoint(Move const& move);

class GameState {
public:
    virtual ~GameState() {}

    virtual Board const& board() const = 0;
    virtual Stone nextPlayer() const = 0;
    virtual GameState const* prevState() const = 0;
    virtual std::shared_ptr<GameState> applyMove(Move const& move) const = 0;
    virtual bool isMoveLegal(Move const& move) const = 0;
    virtual bool doesMoveViolateKo(Move const& move) const = 0;
    virtual bool isOver() const = 0;
    virtual bool hasLastMove() const = 0;
    virtual Move lastMove() const = 0;
    virtual float komi() const = 0;

    virtual bool operator==(GameState const&) const = 0;

    virtual zobrist::hashcode hash() const = 0;
};

std::shared_ptr<const GameState> newGame(unsigned int board_size, float komi);
std::shared_ptr<const GameState> gameFromBoard(
    Board board, Stone next_player,
    float komi);

}

#endif
