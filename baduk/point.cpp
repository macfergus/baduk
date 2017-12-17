#include <iostream>

#include "board.h"

namespace baduk {

Stone other(Stone stone) {
    if (stone == Stone::black) {
        return Stone::white;
    }
    return Stone::black;
}

std::ostream& operator<<(std::ostream& out, Stone stone) {
    if (stone == Stone::black) {
        return out << "b";
    }
    return out << "w";
}

// I is excluded on purpose
const std::string COLS = "ABCDEFGHJKLMNOPQRSTUVWXYZ";

Point::Point(std::string const& name) {
    const auto col_s = name.substr(0, 1);
    const auto row_s = name.substr(1);
    col_ = COLS.find(col_s);
    row_ = std::stoi(row_s) - 1;
}

std::string Point::name() const {
    return COLS.substr(col_, 1) + std::to_string(row_ + 1);
}

}
