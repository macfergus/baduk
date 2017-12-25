import random
from collections import namedtuple

class Point(namedtuple('Point', 'row col')):
    pass

black = 1
white = 2

MAX63 = 0x7fffffffffffffff

table = {}
empty_board = 0
for row in range(25):
    for col in range(25):
        for state in (None, black, white):
            code = random.randint(0, MAX63)
            table[Point(row, col), state] = code
            if state is None:
                empty_board ^= code
black_to_play = random.randint(0, MAX63)
white_to_play = random.randint(0, MAX63)

print('#include "zobrist.h"')
print('')
print('namespace baduk {')
print('namespace zobrist {')
print('')
print('void ZobristCodes::initialize() {')
print('    empty_codes_.resize(25 * 25);')
for (pt, state), hash_code in table.items():
    if state is not None:
        continue
    print('    empty_codes_[index(::baduk::Point(%d, %d))] = %dLL;' % (
        pt.row, pt.col, hash_code))
print('    black_codes_.resize(25 * 25);')
for (pt, state), hash_code in table.items():
    if state != black:
        continue
    print('    black_codes_[index(::baduk::Point(%d, %d))] = %dLL;' % (
        pt.row, pt.col, hash_code))
print('    white_codes_.resize(25 * 25);')
for (pt, state), hash_code in table.items():
    if state != white:
        continue
    print('    white_codes_[index(::baduk::Point(%d, %d))] = %dLL;' % (
        pt.row, pt.col, hash_code))
print('    empty_board_ = %dLL;' % (empty_board,));
print('}')
print('')
print('}')
print('}')
