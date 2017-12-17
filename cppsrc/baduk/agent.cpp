#include <cassert>
#include <vector>

#include "agent.h"

namespace baduk {

bool isPointAnEye(Board const& board, Point const& point, Stone stone) {
    // An eye is an empty point.
    if (!board.isEmpty(point)) {
        return false;
    }

    // All adjacent points must contain friendly stones.
    for (auto neighbor : board.neighbors(point)) {
        if (board.isEmpty(neighbor)) {
            return false;
        }
        if (board.at(neighbor) != stone) {
            return false;
        }
    }

    // We must control 3 out of 4 corners in the middle. On the edge we must
    // control all corners.
    unsigned int friendly_corners = 0;
    unsigned int off_board_corners = 0;
    std::vector<Point> corners;
    if (point.row() > 0 && point.col() > 0) {
        corners.push_back(Point(point.row() - 1, point.col() - 1));
    } else {
        ++off_board_corners;
    }
    if (point.row() > 0 && point.col() < board.numCols() - 1) {
        corners.push_back(Point(point.row() - 1, point.col() + 1));
    } else {
        ++off_board_corners;
    }
    if (point.row() < board.numRows() - 1 && point.col() > 0) {
        corners.push_back(Point(point.row() + 1, point.col() - 1));
    } else {
        ++off_board_corners;
    }
    if (point.row() < board.numRows() - 1 &&
            point.col() < board.numCols() - 1) {
        corners.push_back(Point(point.row() + 1, point.col() + 1));
    } else {
        ++off_board_corners;
    }
    for (auto corner : corners) {
        if (!board.isEmpty(corner) && board.at(corner) == stone) {
            ++friendly_corners;
        }
    }
    if (off_board_corners > 0) {
        return off_board_corners + friendly_corners == 4;
    }
    return friendly_corners >= 3;
}

Move RandomBot::selectMove(GameState const& game_state) {
    std::vector<Point> candidates;
    auto const board = game_state.board();
    for (unsigned int r = 0; r < board.numRows(); ++r) {
        for (unsigned int c = 0; c < board.numCols(); ++c) {
            const Point candidate(r, c);
            if (game_state.isMoveLegal(Play(candidate)) &&
                    !isPointAnEye(
                        board, candidate, game_state.nextPlayer())) {
                assert(board.isEmpty(candidate));
                candidates.push_back(candidate);
            }
        }
    }

    if (candidates.empty()) {
        return Pass();
    }
    std::uniform_int_distribution<unsigned int> dist(0, candidates.size() - 1);
    return candidates[dist(rng_)];
}

}
