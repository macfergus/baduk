#include <cxxtest/TestSuite.h>

#include "../baduk/board.h"

class BoardTestSuite : public CxxTest::TestSuite {
public:
    void testEqual() {
        baduk::Board board1(19, 19);
        board1.place("A1", baduk::Stone::black);
        board1.place("A2", baduk::Stone::white);
        board1.place("B1", baduk::Stone::white);

        baduk::Board board2(19, 19);
        board2.place("A2", baduk::Stone::white);
        board2.place("B1", baduk::Stone::white);

        TS_ASSERT(board1.isEmpty("A1"))
        TS_ASSERT(board2.isEmpty("A1"))
        TS_ASSERT_EQUALS(board1, board2);
    }

    void testCopy() {
        baduk::Board board1(19, 19);
        board1.place("A1", baduk::Stone::black);
        board1.place("A2", baduk::Stone::black);
        board1.place("B2", baduk::Stone::black);

        baduk::Board board2(board1);
        board2.place("B1", baduk::Stone::white);

        const auto string1 = board1.stringAt("A2");
        const auto string2 = board2.stringAt("A2");

        TS_ASSERT_EQUALS(4, string1.numLiberties());
        TS_ASSERT_EQUALS(3, string2.numLiberties());
    }

    void testCaptureIsNotSuicide() {
        baduk::Board board(19, 19);
        board.place("A1", baduk::Stone::black);
        board.place("B2", baduk::Stone::black);
        board.place("C1", baduk::Stone::black);
        board.place("A2", baduk::Stone::white);
        board.place("B1", baduk::Stone::white);
        TS_ASSERT(board.isEmpty("A1"));
        TS_ASSERT_EQUALS(baduk::Stone::white, board.at("A2"));
        TS_ASSERT_EQUALS(baduk::Stone::white, board.at("B1"));
    }

    void testCaptureAddsLiberties() {
        baduk::Board board(19, 19);
        board.place("P17", baduk::Stone::black);
        board.place("Q16", baduk::Stone::black);
        board.place("O16", baduk::Stone::black);
        board.place("P15", baduk::Stone::black);
        board.place("O17", baduk::Stone::white);
        board.place("N16", baduk::Stone::white);
        board.place("O15", baduk::Stone::white);
        board.place("P16", baduk::Stone::white);
        const auto string = board.stringAt("P16");
        TS_ASSERT_EQUALS(1, string.numLiberties());
    }

    void testWillCapture() {
        baduk::Board board(19, 19);
        board.place("A1", baduk::Stone::black);
        board.place("B2", baduk::Stone::black);
        board.place("C1", baduk::Stone::black);
        board.place("A2", baduk::Stone::white);
        TS_ASSERT(board.willCapture("B1", baduk::Stone::white));
    }

    void testWillHaveNoLiberties() {
        // ooo.
        // x*xo
        baduk::Board board(19, 19);
        board.place("A1", baduk::Stone::black);
        board.place("A3", baduk::Stone::black);
        board.place("B1", baduk::Stone::white);
        board.place("B2", baduk::Stone::white);
        board.place("B3", baduk::Stone::white);
        board.place("A4", baduk::Stone::white);
        TS_ASSERT(board.willHaveNoLiberties("A2", baduk::Stone::black));
    }

    void testWillHaveNoLiberties_newStoneAddsLiberty() {
        // o.o.
        // x*xo
        baduk::Board board(19, 19);
        board.place("A1", baduk::Stone::black);
        board.place("A3", baduk::Stone::black);
        board.place("B1", baduk::Stone::white);
        board.place("B3", baduk::Stone::white);
        board.place("A4", baduk::Stone::white);
        TS_ASSERT(!board.willHaveNoLiberties("A2", baduk::Stone::black));
    }

    void testWillHaveNoLiberties_connectingStoneHasLiberty() {
        // ooo.
        // x*x.
        baduk::Board board(19, 19);
        board.place("A1", baduk::Stone::black);
        board.place("A3", baduk::Stone::black);
        board.place("B1", baduk::Stone::white);
        board.place("B2", baduk::Stone::white);
        board.place("B3", baduk::Stone::white);
        TS_ASSERT(!board.willHaveNoLiberties("A2", baduk::Stone::black));
    }

    void verifyHash(baduk::Board orig, baduk::Point p, baduk::Stone player) {
        const auto anticipated_hash = orig.hashAfter(p, player);
        orig.place(p, player);
        const auto actual_hash = orig.hash();
        TS_ASSERT_EQUALS(anticipated_hash, actual_hash);
    }

    void testHashAfter() {
        // .....    .....
        // .....    .....
        // ..... -> ..x..
        // .....    .....
        // .....    .....
        baduk::Board board(5, 5);
        verifyHash(board, "C3", baduk::Stone::black);
        verifyHash(board, "C3", baduk::Stone::white);
    }

    void testHashAfter_capture() {
        // .....    .....
        // ..x..    ..x..
        // .xo.. -> .x.x.
        // ..x..    ..x..
        // .....    .....
        baduk::Board board(5, 5);
        board.place("C2", baduk::Stone::black);
        board.place("B3", baduk::Stone::black);
        board.place("C4", baduk::Stone::black);
        board.place("C3", baduk::Stone::white);
        verifyHash(board, "D3", baduk::Stone::black);
    }

    void testStringIteration() {
        baduk::Board board(5, 5);
        // .o...
        // .o...
        // ....o
        // xx...
        // .x...
        board.place("A2", baduk::Stone::black);
        board.place("B1", baduk::Stone::black);
        board.place("B2", baduk::Stone::black);
        board.place("E3", baduk::Stone::white);
        board.place("B4", baduk::Stone::white);
        board.place("B5", baduk::Stone::white);
        int white_strings = 0;
        int black_strings = 0;
        auto it = board.stringsBegin();
        while (it != board.stringsEnd()) {
            if (it->color() == baduk::Stone::black) {
                ++black_strings;
            } else {
                ++white_strings;
            }
            ++it;
        }
        TS_ASSERT_EQUALS(black_strings, 1);
        TS_ASSERT_EQUALS(white_strings, 2);
    }
};
