#include <algorithm>

#include "game.h"

namespace baduk {

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
    GameImpl(unsigned int board_size) {
        states_.push_back(std::make_unique<GameStateImpl>(
            Board(board_size, board_size),
            Stone::black
        ));
    }

    void applyMove(Move const& move) override {
        states_.push_back(std::visit(ProcessMove(this), move));
        moves_.push_back(move);
    }

    GameState const* currentState() const override {
        return states_.back().get();
    }

    bool isPlayLegal(Play const& play) override {
        const auto state = currentState();
        const auto point = play.point();
        return state->board().isEmpty(point) &&
            !isSelfCapture(play) &&
            !doesViolateKo(play);
    }

private:
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

    bool isSelfCapture(Play const& play) {
        Board next_board(currentState()->board());
        next_board.place(play.point(), currentState()->nextPlayer());
        const auto string = next_board.stringAt(play.point());
        return string.numLiberties() == 0;
    }

    bool doesViolateKo(Play const& play) {
        Board next_board(currentState()->board());
        next_board.place(play.point(), currentState()->nextPlayer());
        Stone next_player = other(currentState()->nextPlayer());
        const GameStateImpl next_state(next_board, next_player);
        return std::find_if(
            states_.begin(), states_.end(),
            [&next_state](auto const& s) {
                return *s == next_state;
            }
        ) != states_.end();
    }
};

std::unique_ptr<Game> newGame(unsigned int board_size) {
    return std::unique_ptr<Game>(
        std::make_unique<GameImpl>(board_size)
    );
}

}
