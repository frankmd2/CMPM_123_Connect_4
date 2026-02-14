#pragma once
#include "Game.h"

//
// the classic game of connect four
//

class ConnectFour : public Game
{
public:
    ConnectFour();
    ~ConnectFour();

    void        setUpBoard() override;

    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;

    void        setStateString(const std::string &s) override;

    bool        actionForEmptyHolder(BitHolder &holder) override;
    bool        canBitMoveFrom(Bit &bit, BitHolder &src) override;
    bool        canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    void        stopGame() override;

    void        updateAI() override;
    bool        gameHasAI() override { return true; }
    Grid*       getGrid() override { return _grid; }

private:
    Bit *       PieceForPlayer(const int playerNumber);
    Player*     ownerAt(int index ) const;

    void        printBoardFromBitBoard(uint64_t &board1, uint64_t &board2);
    int         AIBoardEval(uint64_t &myBoard, uint64_t &opponentBoard);
    int         alphaBeta(uint64_t &myBoard, uint64_t &opponentBoard, int depth, int alpha, int beta);

    Grid*       _grid;
    uint64_t    _bitboard[2];
};
