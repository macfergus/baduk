#include <cxxtest/TestSuite.h>

#include "../baduk/board.h"

class BoardTestSuite : public CxxTest::TestSuite {
public:
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
};
