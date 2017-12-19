import unittest

from baduk import GameState, Move, Player, Point


class GameTest(unittest.TestCase):
    def test_new_game(self):
        start = GameState.new_game(19)
        next_state = start.apply_move(Move.play(Point(16, 16)))

        self.assertEqual(start, next_state.previous_state)
        self.assertEqual(Player.white, next_state.next_player)
        self.assertEqual(Player.black, next_state.board.get(Point(16, 16)))


if __name__ == '__main__':
    unittest.main()
