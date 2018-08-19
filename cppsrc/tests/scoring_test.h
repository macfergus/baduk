#include <cxxtest/TestSuite.h>

#include "../baduk/board.h"
#include "../baduk/scoring.h"

const auto black = baduk::PointStatus::black;
const auto white = baduk::PointStatus::white;
const auto neutral = baduk::PointStatus::neutral;

class ScoringTestSuite : public CxxTest::TestSuite {
public:
    void testTerritory() {
        // E . x o o .    x x o o o E
        // D . . x o .    x x x o o D
        // C . x x o . => x x x o o C
        // B . x . o .    x x . o o B
        // A . x . o .    x x . o o A
        //   1 2 3 4 5    1 2 3 4 5
        baduk::Board board(5, 5);
        board.place("A2", baduk::Stone::black);
        board.place("B2", baduk::Stone::black);
        board.place("C2", baduk::Stone::black);
        board.place("C3", baduk::Stone::black);
        board.place("D3", baduk::Stone::black);
        board.place("E2", baduk::Stone::black);
        board.place("A4", baduk::Stone::white);
        board.place("B4", baduk::Stone::white);
        board.place("C4", baduk::Stone::white);
        board.place("D4", baduk::Stone::white);
        board.place("E4", baduk::Stone::white);
        board.place("E3", baduk::Stone::white);

        const auto territory = baduk::evaluateTerritory(board);
        TS_ASSERT_EQUALS(territory.at("E1"), black);
        TS_ASSERT_EQUALS(territory.at("E2"), black);
        TS_ASSERT_EQUALS(territory.at("E3"), white);
        TS_ASSERT_EQUALS(territory.at("E4"), white);
        TS_ASSERT_EQUALS(territory.at("E5"), white);
        TS_ASSERT_EQUALS(territory.at("B3"), neutral);
        TS_ASSERT_EQUALS(territory.at("A3"), neutral);
    }
};
