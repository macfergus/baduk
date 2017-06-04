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
};
