import unittest

from baduk import Board, GameState, Move, Player, Point


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

    def test_last_move_is_none(self):
        start = GameState.new_game(19)
        self.assertIsNone(start.last_move)

    def test_move_is_hashable(self):
        moves = {
            Move.play(Point(1, 1)): 1,
            Move.resign(): 2,
        }

        self.assertEqual(1, moves[Move.play(Point(1, 1))])
        self.assertEqual(2, moves[Move.resign()])

    def test_ko(self):
        start = GameState.new_game(19)
        # .wb.
        # wb.b
        # .wb.
        # ....
        game = start.apply_move(Move.play(Point(1, 3)))
        game = game.apply_move(Move.play(Point(1, 2)))
        game = game.apply_move(Move.play(Point(2, 2)))
        game = game.apply_move(Move.play(Point(2, 1)))
        game = game.apply_move(Move.play(Point(3, 3)))
        game = game.apply_move(Move.play(Point(3, 2)))
        game = game.apply_move(Move.play(Point(2, 4)))

        # W takes the ko
        game = game.apply_move(Move.play(Point(2, 3)))
        # B can't take back
        self.assertTrue(game.does_move_violate_ko(Move.play(Point(2, 2))))
        self.assertFalse(game.is_valid_move(Move.play(Point(2, 2))))

        # "ko threat"
        game = game.apply_move(Move.play(Point(19, 19)))
        game = game.apply_move(Move.play(Point(18, 18)))

        # B can take now
        self.assertFalse(game.does_move_violate_ko(Move.play(Point(2, 2))))
        self.assertTrue(game.is_valid_move(Move.play(Point(2, 2))))

    def test_create_game_from_board(self):
        board = Board(5, 5)
        board.place_stone(Player.black, Point(2, 2))
        board.place_stone(Player.black, Point(4, 4))
        game = GameState.from_board(board, Player.white)

        self.assertEqual(Player.white, game.next_player)
        self.assertFalse(game.is_valid_move(Move.play(Point(2, 2))))
        self.assertTrue(game.is_valid_move(Move.play(Point(3, 3))))

    def test_komi(self):
        start = GameState.new_game(19, 0.5)
        next_state = start.apply_move(Move.play(Point(16, 16)))

        self.assertAlmostEqual(0.5, next_state.komi())

    def test_ko_as_array(self):
        start = GameState.new_game(19)
        # .wb.
        # wb.b
        # .wb.
        # ....
        game = start.apply_move(Move.play(Point(1, 3)))
        game = game.apply_move(Move.play(Point(1, 2)))
        game = game.apply_move(Move.play(Point(2, 2)))
        game = game.apply_move(Move.play(Point(2, 1)))
        game = game.apply_move(Move.play(Point(3, 3)))
        game = game.apply_move(Move.play(Point(3, 2)))
        game = game.apply_move(Move.play(Point(2, 4)))

        # W takes the ko
        game = game.apply_move(Move.play(Point(2, 3)))

        ko_array = game.ko_points_as_array()
        self.assertEqual((19, 19), ko_array.shape)
        self.assertEqual(1, ko_array[1, 1])
        self.assertEqual(0, ko_array[2, 1])
        self.assertEqual(0, ko_array[5, 5])

    def test_legal_move_mask(self):
        start = GameState.new_game(19)
        # .wb.
        # wb.b
        # .wb.
        # ....
        game = start.apply_move(Move.play(Point(1, 3)))
        game = game.apply_move(Move.play(Point(1, 2)))
        game = game.apply_move(Move.play(Point(2, 2)))
        game = game.apply_move(Move.play(Point(2, 1)))
        game = game.apply_move(Move.play(Point(3, 3)))
        game = game.apply_move(Move.play(Point(3, 2)))
        game = game.apply_move(Move.play(Point(2, 4)))

        # W takes the ko
        game = game.apply_move(Move.play(Point(2, 3)))

        legal_moves = game.legal_moves_as_array()
        self.assertEqual((19 * 19 + 1,), legal_moves.shape)
        illegal_indices = [
            # Suicide
            19 * 0 + 0,
            # Stones here
            19 * 0 + 2,
            19 * 0 + 1,
            19 * 1 + 0,
            19 * 2 + 2,
            19 * 2 + 1,
            19 * 1 + 3,
            19 * 1 + 2,
            # ko
            19 * 1 + 1,
        ]
        for i, val in enumerate(legal_moves):
            if i in illegal_indices:
                self.assertEqual(0, val, "{} should be illegal".format(i))
            else:
                self.assertEqual(1, val, "{} should be legal".format(i))


if __name__ == '__main__':
    unittest.main()
