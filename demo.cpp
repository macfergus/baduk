#include <iostream>

#include "baduk/baduk.h"

void print_board(baduk::Board const& board) {
    for (int r = board.numRows() - 1; r >= 0; --r) {
        for (unsigned int c = 0; c < board.numCols(); ++c) {
            baduk::Point p(static_cast<unsigned int>(r), c);
            if (board.isEmpty(p)) {
                std::cout << ".";
            } else {
                const auto stone = board.at(p);
                std::cout << (stone == baduk::Stone::black ? "x" : "o");
            }
        }
        std::cout << "\n";
    }
}

int main() {
        baduk::Board board(19, 19);
        board.place("E4", baduk::Stone::black);
        board.place(baduk::Point(4, 4), baduk::Stone::white);
        board.place(baduk::Point(4, 3), baduk::Stone::black);
        board.place(baduk::Point(4, 5), baduk::Stone::white);
        board.place(baduk::Point(3, 5), baduk::Stone::black);
        board.place(baduk::Point(5, 5), baduk::Stone::black);
        board.place(baduk::Point(5, 4), baduk::Stone::black);
        board.place(baduk::Point(3, 3), baduk::Stone::white);
        board.place(baduk::Point(4, 6), baduk::Stone::black);
        print_board(board);
        return 0;
}
