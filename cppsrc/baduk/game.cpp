#include <algorithm>
#include <optional>
#include <unordered_set>

#include "game.h"
#include "statichash.h"

namespace baduk {

const unsigned int HASH_SIZE = 1039;

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

Point getPoint(Move const& move) {
    return std::get<Play>(move).point();
}

class GameStateImpl :
    public GameState,
    public std::enable_shared_from_this<GameStateImpl> {
public:
    GameStateImpl(Board const& board, Stone next_player) :
        board_(board),
        next_player_(next_player) {}

    GameStateImpl(
            Stone next_player,
            std::shared_ptr<const GameStateImpl> parent,
            Move const& last_move) :
        board_(parent->board_),
        next_player_(next_player),
        last_move_(last_move),
        prev_state_(parent) {

        if (std::holds_alternative<Play>(last_move)) {
            board_.place(
                std::get<Play>(last_move).point(),
                other(next_player_));
        }

        previous_states_ = parent->previous_states_;
        previous_states_.insert(prev_state_->hash());
    }

    Board const& board() const override { return board_; }
    Stone nextPlayer() const override { return next_player_; }
    GameState const* prevState() const override { return prev_state_.get(); }
    Move lastMove() const override { return last_move_.value(); }

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

    std::shared_ptr<GameState> applyMove(Move const& move) const override {
        return std::make_shared<GameStateImpl>(
            other(next_player_),
            shared_from_this(),
            move
        );
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
                if (game_state->willViolateKo(point)) {
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

    struct CheckViolatesKo {
        GameStateImpl const* game_state;

        CheckViolatesKo(GameStateImpl const* parent) : game_state(parent) {}

        bool operator()(Play const& play) {
            const auto point = play.point();
            const auto player = game_state->next_player_;
            if (!game_state->board_.isEmpty(point)) {
                return false;
            }

            if (game_state->board_.willCapture(point, player)) {
                return game_state->willViolateKo(point);
            }
            return false;
        }

        bool operator()(Pass const&) {
            return false;
        }

        bool operator()(Resign const&) {
            return false;
        }
    };

    bool doesMoveViolateKo(Move const& move) const override {
        return std::visit(CheckViolatesKo(this), move);
    }

private:
    Board board_;
    Stone next_player_;
    std::optional<Move> last_move_;
    std::shared_ptr<const GameStateImpl> prev_state_;
    StaticHash<zobrist::hashcode, HASH_SIZE> previous_states_;

    bool willViolateKo(Point point) const {
        // Find the hash of the next state, without actually computing
        // the full board position.
        const auto next_board_hash = board_.hashAfter(point, next_player_);
        const auto next_player_hash =
            next_player_ == Stone::black ?
                zobrist::WHITE_TO_PLAY :
                zobrist::BLACK_TO_PLAY;
        const auto next_state_hash = next_board_hash ^ next_player_hash;

        return previous_states_.contains(next_state_hash);
    }
};


std::shared_ptr<const GameState> newGame(unsigned int board_size) {
    return std::shared_ptr<const GameState>(
        std::make_shared<const GameStateImpl>(
            Board(board_size, board_size),
            Stone::black
        )
    );
}

std::shared_ptr<const GameState> gameFromBoard(Board board, Stone next_player) {
    return std::shared_ptr<const GameState>(
        std::make_shared<const GameStateImpl>(
            board, next_player
        )
    );
}

}
