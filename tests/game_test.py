import unittest

from baduk import GameState, Move, Player, Point


class GameTest(unittest.TestCase):
    def test_new_game(self):
        start = GameState.new_game(19)
        next_state = start.apply_move(Move.play(Point(16, 16)))

        self.assertEqual(start, next_state.previous_state)
        self.assertEqual(Player.white, next_state.next_player)
        self.assertEqual(Player.black, next_state.board.get(Point(16, 16)))

    def test_is_valid_move(self):
        start = GameState.new_game(19)
        state = start.apply_move(Move.play(Point(16, 16)))

        self.assertTrue(state.is_valid_move(Move.play(Point(16, 17))))
        self.assertFalse(state.is_valid_move(Move.play(Point(16, 16))))

    def test_last_move(self):
        start = GameState.new_game(19)
        next_move = Move.play(Point(16, 16))
        state = start.apply_move(next_move)

        self.assertEqual(Move.play(Point(16, 16)), state.last_move)

    def test_move_is_hashable(self):
        moves = {
            Move.play(Point(1, 1)): 1,
            Move.resign(): 2,
        }

        self.assertEqual(1, moves[Move.play(Point(1, 1))])
        self.assertEqual(2, moves[Move.resign()])


if __name__ == '__main__':
    unittest.main()
