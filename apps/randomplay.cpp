#include <iostream>

#include "../baduk/baduk.h"

int main() {
    auto game = baduk::newGame(19);
    baduk::RandomBot black_bot;
    baduk::RandomBot white_bot;

    while (!game->isOver()) {
        std::cout << game->board() << "\n";
        auto next_agent =
            game->nextPlayer() == baduk::Stone::black ?
                &black_bot :
                &white_bot;
        const auto next_move = next_agent->selectMove(*game);

        game = game->applyMove(next_move);
    }
    std::cout << game->board() << "\n";

    return 0;
}
