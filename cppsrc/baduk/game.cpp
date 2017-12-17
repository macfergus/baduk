#include <algorithm>
#include <optional>
#include <unordered_set>

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

class GameStateImpl :
    public GameState,
    public std::enable_shared_from_this<GameStateImpl> {
public:
    GameStateImpl(
            Board const& board,
            Stone next_player,
            std::optional<Move> last_move,
            std::shared_ptr<const GameStateImpl> parent) :
        board_(board),
        next_player_(next_player),
        last_move_(last_move),
        prev_state_(parent) {
        if (prev_state_ != nullptr) {
            previous_states_ = parent->previous_states_;
            previous_states_.insert(prev_state_->hash());
        }
    }

    Board board() const override { return board_; }
    Stone nextPlayer() const override { return next_player_; }
    GameState const* prevState() const override { return prev_state_.get(); }

    zobrist::hashcode hash() const {
        const auto player_hash = next_player_ == Stone::black ?
            zobrist::BLACK_TO_PLAY :
            zobrist::WHITE_TO_PLAY;
        return board_.hash() ^ player_hash;
    }

    bool operator==(GameState const& b) const override {
        auto other = dynamic_cast<GameStateImpl const&>(b);
        return board_ == other.board_ && next_player_ == other.next_player_;
    }

    struct ProcessMove {
        std::shared_ptr<const GameStateImpl> game_state;

        ProcessMove(std::shared_ptr<const GameStateImpl> parent) :
            game_state(parent) {}

        std::shared_ptr<GameStateImpl> operator()(Play const& play) {
            const auto player = game_state->nextPlayer();
            Board next_board(game_state->board());
            next_board.place(play.point(), player);

            return std::make_shared<GameStateImpl>(
                next_board, other(player), play, game_state
            );
        }

        std::shared_ptr<GameStateImpl> operator()(Pass const& pass) {
            return std::make_shared<GameStateImpl>(
                game_state->board(),
                other(game_state->nextPlayer()),
                pass,
                game_state
            );
        }

        std::shared_ptr<GameStateImpl> operator()(Resign const& resign) {
            return std::make_shared<GameStateImpl>(
                game_state->board(),
                other(game_state->nextPlayer()),
                resign,
                game_state
            );
        }
    };

    std::shared_ptr<GameState> applyMove(Move const& move) const override {
        return std::visit(ProcessMove(shared_from_this()), move);
    }

    bool isOver() const override {
        if (!last_move_) {
            // First move of game.
            return false;
        }
        const auto last_move = *last_move_;
        if (isResign(last_move)) {
            return true;
        }

        if (isPass(last_move)) {
            if (prev_state_ != nullptr) {
                if (prev_state_->last_move_) {
                    const auto prev_prev_move = *prev_state_->last_move_;
                    if (isPass(prev_prev_move)) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    struct CheckLegal {
        GameStateImpl const* game_state;

        CheckLegal(GameStateImpl const* parent) : game_state(parent) {}

        bool operator()(Play const& play) {
            const auto point = play.point();
            const auto player = game_state->next_player_;
            if (!game_state->board_.isEmpty(point)) {
                return false;
            }

            // Ko.
            if (game_state->board_.willCapture(point, player)) {
                const auto next_state = game_state->applyMove(play);
                const auto prev_ptr = game_state->previous_states_.find(
                    next_state->hash());
                if (prev_ptr != game_state->previous_states_.end()) {
                    return false;
                }
            } else if (game_state->board_.willHaveNoLiberties(point, player)) {
                return false;
            }

            return true;
        }

        bool operator()(Pass const&) {
            // Always legal.
            return true;
        }

        bool operator()(Resign const&) {
            // Always legal.
            return true;
        }
    };

    bool isMoveLegal(Move const& move) const override {
        return std::visit(CheckLegal(this), move);
    }

private:
    Board board_;
    Stone next_player_;
    std::optional<Move> last_move_;
    std::shared_ptr<const GameStateImpl> prev_state_;
    std::unordered_set<zobrist::hashcode> previous_states_;
};


std::shared_ptr<const GameState> newGame(unsigned int board_size) {
    return std::shared_ptr<const GameState>(
        std::make_shared<const GameStateImpl>(
            Board(board_size, board_size),
            Stone::black,
            std::nullopt, /* last move */
            nullptr /* parent */
        )
    );
}

}