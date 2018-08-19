import unittest

from baduk import Board, GameState, Move, Player, Point, remove_dead_stones


def board_from_string(board_string):
    rows = board_string.strip().split("\n")
    assert len(rows) == len(rows[0])
    dim = len(rows)
    board = Board(dim, dim)
    for r, row_str in enumerate(rows):
        row_str = row_str.strip()
        for c, stone in enumerate(row_str):
            int_row = dim - r
            point = Point(int_row, c + 1)
            if stone == 'x':
                board.place_stone(Player.black, point)
            elif stone == 'o':
                board.place_stone(Player.white, point)
    return board


class ScoringTest(unittest.TestCase):
    def test_easy_scoring(self):
        # Both B and W have an unconditionally alive group. One
        # unconditionally dead W stone.
        orig_board = board_from_string('''
            ..xo.
            xxxo.
            .oxo.
            xxxoo
            ..xo.
        ''')
        final_board = board_from_string('''
            ..xo.
            xxxo.
            ..xo.
            xxxoo
            ..xo.
        ''')
        game = GameState.from_board(orig_board, Player.white, komi=7.5)

        board = remove_dead_stones(game)
        self.assertEqual(final_board, board)

    def test_large_dead_dragon(self):
        orig_board = board_from_string('''
            oxx...xoo...oox....
            ooxx..xxo.o.oxx..x.
            .oox..xoooxxooxx..x
            ..oxxxxxxxoooooxxxo
            .oxxo.x.x.xo.xooxoo
            ..oooxxooxxoxxxooo.
            ..oxxoooxx.x...xo..
            .oxxxo.xox.xxxxxoo.
            .ox.xxo.oxxooxoox..
            ..oxxooooxxo.o.oo..
            ..oxxxoxxxo...o.xo.
            ..oxxoooxo.o.oxxxo.
            .oxxooooxooxoo.xooo
            ..oxxoxxoo.xxxxxxo.
            ..oxxoxxo.oxx..xo..
            ..ooxxxooo.oox.xoo.
            ....oxxxoo..ox.xo..
            ..o.ox.xxo..ooxxxoo
            ...oxxooxo..oxxxxxo
        ''')
        # White has a large dragon that is dead (black stone on the
        # nakade point)
        expected_board = board_from_string('''
            oxx...xoo...oox....
            ooxx..xxo.o.oxx..x.
            .oox..xooo..ooxx..x
            ..oxxxxxxxoooooxxxo
            .oxxo.x.x.xo.xooxoo
            ..oooxx..xxoxxxooo.
            ..oxx...xx.x...xo..
            .oxxx..x.x.xxxxxoo.
            .ox.xx...xxooxoo...
            ..oxx....xxo.o.oo..
            ..oxxx.xxxo...o.xo.
            ..oxx...xo.o.oxxxo.
            .oxx....xooxoo.xooo
            ..oxx.xxoo.xxxxxxo.
            ..oxx.xxo.oxx..xo..
            ..ooxxxooo.oox.xoo.
            ....oxxxoo..ox.xo..
            ..o.ox.xxo..ooxxxoo
            ...oxx..xo..oxxxxxo
        ''')

        game = GameState.from_board(orig_board, Player.white, komi=7.5)
        final_board = remove_dead_stones(game)

        self.assertEqual(final_board, expected_board)


if __name__ == '__main__':
    unittest.main()
