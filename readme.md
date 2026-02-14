# Connect Four

Platform: windows.


As usual, the cmakelist was modified for mingw tool chain.

The game AI uses a humble representation of the Wikipedia's alphabeta pruning under the negamax page.

The program uses \_\_builtin\_popcount, a built-in function in GCC complier for counting the number of 1's in an int-style bit string.

A alternate version \_\_popcnt was provided for MSVC complier, but I don't know if it works since I don't have VSCode.

