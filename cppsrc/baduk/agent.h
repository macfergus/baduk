#ifndef incl_BADUK_AGENT_H__
#define incl_BADUK_AGENT_H__

#include <chrono>
#include <random>

#include "game.h"

namespace baduk {

class Agent {
public:
    virtual Move selectMove(GameState const& game_state) = 0;
};

class RandomBot : public Agent {
public:
    RandomBot() :
        rng_(
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count()) {}
    Move selectMove(GameState const& game_state) override;

private:
    std::default_random_engine rng_;
};

}

#endif
