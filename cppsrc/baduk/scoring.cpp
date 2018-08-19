#include <queue>
#include <set>
#include "agent.h"
#include "counter.h"
#include "scoring.h"

namespace baduk {

const double DEAD_THRESHOLD = 0.75;

PointStatus TerritoryMap::at(Point p) const {
    // TODO Should bounds-check p;
    const auto ptr = map_.find(p);
    if (ptr != map_.end()) {
        return ptr->second;
    }
    return PointStatus::neutral;
}

void TerritoryMap::set(Point p, PointStatus s) {
    map_[p] = s;
}

struct Boundary {
    Boundary() : includes_black(false), includes_white(false) {}
    Boundary(bool b, bool w) : includes_black(b), includes_white(w) {}

    bool includes_black;
    bool includes_white;

    bool isOnlyBlack() const {
        return includes_black && !includes_white;
    }

    bool isOnlyWhite() const {
        return includes_white && !includes_black;
    }
};

Boundary collectBoundary(Board board, Point origin) {
    Boundary b;
    std::queue<Point> q;
    std::set<Point> visited;

    const auto check_and_enqueue = [&q, &visited](Point new_point) {
        if (visited.find(new_point) == visited.end()) {
            q.push(new_point);
        }
    };

    q.push(origin);
    while (!q.empty()) {
        const auto p = q.front();
        q.pop();
        if (visited.find(p) != visited.end()) {
            continue;
        }
        if (board.isEmpty(p)) {
            // Enqueue its neighbors.
            if (p.row() > 0) {
                check_and_enqueue(Point(p.row() - 1, p.col()));
            }
            if (p.row() < board.numRows() - 1) {
                check_and_enqueue(Point(p.row() + 1, p.col()));
            }
            if (p.col() > 0) {
                check_and_enqueue(Point(p.row(), p.col() - 1));
            }
            if (p.col() < board.numCols() - 1) {
                check_and_enqueue(Point(p.row(), p.col() + 1));
            }
        } else {
            const auto stone = board.at(p);
            if (stone == Stone::black) {
                b.includes_black = true;
            } else {
                // Not empty, not black, must be white
                b.includes_white = true;
            }
        }
        visited.insert(p);
    }

    return b;
}

TerritoryMap evaluateTerritory(Board const& board) {
    TerritoryMap tmap;

    for (unsigned int r = 0; r < board.numRows(); ++r) {
        for (unsigned int c = 0; c < board.numCols(); ++c) {
            const Point p(r, c);
            if (board.isEmpty(p)) {
                // If it is empty, figure out the surrounding stones. If
                // they are exclusively one color, it's territory. Otherwise
                // it's dame.
                const auto boundary = collectBoundary(board, p);
                if (boundary.isOnlyBlack()) {
                    tmap.set(p, PointStatus::black);
                } else if (boundary.isOnlyWhite()) {
                    tmap.set(p, PointStatus::white);
                } else {
                    tmap.set(p, PointStatus::neutral);
                }
            } else {
                // If it contains a stone, count it for that color (chinese
                // style)
                const auto stone = board.at(p);
                if (stone == Stone::black) {
                    tmap.set(p, PointStatus::black);
                } else {
                    // not empty, not black, must be white
                    tmap.set(p, PointStatus::white);
                }
            }
        }
    }

    return tmap;
}

std::shared_ptr<const GameState> completeGame(
        std::shared_ptr<const GameState> start) {
    RandomBot bot;

    auto game = start;
    while (!game->isOver()) {
        const auto next_move = bot.selectMove(*game);
        game = game->applyMove(next_move);
    }
    return game;
}

Board removeDeadStones(std::shared_ptr<const GameState> game) {
    const unsigned int num_rounds = 1000;
    const auto orig_board = game->board();
    const auto num_rows = orig_board.numRows();
    const auto num_cols = orig_board.numCols();
    BoardCounter b_count(num_rows, num_cols);
    BoardCounter w_count(num_rows, num_cols);
    for (unsigned int num_done = 0; num_done < num_rounds; ++num_done) {
        // Randomly complete the game
        const auto final_state = completeGame(game);
        // Score the result
        const auto tmap = evaluateTerritory(final_state->board());
        // Count up the status
        for (unsigned int r = 0; r < num_rows; ++r) {
            for (unsigned int c = 0; c < num_cols; ++c) {
                const Point p(r, c);
                const auto status = tmap.at(p);
                if (status == PointStatus::black) {
                    b_count.increment(p);
                } else if (status == PointStatus::white) {
                    w_count.increment(p);
                }
            }
        }
    }

    // Remove dead stones.
    Board cleaned_board(num_rows, num_cols);
    for (unsigned int r = 0; r < num_rows; ++r) {
        for (unsigned int c = 0; c < num_cols; ++c) {
            const Point p(r, c);
            if (orig_board.isEmpty(p)) {
                continue;
            }
            const auto orig_stone = orig_board.at(p);
            if (orig_stone == Stone::black) {
                const double p_white = w_count.get(p) / float(num_rounds);
                if (p_white < DEAD_THRESHOLD) {
                    cleaned_board.place(p, orig_stone);
                }
            } else {
                const double p_black = b_count.get(p) / float(num_rounds);
                if (p_black < DEAD_THRESHOLD) {
                    cleaned_board.place(p, orig_stone);
                }
            }
        }
    }
    return cleaned_board;
}

}
