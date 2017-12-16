#include <algorithm>

#include "game.h"

namespace baduk {

struct IsResignImpl {
    bool operator()(Play const&) { return false; }
    bool operator()(Pass const&) { return false; }
    bool operator()(Resign const&) { return true;}
};

struct IsPassImpl {
    bool operator()(Play const&) { return false; }
    bool operator()(Pass const&) { return true; }
    bool operator()(Resign const&) { return false;}
};

bool isPass(Move const& move) {
    return std::visit(IsPassImpl(), move);
}

bool isResign(Move const& move) {
    return std::visit(IsResignImpl(), move);
}

class GameStateImpl : public GameState {
public:
    GameStateImpl(Board const& board, Stone next_player) :
        board_(board),
        next_player_(next_player) {}

    Board board() const override { return board_; }
    Stone nextPlayer() const override { return next_player_; }

    bool operator==(GameStateImpl const& b) const {
        return board_ == b.board_ && next_player_ == b.next_player_;
    }

private:
    Board board_;
    Stone next_player_;
};

class GameImpl : public Game {
public:
    GameImpl(unsigned int board_size) :
        is_over_(false) {
        states_.push_back(std::make_unique<GameStateImpl>(
            Board(board_size, board_size),
            Stone::black
        ));
    }

    void applyMove(Move const& move) override {
        states_.push_back(std::visit(ProcessMove(this), move));
        moves_.push_back(move);

        if (isResign(move)) {
            is_over_ = true;
        }

        if (isPass(move) && moves_.size() >= 2) {
            const auto second_last_move = moves_[moves_.size() - 2];
            if (isPass(second_last_move)) {
                is_over_ = true;
            }
        }
    }

    GameState const* currentState() const override {
        return states_.back().get();
    }

    bool isPlayLegal(Play const& play) const override {
        const auto state = currentState();
        const auto point = play.point();
        if (!state->board().isEmpty(point)) {
            return false;
        }
        Board next_board(state->board());
        next_board.place(play.point(), state->nextPlayer());

        // Self-capture.
        const auto string = next_board.stringAt(play.point());
        if (string.numLiberties() == 0) {
            return false;
        }

        // Ko.
        Stone next_player = other(currentState()->nextPlayer());
        const GameStateImpl next_state(next_board, next_player);
        const bool violates_ko = std::find_if(
            states_.begin(), states_.end(),
            [&next_state](auto const& s) {
                return *s == next_state;
            }
        ) != states_.end();

        return !violates_ko;
    }

    bool isOver() const override {
        return is_over_;
    }

private:
    bool is_over_;
    std::vector<std::unique_ptr<GameStateImpl>> states_;
    std::vector<Move> moves_;

    struct ProcessMove {
        GameImpl const* game;

        ProcessMove(GameImpl const* parent) : game(parent) {}

        std::unique_ptr<GameStateImpl> operator()(Play const& play) {
            const auto state = game->currentState();
            const auto player = state->nextPlayer();
            Board next_board(state->board());
            next_board.place(play.point(), player);

            return std::make_unique<GameStateImpl>(
                next_board, other(player)
            );
        }

        std::unique_ptr<GameStateImpl> noMove() {
            return std::make_unique<GameStateImpl>(
                game->currentState()->board(),
                other(game->currentState()->nextPlayer())
            );
        }

        std::unique_ptr<GameStateImpl> operator()(Pass const&) {
            return noMove();
        }

        std::unique_ptr<GameStateImpl> operator()(Resign const&) {
            return noMove();
        }
    };
};

std::unique_ptr<Game> newGame(unsigned int board_size) {
    return std::unique_ptr<Game>(
        std::make_unique<GameImpl>(board_size)
    );
}

}
