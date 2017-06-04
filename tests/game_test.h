#include <cxxtest/TestSuite.h>

#include "../baduk/game.h"

class GameTestSuite : public CxxTest::TestSuite {
public:
    void testGameState() {
        auto game = baduk::newGame(19);
        game->applyMove(baduk::Play("Q16"));
        const auto state = game->currentState();
        const auto board = state->board();
        TS_ASSERT_EQUALS(baduk::Stone::black, board.at("Q16"));
        TS_ASSERT_EQUALS(baduk::Stone::white, state->nextPlayer());
    }

    void testKo() {
        auto game = baduk::newGame(19);
        game->applyMove(baduk::Play("Q16"));
        game->applyMove(baduk::Play("P16"));
        game->applyMove(baduk::Play("P17"));
        game->applyMove(baduk::Play("O17"));
        game->applyMove(baduk::Play("P15"));
        game->applyMove(baduk::Play("O15"));
        game->applyMove(baduk::Play("A1"));
        game->applyMove(baduk::Play("N16"));
        // Take the ko
        game->applyMove(baduk::Play("O16"));

        // White can't retake it
        TS_ASSERT(!game->isPlayLegal(baduk::Play("P16")));

        // White plays elsewhere
        game->applyMove(baduk::Play("T1"));
        // Black responds
        game->applyMove(baduk::Play("T2"));

        // White can take ko now
        TS_ASSERT(game->isPlayLegal(baduk::Play("P16")));
    }
};
