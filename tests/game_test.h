#include <cxxtest/TestSuite.h>

#include "../baduk/game.h"

class GameTestSuite : public CxxTest::TestSuite {
public:
    void testGameState() {
        auto game = baduk::newGame(19);
        game = game->applyMove(baduk::Play("Q16"));
        const auto board = game->board();
        TS_ASSERT_EQUALS(baduk::Stone::black, board.at("Q16"));
        TS_ASSERT_EQUALS(baduk::Stone::white, game->nextPlayer());
    }

    void testKo() {
        auto game = baduk::newGame(19);
        game = game->applyMove(baduk::Play("Q16"));
        game = game->applyMove(baduk::Play("P16"));
        game = game->applyMove(baduk::Play("P17"));
        game = game->applyMove(baduk::Play("O17"));
        game = game->applyMove(baduk::Play("P15"));
        game = game->applyMove(baduk::Play("O15"));
        game = game->applyMove(baduk::Play("A1"));
        game = game->applyMove(baduk::Play("N16"));
        // Take the ko
        game = game->applyMove(baduk::Play("O16"));

        // White can't retake it
        TS_ASSERT(!game->isMoveLegal(baduk::Play("P16")));

        // White plays elsewhere
        game = game->applyMove(baduk::Play("T1"));
        // Black responds
        game = game->applyMove(baduk::Play("T2"));

        // White can take ko now
        TS_ASSERT(game->isMoveLegal(baduk::Play("P16")));
    }
};
