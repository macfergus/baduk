#ifndef incl_BADUK_ZOBRIST_ZOBRIST_H__
#define incl_BADUK_ZOBRIST_ZOBRIST_H__

#include <vector>

#include "../point.h"

namespace baduk {
namespace zobrist {

using hashcode = unsigned long long;

const hashcode BLACK_TO_PLAY = 3497503632401365394LL;
const hashcode WHITE_TO_PLAY = 8648928564975707047LL;

const unsigned int MAX_BOARD_SIZE = 25;

class ZobristCodes {
public:
    static ZobristCodes& get() {
        static ZobristCodes instance;
        return instance;
    }

    hashcode getStone(::baduk::Stone, ::baduk::Point const&) const;
    hashcode getEmpty(::baduk::Point const&) const;

    hashcode emptyBoard() const { return empty_board_; }

private:
    ZobristCodes();

    unsigned int index(::baduk::Point const& p) const {
        return MAX_BOARD_SIZE * p.row() + p.col();
    }

    void initialize();

    std::vector<hashcode> black_codes_;
    std::vector<hashcode> white_codes_;
    std::vector<hashcode> empty_codes_;

    hashcode empty_board_;
};

}
}

#endif
