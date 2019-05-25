# cython: language_level=3

import collections
import enum

import numpy as np

from cython.operator cimport dereference as deref
from libcpp cimport bool
from libcpp.memory cimport shared_ptr, unique_ptr

cimport numpy as np

DTYPE = np.float64
ctypedef np.float64_t DTYPE_t

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
        CGoString stringAt(CPoint) const

        bool operator==(CBoard) const

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
    bool isPass(CMove)
    bool isResign(CMove)
    CPoint getPoint(CMove)

    cdef cppclass CGameState "baduk::GameState":
        CBoard board() const
        CStone nextPlayer() const
        bool isMoveLegal(CMove) const
        bool doesMoveViolateKo(CMove) const
        bool isOver() const
        bool hasLastMove() const
        CMove lastMove() const
        float komi() const
        int numMoves() const

        shared_ptr[const CGameState] applyMove(CMove) const

    shared_ptr[const CGameState] newGame(unsigned int, float)
    shared_ptr[const CGameState] gameFromBoard(CBoard, CStone, float)

    CBoard removeDeadStones(shared_ptr[const CGameState])

cdef extern from "baduk/baduk.h" namespace "baduk::Stone":
    cdef CStone CBlackStone "baduk::Stone::black"
    cdef CStone CWhiteStone "baduk::Stone::white"

cdef class Point:
    cdef public unsigned int row
    cdef public unsigned int col

    def __cinit__(self, unsigned int row, unsigned int col):
        self.row = row
        self.col = col

    def __hash__(self):
        return hash((self.row, self.col))

    def __eq__(self, other):
        return self.row == other.row and self.col == other.col

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

    def __eq__(self, other):
        if self.is_pass and other.is_pass:
            return True
        if self.is_resign and other.is_resign:
            return True
        if self.is_play and other.is_play:
            return self.point == other.point
        return False

    def __hash__(self):
        return hash((self.is_pass, self.is_resign, self.is_play, self.point))

cdef CMove c_move(move):
    if move.is_pass:
        return CMove(CPass())
    if move.is_resign:
        return CMove(CResign())
    return CMove(CPlay(c_point(move.point)))

cdef py_move(CMove c_move):
    if isPass(c_move):
        return Move.pass_turn()
    if isResign(c_move):
        return Move.resign()
    return Move.play(py_point(getPoint(c_move)))

cdef class GoString:
    cdef CGoString c_string

    @property
    def color(self):
        return py_player(self.c_string.color())

    @property
    def num_liberties(self):
        return self.c_string.numLiberties()

cdef wrap_gostring(CGoString the_string):
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

cdef py_point(CPoint point):
    return Point(point.row() + 1, point.col() + 1)

cdef class Board:
    cdef unique_ptr[CBoard] c_board
    def __cinit__(self, unsigned int num_rows, unsigned int num_cols):
        self.c_board.reset(new CBoard(num_rows, num_cols))

    def is_on_grid(self, point):
        return 1 <= point.row <= deref(self.c_board).numRows() and \
            1 <= point.col <= deref(self.c_board).numCols()

    def place_stone(self, player, point):
        deref(self.c_board).place(c_point(point), c_player(player))

    def is_empty(self, point):
        cdef CPoint pt = c_point(point)
        return deref(self.c_board).isEmpty(pt)

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

    @property
    def num_rows(self):
        return deref(self.c_board).numRows()

    @property
    def num_cols(self):
        return deref(self.c_board).numCols()

    def __eq__(self, Board other):
        return deref(self.c_board) == deref(other.c_board)

    def black_stones_as_array(self):
        cdef np.ndarray[DTYPE_t, ndim=2] x = \
            np.zeros((self.num_rows, self.num_cols), dtype=DTYPE)
        cdef unsigned int r = 0
        cdef unsigned int c = 0
        cdef unsigned int num_rows = self.num_rows
        cdef unsigned int num_cols = self.num_cols
        cdef CPoint p = CPoint(0, 0)
        while r < num_rows:
            c = 0
            while c < num_cols:
                p = CPoint(r, c)
                if not deref(self.c_board).isEmpty(p):
                    if deref(self.c_board).at(p) == CBlackStone:
                        x[r, c] = 1
                c += 1
            r += 1
        return x

    def white_stones_as_array(self):
        cdef np.ndarray[DTYPE_t, ndim=2] x = \
            np.zeros((self.num_rows, self.num_cols), dtype=DTYPE)
        cdef unsigned int r = 0
        cdef unsigned int c = 0
        cdef unsigned int num_rows = self.num_rows
        cdef unsigned int num_cols = self.num_cols
        cdef CPoint p = CPoint(0, 0)
        while r < num_rows:
            c = 0
            while c < num_cols:
                p = CPoint(r, c)
                if not deref(self.c_board).isEmpty(p):
                    if deref(self.c_board).at(p) == CWhiteStone:
                        x[r, c] = 1
                c += 1
            r += 1
        return x

    def stones_with_n_liberties_as_array(self, color, num_libs):
        cdef CStone player = c_player(color)
        cdef np.ndarray[DTYPE_t, ndim=2] x = \
            np.zeros((self.num_rows, self.num_cols), dtype=DTYPE)
        cdef unsigned int r = 0
        cdef unsigned int c = 0
        cdef unsigned int num_rows = self.num_rows
        cdef unsigned int num_cols = self.num_cols
        cdef CPoint p = CPoint(0, 0)
        while r < num_rows:
            c = 0
            while c < num_cols:
                p = CPoint(r, c)
                if not deref(self.c_board).isEmpty(p):
                    if deref(self.c_board).stringAt(p).color() == player \
                            and deref(self.c_board).stringAt(p).numLiberties() == num_libs:
                        x[r, c] = 1
                c += 1
            r += 1
        return x

    def stones_with_min_liberties_as_array(self, color, min_libs):
        cdef CStone player = c_player(color)
        cdef np.ndarray[DTYPE_t, ndim=2] x = \
            np.zeros((self.num_rows, self.num_cols), dtype=DTYPE)
        cdef unsigned int r = 0
        cdef unsigned int c = 0
        cdef unsigned int num_rows = self.num_rows
        cdef unsigned int num_cols = self.num_cols
        cdef CPoint p = CPoint(0, 0)
        while r < num_rows:
            c = 0
            while c < num_cols:
                p = CPoint(r, c)
                if not deref(self.c_board).isEmpty(p):
                    if deref(self.c_board).stringAt(p).color() == player \
                            and deref(self.c_board).stringAt(p).numLiberties() >= min_libs:
                        x[r, c] = 1
                c += 1
            r += 1
        return x

cdef copy_and_wrap_board(CBoard board):
    pyboard = Board(1, 1)
    pyboard.c_board.reset(new CBoard(board))
    return pyboard

cdef create_game(unsigned int board_size, float komi):
    new_gs = GameState()
    new_gs.c_gamestate = newGame(board_size, komi)
    return new_gs

cdef create_game_from_board(Board board, next_player, float komi):
    new_gs = GameState()
    new_gs.c_gamestate = gameFromBoard(
        deref(board.c_board),
        c_player(next_player),
        komi)
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
    def new_game(cls, board_size, komi=7.5):
        return create_game(board_size, komi)

    @classmethod
    def from_board(cls, board, next_player, komi=7.5):
        return create_game_from_board(board, next_player, komi)

    @property
    def board(self):
        return copy_and_wrap_board(deref(self.c_gamestate).board())

    @property
    def last_move(self):
        if not deref(self.c_gamestate).hasLastMove():
            return None
        return py_move(deref(self.c_gamestate).lastMove())

    @property
    def num_moves(self):
        return deref(self.c_gamestate).numMoves()

    cpdef bool is_over(self):
        return deref(self.c_gamestate).isOver()

    cpdef bool is_valid_move(self, Move move):
        return deref(self.c_gamestate).isMoveLegal(c_move(move))

    cpdef bool does_move_violate_ko(self, Move move):
        return deref(self.c_gamestate).doesMoveViolateKo(c_move(move))

    cpdef legal_plays(self):
        moves = []
        for row in range(1, self.board.num_rows + 1):
            for col in range(1, self.board.num_cols + 1):
                move = Move.play(Point(row, col))
                if self.is_valid_move(move):
                    moves.append(move)
        return moves

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

    cpdef komi(self):
        return deref(self.c_gamestate).komi()

    def ko_points_as_array(self):
        cdef unsigned int num_rows = deref(self.c_gamestate).board().numRows()
        cdef unsigned int num_cols = deref(self.c_gamestate).board().numCols()
        cdef np.ndarray[DTYPE_t, ndim=2] x = \
            np.zeros((num_rows, num_cols), dtype=DTYPE)
        cdef unsigned int r = 0
        cdef unsigned int c = 0
        cdef CPoint p = CPoint(0, 0)
        cdef CMove move = CMove(CPlay(p))
        while r < num_rows:
            c = 0
            while c < num_cols:
                p = CPoint(r, c)
                move = CMove(CPlay(p))
                if deref(self.c_gamestate).doesMoveViolateKo(move):
                    x[r, c] = 1
                c += 1
            r += 1
        return x

    def legal_moves_as_array(self):
        cdef unsigned int num_rows = deref(self.c_gamestate).board().numRows()
        cdef unsigned int num_cols = deref(self.c_gamestate).board().numCols()
        cdef unsigned int size = num_rows * num_cols + 1
        cdef np.ndarray[DTYPE_t, ndim=1] x = np.zeros((size,), dtype=DTYPE)
        cdef unsigned int r = 0
        cdef unsigned int c = 0
        cdef CPoint p = CPoint(0, 0)
        cdef CMove move = CMove(CPlay(p))
        while r < num_rows:
            c = 0
            while c < num_cols:
                p = CPoint(r, c)
                move = CMove(CPlay(p))
                if deref(self.c_gamestate).isMoveLegal(move):
                    x[num_cols * r + c] = 1
                c += 1
            r += 1
        # Pass is always legal.
        x[size - 1] = 1
        return x


def remove_dead_stones(GameState game):
    return copy_and_wrap_board(removeDeadStones(game.c_gamestate))
