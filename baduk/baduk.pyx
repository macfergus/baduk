import collections
import enum

from cython.operator cimport dereference as deref
from libcpp cimport bool
from libcpp.memory cimport shared_ptr

cdef extern from "baduk/baduk.h" namespace "baduk":
    cdef cppclass CPoint "baduk::Point":
        CPoint(unsigned int, unsigned int)
        CPoint()

        unsigned int row() const
        unsigned int col() const

    cdef cppclass CStone "baduk::Stone":
        bool operator==(CStone) const

    cdef cppclass CGoString "baduk::GoString":
        CStone color() const
        size_t numLiberties() const

    cdef cppclass CBoard "baduk::Board":
        CBoard(unsigned int, unsigned int) except +

        unsigned int numRows() const
        unsigned int numCols() const

        void place(CPoint, CStone) except +

        bool isEmpty(CPoint point) const
        CStone at(CPoint) const
        shared_ptr[CGoString] stringAt(CPoint) const

cdef extern from "baduk/baduk.h" namespace "baduk::Stone":
    cdef CStone CBlackStone "baduk::Stone::black"
    cdef CStone CWhiteStone "baduk::Stone::white"

cdef class Point:
    cdef unsigned int row
    cdef unsigned int col

    def __cinit__(self, unsigned int row, unsigned int col):
        self.row = row
        self.col = col

cdef class GoString:
    cdef shared_ptr[CGoString] c_string

    @property
    def num_liberties(self):
        return deref(self.c_string).numLiberties()

cdef wrap_gostring(shared_ptr[CGoString] the_string):
    result = GoString()
    result.c_string = the_string
    return result

class Player(enum.Enum):
    black = 1
    white = 2

    @property
    def other(self):
        return Player.black if self == Player.white else Player.white

cdef CStone c_player(player):
    if player == Player.black:
        return CBlackStone
    if player == Player.white:
        return CWhiteStone
    raise ValueError(player)

cdef py_player(CStone c_player):
    if c_player == CBlackStone:
        return Player.black
    if c_player == CWhiteStone:
        return Player.white
    raise ValueError()

cdef CPoint c_point(Point point):
    return CPoint(point.row - 1, point.col - 1)

cdef class Board:
    cdef CBoard* c_board
    def __cinit__(self, unsigned int num_rows, unsigned int num_cols):
        self.c_board = new CBoard(num_rows, num_cols)

    def __dealloc__(self):
        del self.c_board

    def place_stone(self, player, point):
        self.c_board.place(c_point(point), c_player(player))

    def get(self, point):
        cdef CPoint pt = c_point(point)
        if self.c_board.isEmpty(pt):
            return None
        c_player = self.c_board.at(pt)
        return py_player(c_player)

    def get_string(self, point):
        cdef CPoint pt = c_point(point)
        if self.c_board.isEmpty(pt):
            return None
        return wrap_gostring(self.c_board.stringAt(pt))
