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

print('#ifndef incl_BADUK_ZOBRIST_CODES_H__')
print('#define incl_BADUK_ZOBRIST_CODES_H__')
print('')
print('#include <unordered_map>')
print('')
print('#include "../point.h"')
print('')
print('namespace baduk {')
print('namespace zobrist {')
print('using hashcode = unsigned long long;')
print('const std::unordered_map<::baduk::Point, hashcode> EMPTY_HASH_CODE{');
for (pt, state), hash_code in table.items():
    if state is not None:
        continue
    print('    {::baduk::Point(%d, %d), %dLL},' % (pt.row, pt.col, hash_code))
print('};')
print('const std::unordered_map<::baduk::Point, hashcode> BLACK_HASH_CODE{');
for (pt, state), hash_code in table.items():
    if state != black:
        continue
    print('    {::baduk::Point(%d, %d), %dLL},' % (pt.row, pt.col, hash_code))
print('};')
print('const std::unordered_map<::baduk::Point, hashcode> WHITE_HASH_CODE{');
for (pt, state), hash_code in table.items():
    if state != white:
        continue
    print('    {::baduk::Point(%d, %d), %dLL},' % (pt.row, pt.col, hash_code))
print('};')
print('const hashcode EMPTY_BOARD = %dLL;' % (empty_board,))
print('const hashcode BLACK_TO_PLAY = %dLL;' % (black_to_play,))
print('const hashcode WHITE_TO_PLAY = %dLL;' % (white_to_play,))
print('}')
print('}')
print('')
print('#endif')
# end::generate_zobrist[]
