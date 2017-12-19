import collections
import enum

from cython.operator cimport dereference as deref
from libcpp cimport bool
from libcpp.memory cimport shared_ptr, unique_ptr

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
        CBoard() except +
        CBoard(unsigned int, unsigned int) except +
        CBoard(CBoard) except +

        unsigned int numRows() const
        unsigned int numCols() const

        void place(CPoint, CStone) except +

        bool isEmpty(CPoint point) const
        CStone at(CPoint) const
        shared_ptr[CGoString] stringAt(CPoint) const

    cdef cppclass CPass "baduk::Pass":
        CPass()

    cdef cppclass CResign "baduk::Resign":
        CResign()

    cdef cppclass CPlay "baduk::Play":
        CPlay()
        CPlay(CPoint)

    cdef cppclass CMove "baduk::Move":
        CMove()
        CMove(CPass)
        CMove(CResign)
        CMove(CPlay)

    cdef cppclass CGameState "baduk::GameState":
        CBoard board() const
        CStone nextPlayer() const
        bool isMoveLegal(CMove) const
        bool isOver() const

        shared_ptr[const CGameState] applyMove(CMove) const

    shared_ptr[const CGameState] newGame(unsigned int)

cdef extern from "baduk/baduk.h" namespace "baduk::Stone":
    cdef CStone CBlackStone "baduk::Stone::black"
    cdef CStone CWhiteStone "baduk::Stone::white"

cdef class Point:
    cdef unsigned int row
    cdef unsigned int col

    def __cinit__(self, unsigned int row, unsigned int col):
        self.row = row
        self.col = col

cdef class Move:
    cdef public point
    cdef public is_play
    cdef public is_pass
    cdef public is_resign

    def __init__(self, point=None, is_pass=False, is_resign=False):
        assert (point is not None) ^ is_pass ^ is_resign
        self.point = point
        self.is_play = (self.point is not None)
        self.is_pass = is_pass
        self.is_resign = is_resign

    @classmethod
    def play(cls, point):
        return Move(point=point)

    @classmethod
    def pass_turn(cls):
        return Move(is_pass=True)

    @classmethod
    def resign(cls):
        return Move(is_resign=True)

cdef CMove c_move(move):
    if move.is_pass:
        return CMove(CPass())
    if move.is_resign:
        return CMove(CResign())
    return CMove(CPlay(c_point(move.point)))

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
    cdef unique_ptr[CBoard] c_board
    def __cinit__(self, unsigned int num_rows, unsigned int num_cols):
        self.c_board.reset(new CBoard(num_rows, num_cols))

    def place_stone(self, player, point):
        deref(self.c_board).place(c_point(point), c_player(player))

    def get(self, point):
        cdef CPoint pt = c_point(point)
        if deref(self.c_board).isEmpty(pt):
            return None
        c_player = deref(self.c_board).at(pt)
        return py_player(c_player)

    def get_string(self, point):
        cdef CPoint pt = c_point(point)
        if deref(self.c_board).isEmpty(pt):
            return None
        return wrap_gostring(deref(self.c_board).stringAt(pt))

cdef copy_and_wrap_board(CBoard board):
    pyboard = Board(1, 1)
    pyboard.c_board.reset(new CBoard(board))
    return pyboard

cdef create_game(unsigned int board_size):
    new_gs = GameState()
    new_gs.c_gamestate = newGame(board_size)
    return new_gs

cdef wrap_gamestate(shared_ptr[const CGameState] gamestate):
    new_gs = GameState()
    new_gs.c_gamestate = gamestate
    return new_gs

cdef class GameState:
    cdef shared_ptr[const CGameState] c_gamestate
    cdef public previous_state

    def __cinit__(self):
        self.previous_state = None

    cdef shared_ptr[const CGameState] c_apply_move(self, Move move):
        return deref(self.c_gamestate).applyMove(c_move(move))

    def apply_move(self, Move move):
        child = wrap_gamestate(self.c_apply_move(move))
        child.previous_state = self
        return child

    @property
    def next_player(self):
        return py_player(deref(self.c_gamestate).nextPlayer())

    @classmethod
    def new_game(cls, board_size):
        return create_game(board_size)

    @property
    def board(self):
        return copy_and_wrap_board(deref(self.c_gamestate).board())

    cpdef bool is_over(self):
        return deref(self.c_gamestate).isOver()

    cpdef bool is_valid_move(self, Move move):
        return deref(self.c_gamestate).isMoveLegal(c_move(move))

    cpdef legal_moves(self):
        moves = []
        for row in range(1, self.board.num_rows + 1):
            for col in range(1, self.board.num_cols + 1):
                move = Move.play(Point(row, col))
                if self.is_valid_move(move):
                    moves.append(move)
        # These two moves are always legal.
        moves.append(Move.pass_turn())
        moves.append(Move.resign())
        return moves
