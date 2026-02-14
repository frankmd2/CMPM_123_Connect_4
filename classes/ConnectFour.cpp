#include "ConnectFour.h"
#include <cstdint>
#ifdef _MSC_VER
#  include <intrin.h>
#  define __builtin_popcount __popcnt
#endif
//
// Frank Zeng
//
// Bitboard layout:
//
// 35 36 37 38 39 40 41
// 28 29 30 31 32 33 34
// 21 22 23 24 25 26 27
// 14 15 16 17 18 19 20
// 7  8  9  10 11 12 13
// 0  1  2  3  4  5  6
//


ConnectFour::ConnectFour() {
    _grid = new Grid(7, 6);
}

ConnectFour::~ConnectFour() {
    delete _grid;
}

Bit *ConnectFour::PieceForPlayer(const int playerNumber) {
    Bit *bit = new Bit();

    bit->LoadTextureFromFile(playerNumber == AI_PLAYER ? "red.png" : "yellow.png");
    bit->setOwner(getPlayerAt(playerNumber == AI_PLAYER ? 1 : 0));
    return bit;
}

void ConnectFour::setUpBoard() {
    setNumberOfPlayers(2);
    _gameOptions.rowX = 7;
    _gameOptions.rowY = 6;
    _bitboard[0] = 0;
    _bitboard[1] = 0;
    _grid->initializeSquares(80, "square.png");

    if (gameHasAI()) {
        setAIPlayer(AI_PLAYER);
    }

    startGame();
}

bool ConnectFour::actionForEmptyHolder(BitHolder &holder)
{
    auto it = holder.getPosition();
    int x = (it.x - 40) / 80;
    int y = 0;
    if (_grid->getSquare(x, y)->bit() != nullptr) return false;
    while (y < 5) {
        if (_grid->getSquare(x, y + 1)->bit() == nullptr) y++;
        else break;
    }
    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber() == 0 ? HUMAN_PLAYER : AI_PLAYER);
    if (bit) {
        ChessSquare *actualHolder = _grid->getSquare(x, y);
        bit->setPosition(actualHolder->getPosition());
        actualHolder->setBit(bit);
        _bitboard[getCurrentPlayer()->playerNumber()] |= (1ULL << ((5-y) * 7 + x));
        //std::cout << AIBoardEval(_bitboard[0], _bitboard[1]) << std::endl;

        endTurn();
        return true;
    }
    return false;
}

bool ConnectFour::canBitMoveFrom(Bit &bit, BitHolder &src) {
    return false;
}

bool ConnectFour::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
    return false;
}

void ConnectFour::stopGame() {
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

Player* ConnectFour::ownerAt(int index ) const {
    auto square = _grid->getSquare(index % 7, index / 6);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

Player *ConnectFour::checkForWinner() {
    int height = 6;
    for (int i = 0; i < 2; i ++) {
        // horizontal
        uint64_t temp = _bitboard[i] & (_bitboard[i] >> (height + 1));
        if(temp & (temp >> (2 * (height + 1)))) return getPlayerAt(i);

        // diagonal 1
        temp = _bitboard[i] & (_bitboard[i] >> height);
        if(temp & (temp >> (2 * height))) return getPlayerAt(i);

        // diagonal 2
        temp = _bitboard[i] & (_bitboard[i] >> (height + 2));
        if(temp & (temp >> (2 * (height + 2)))) return getPlayerAt(i);

        // vertical;
        temp = _bitboard[i] & (_bitboard[i] >> 1);
        if(temp & (temp >> 2)) return getPlayerAt(i);
    }
    return nullptr;
}

bool ConnectFour::checkForDraw() {
    // we only need to check if the top row is full
    for (int x = 0; x < 7; x++) {
        if (_grid->getSquare(x, 0)->bit() == nullptr) {
            return false;
        }
    }
    return true;
}

std::string ConnectFour::initialStateString() {
    return {42, '0'};
}

std::string ConnectFour::stateString() {
    std::string s(42, '0');
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        Bit *bit = square->bit();
        if (bit) {
            s[y * 7 + x] = std::to_string(bit->getOwner()->playerNumber()+1)[0];
        }
    });
    return s;
}


void ConnectFour::setStateString(const std::string &s) {
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y*7 + x;
        int playerNumber = s[index] - '0';
        if (playerNumber) {
            square->setBit( PieceForPlayer(playerNumber-1) );
        } else {
            square->setBit( nullptr );
        }
    });
}

void ConnectFour::printBoardFromBitBoard(uint64_t &board1, uint64_t &board2) {
    for (int y = 5; y >= 0; y++) {
        std::string s;
        for (int x = 0; x < 7; x++) {
            if (board1 & (1ULL << (y * 7 + x))) {
                s += "X";
            } else if (board2 & (1ULL << (y * 7 + x))) {
                s += "O";
            } else {
                s += ".";
            }
        }
        std::cout << s << std::endl;
    }
    std::cout << std::endl;
}

int ConnectFour::AIBoardEval(uint64_t &myBoard, uint64_t &opponentBoard) {
    // +1 for each 2 in a row, +10 for each 3 in a row, +100 for each 4 in a row
    int HEIGHT = 6;
    int score = 0;

    // horizontal
    for (int i = 0; i < 2; i ++) {
        uint64_t board;
        if (i == 0) board = myBoard;
        else board = opponentBoard;

        // horizontal
        uint64_t temp = board & (board >> (HEIGHT + 1));
        score += 1 * __builtin_popcountll(temp);
        score += 10 * __builtin_popcountll(temp & (temp >> (HEIGHT + 1)));
        score += 100 * __builtin_popcountll(temp & (temp >> 2 * (HEIGHT + 1)));

        // diagonal 1
        temp = board & (board >> HEIGHT);
        score += 1 * __builtin_popcountll(temp);
        score += 10 * __builtin_popcountll(temp & (temp >> HEIGHT));
        score += 100 * __builtin_popcountll(temp & (temp >> 2 * HEIGHT));

        // diagonal 2
        temp = board & (board >> (HEIGHT + 2));
        score += 1 * __builtin_popcountll(temp);
        score += 10 * __builtin_popcountll(temp & (temp >> (HEIGHT + 2)));
        score += 100 * __builtin_popcountll(temp & (temp >> (2 * (HEIGHT + 2))));

        // vertical;
        temp = board & (board >> 1);
        score += 1 * __builtin_popcountll(temp);
        score += 10 * __builtin_popcountll(temp & (temp >> 1));
        score += 100 * __builtin_popcountll(temp & (temp >> 2));

        score = -score;
    }
    return score;
}

int ConnectFour::alphaBeta(uint64_t &myBoard, uint64_t &opponentBoard, int depth, int alpha, int beta) {

    int eval = AIBoardEval(myBoard, opponentBoard);

    // if the game is over, or we've reached the maximum depth, return the evaluation of the board
    if (depth == 0 || eval >= 100 || eval <= -100) {
        return AIBoardEval(myBoard, opponentBoard);
    }

    uint64_t occupationBoard = myBoard | opponentBoard;
    // draw
    if (occupationBoard == 0x3FFFFFFFFFFFF) {
        return 0;
    }

    int score = -10000;
    for (int x = 0; x < 7; x ++) {
        if (occupationBoard & (1ULL << (5 * 7 + x))) continue;
        uint64_t myNewBoard = myBoard;
        int y = 5;
        while (y > 0) {
            if (occupationBoard & (1ULL << ((y - 1) * 7 + x))) break;
            y --;
        }
        myNewBoard |= (1ULL << (y * 7 + x));

        score = std::max(score, -alphaBeta(opponentBoard, myNewBoard, depth - 1, -beta, -alpha));
        alpha = std::max(alpha, score);
        if (alpha >= beta) break;
    }
    return score;
}

void ConnectFour::updateAI() {
    int move = -1;
    int bestScore = -10000;
    int score = -10000;
    uint64_t occupationBoard = _bitboard[0] | _bitboard[1];
    for (int x = 0; x < 7; x ++) {
        if (occupationBoard & (1ULL << (5 * 7 + x))) continue;
        uint64_t myNewBoard = _bitboard[AI_PLAYER];
        int y = 5;
        while (y > 0) {
            if (occupationBoard & (1ULL << ((y - 1) * 7 + x))) break;
            y --;
        }
        myNewBoard |= (1ULL << (y * 7 + x));

        score = std::max(score, -alphaBeta(_bitboard[1 - AI_PLAYER], myNewBoard, 8, -10000, 10000));
        if (score > bestScore) {
            bestScore = score;
            move = x;
        }
    }
    if (move != -1) {
        actionForEmptyHolder(*_grid->getSquare(move, 0));
    }
}








