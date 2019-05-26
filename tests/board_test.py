import unittest

import numpy as np

from baduk import Board, Player, Point

class BoardTest(unittest.TestCase):
    def test_place_stone(self):
        board = Board(5, 5)
        self.assertIsNone(board.get(Point(2, 3)))
        self.assertIsNone(board.get(Point(3, 3)))

        board.place_stone(Player.black, Point(3, 3))

        self.assertIsNone(board.get(Point(2, 3)))
        self.assertEqual(Player.black, board.get(Point(3, 3)))

    def test_equality(self):
        board1 = Board(5, 5)
        board2 = Board(5, 5)

        board1.place_stone(Player.black, Point(3, 3))

        self.assertNotEqual(board1, board2)

        board2.place_stone(Player.black, Point(3, 3))
        self.assertEqual(board1, board2)

    def test_corner_capture(self):
        board = Board(5, 5)
        board.place_stone(Player.black, Point(1, 1))
        board.place_stone(Player.white, Point(1, 2))
        board.place_stone(Player.white, Point(2, 1))
        self.assertIsNone(board.get(Point(1, 1)))

    def test_get_string(self):
        board = Board(5, 5)
        # ....
        # .o..
        # xo..
        # xxo.
        board.place_stone(Player.black, Point(1, 1))
        board.place_stone(Player.black, Point(1, 2))
        board.place_stone(Player.black, Point(2, 1))
        board.place_stone(Player.white, Point(1, 3))
        board.place_stone(Player.white, Point(2, 2))
        board.place_stone(Player.white, Point(3, 2))

        black_string = board.get_string(Point(1, 1))
        self.assertEqual(Player.black, black_string.color)
        self.assertEqual(1, black_string.num_liberties)

        white_string = board.get_string(Point(3, 2))
        self.assertEqual(Player.white, white_string.color)
        self.assertEqual(4, white_string.num_liberties)

    def test_capture(self):
        board = Board(19, 19)
        board.place_stone(Player.black, Point(2, 2))
        board.place_stone(Player.white, Point(1, 2))
        self.assertEqual(Player.black, board.get(Point(2, 2)))
        board.place_stone(Player.white, Point(2, 1))
        self.assertEqual(Player.black, board.get(Point(2, 2)))
        board.place_stone(Player.white, Point(2, 3))
        self.assertEqual(Player.black, board.get(Point(2, 2)))
        board.place_stone(Player.white, Point(3, 2))
        self.assertIsNone(board.get(Point(2, 2)))

    def test_capture_two_stones(self):
        board = Board(19, 19)
        board.place_stone(Player.black, Point(2, 2))
        board.place_stone(Player.black, Point(2, 3))
        board.place_stone(Player.white, Point(1, 2))
        board.place_stone(Player.white, Point(1, 3))
        self.assertEqual(Player.black, board.get(Point(2, 2)))
        self.assertEqual(Player.black, board.get(Point(2, 3)))
        board.place_stone(Player.white, Point(3, 2))
        board.place_stone(Player.white, Point(3, 3))
        self.assertEqual(Player.black, board.get(Point(2, 2)))
        self.assertEqual(Player.black, board.get(Point(2, 3)))
        board.place_stone(Player.white, Point(2, 1))
        board.place_stone(Player.white, Point(2, 4))
        self.assertIsNone(board.get(Point(2, 2)))
        self.assertIsNone(board.get(Point(2, 3)))

    def test_capture_is_not_suicide(self):
        board = Board(19, 19)
        board.place_stone(Player.black, Point(1, 1))
        board.place_stone(Player.black, Point(2, 2))
        board.place_stone(Player.black, Point(1, 3))
        board.place_stone(Player.white, Point(2, 1))
        board.place_stone(Player.white, Point(1, 2))
        self.assertIsNone(board.get(Point(1, 1)))
        self.assertEqual(Player.white, board.get(Point(2, 1)))
        self.assertEqual(Player.white, board.get(Point(1, 2)))

    def test_remove_liberties(self):
        board = Board(5, 5)
        board.place_stone(Player.black, Point(3, 3))
        board.place_stone(Player.white, Point(2, 2))
        white_string = board.get_string(Point(2, 2))
        self.assertEqual(4, white_string.num_liberties)
        board.place_stone(Player.black, Point(3, 2))
        white_string = board.get_string(Point(2, 2))
        self.assertEqual(3, white_string.num_liberties)

    def test_empty_triangle(self):
        board = Board(5, 5)
        board.place_stone(Player.black, Point(1, 1))
        board.place_stone(Player.black, Point(1, 2))
        board.place_stone(Player.black, Point(2, 2))
        board.place_stone(Player.white, Point(2, 1))

        black_string = board.get_string(Point(1, 1))
        self.assertEqual(3, black_string.num_liberties)

    def test_black_stones_array(self):
        board = Board(5, 5)
        board.place_stone(Player.black, Point(2, 1))
        board.place_stone(Player.white, Point(3, 2))

        black_array = board.black_stones_as_array()
        self.assertEqual((5, 5), black_array.shape)
        self.assertEqual(1, black_array[1, 0])
        self.assertEqual(0, black_array[2, 1])
        self.assertEqual(0, black_array[3, 3])

    def test_white_stones_array(self):
        board = Board(5, 5)
        board.place_stone(Player.black, Point(2, 1))
        board.place_stone(Player.white, Point(3, 2))

        white_array = board.white_stones_as_array()
        self.assertEqual((5, 5), white_array.shape)
        self.assertEqual(1, white_array[2, 1])
        self.assertEqual(0, white_array[1, 0])
        self.assertEqual(0, white_array[3, 3])

    def test_num_liberties_array(self):
        # .....
        # .x.o.
        # .....
        # xo...
        # xox..
        board = Board(5, 5)
        board.place_stone(Player.black, Point(1, 1))
        board.place_stone(Player.black, Point(2, 1))
        board.place_stone(Player.black, Point(1, 3))
        board.place_stone(Player.black, Point(4, 2))
        board.place_stone(Player.white, Point(1, 2))
        board.place_stone(Player.white, Point(2, 2))
        board.place_stone(Player.white, Point(4, 4))
        one_lib = board.stones_with_n_liberties_as_array(Player.black, 1)
        # np array indexing is upside-down compared to usual board
        # notation
        np.testing.assert_array_equal(np.array([
            [1, 0, 0, 0, 0],
            [1, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
        ]), one_lib)
        b_two_lib = board.stones_with_n_liberties_as_array(Player.black, 2)
        np.testing.assert_array_equal(np.array([
            [0, 0, 1, 0, 0],
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
        ]), b_two_lib)
        w_two_lib = board.stones_with_n_liberties_as_array(Player.white, 2)
        np.testing.assert_array_equal(np.array([
            [0, 1, 0, 0, 0],
            [0, 1, 0, 0, 0],
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
        ]), w_two_lib)

    def test_min_liberties_array(self):
        # .....
        # .x.o.
        # .....
        # xo...
        # xox..
        board = Board(5, 5)
        board.place_stone(Player.black, Point(1, 1))
        board.place_stone(Player.black, Point(2, 1))
        board.place_stone(Player.black, Point(1, 3))
        board.place_stone(Player.black, Point(4, 2))
        board.place_stone(Player.white, Point(1, 2))
        board.place_stone(Player.white, Point(2, 2))
        board.place_stone(Player.white, Point(4, 4))
        two_lib = board.stones_with_min_liberties_as_array(Player.black, 2)
        # np array indexing is upside-down compared to usual board
        # notation
        np.testing.assert_array_equal(np.array([
            [0, 0, 1, 0, 0],
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
            [0, 1, 0, 0, 0],
            [0, 0, 0, 0, 0],
        ]), two_lib)
        three_lib = board.stones_with_min_liberties_as_array(Player.white, 3)
        np.testing.assert_array_equal(np.array([
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
            [0, 0, 0, 1, 0],
            [0, 0, 0, 0, 0],
        ]), three_lib)

    def test_liberties_array(self):
        # Board position:
        # .....
        # .x.o.
        # .....
        # xo...
        # xox..

        # 1/2 liberty points:
        # .....
        # .x.o.
        # **...
        # xo*..
        # xox*.
        board = Board(5, 5)
        board.place_stone(Player.black, Point(1, 1))
        board.place_stone(Player.black, Point(2, 1))
        board.place_stone(Player.black, Point(1, 3))
        board.place_stone(Player.black, Point(4, 2))
        board.place_stone(Player.white, Point(1, 2))
        board.place_stone(Player.white, Point(2, 2))
        board.place_stone(Player.white, Point(4, 4))

        two_lib = board.max_liberties_as_array(2)
        # np array indexing is upside-down compared to usual board
        # notation
        np.testing.assert_array_equal(np.array([
            [0, 0, 0, 1, 0],
            [0, 0, 1, 0, 0],
            [1, 1, 0, 0, 0],
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
        ]), two_lib)
