#include "ConnectFour.h"
#include <cstdint>
//
// Frank Zeng
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
    std::string state = stateString();
    
}

bool ConnectFour::checkForDraw() {
    bool isDraw = true;
    // check to see if the board is full
    _grid->forEachSquare([&isDraw](ChessSquare* square, int x, int y) {
        if (!square->bit()) {
            isDraw = false;
        }
    });
    return isDraw;
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

void ConnectFour::updateAI() {

}








