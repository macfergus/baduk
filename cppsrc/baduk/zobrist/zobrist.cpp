#include "zobrist.h"

namespace baduk {
namespace zobrist {

ZobristCodes::ZobristCodes() {
    initialize();
}

hashcode ZobristCodes::getEmpty(::baduk::Point const& p) const {
    return empty_codes_.at(index(p));
}

hashcode ZobristCodes::getStone(
        ::baduk::Stone stone, ::baduk::Point const& p) const {
    return stone == ::baduk::Stone::black ?
        black_codes_.at(index(p)) :
        white_codes_.at(index(p));
}

}
}
