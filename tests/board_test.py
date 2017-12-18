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
