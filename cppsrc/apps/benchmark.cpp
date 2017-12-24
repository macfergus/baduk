#include <iostream>

#include "../baduk/baduk.h"

int main() {
    for (int i = 0; i < 4000; ++i) {
        auto game = baduk::newGame(9);
        baduk::RandomBot black_bot;
        baduk::RandomBot white_bot;

        while (!game->isOver()) {
            auto next_agent =
                game->nextPlayer() == baduk::Stone::black ?
                    &black_bot :
                    &white_bot;
            const auto next_move = next_agent->selectMove(*game);

            game = game->applyMove(next_move);
        }
        std::cout << game->board() << "\n";
    }

    return 0;
}
