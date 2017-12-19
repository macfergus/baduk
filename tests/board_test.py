import unittest

from baduk import Board, Player, Point

class BoardTest(unittest.TestCase):
    def test_place_stone(self):
        board = Board(5, 5)
        self.assertIsNone(board.get(Point(2, 3)))
        self.assertIsNone(board.get(Point(3, 3)))

        board.place_stone(Player.black, Point(3, 3))

        self.assertIsNone(board.get(Point(2, 3)))
        self.assertEqual(Player.black, board.get(Point(3, 3)))

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
        self.assertEqual(1, black_string.num_liberties)

        white_string = board.get_string(Point(3, 2))
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
