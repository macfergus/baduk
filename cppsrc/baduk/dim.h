#ifndef incl_BADUK_DIM_H__
#define incl_BADUK_DIM_H__

namespace baduk {

// Set a global max board size because we pre-allocate some fixed-size
// arrays.
const unsigned int MAX_BOARD_SIZE = 19;
const auto MAX_POINTS = MAX_BOARD_SIZE * MAX_BOARD_SIZE;

struct Dim {
    unsigned int width;
    unsigned int height;

    Dim(unsigned int the_width, unsigned int the_height) :
        width(the_width),
        height(the_height) {}
};

}

#endif
