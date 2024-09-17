#include "Board.hpp"
#include "Check.hpp"
#include "CheckUtils.hpp"
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <cassert>

// Initialize all the pieces of the board
Board::Board(Team team) : currentTeamTurn(Team::WHITE) {
    initializeMap();

    constexpr Type pieceLayout[] = {Type::ROOK, Type::KNIGHT, Type::BISHOP, Type::QUEEN,
                                    Type::KING, Type::BISHOP, Type::KNIGHT, Type::ROOK};
    for(size_t i{}; i < 8; i++) {
        // Pawns
        grid[1][i] = std::make_unique<Piece>(Type::PAWN, Team::BLACK);
        grid[6][i] = std::make_unique<Piece>(Type::PAWN, Team::WHITE);
        // Other pieces
        grid[0][i] = std::make_unique<Piece>(pieceLayout[i], Team::BLACK);
        grid[7][i] = std::make_unique<Piece>(pieceLayout[i], Team::WHITE);
    }
}

bool Board::movePiece(Position startPos, Position endPos) {
    // Flip all positions if its Black
    if(currentTeamTurn == Team::BLACK) {
        startPos.rank = 7 - startPos.rank;
        startPos.file = 7 - startPos.file;
        endPos.rank = 7 - endPos.rank;
        endPos.file = 7 - endPos.file;
    }

    Piece* piece = grid[startPos.rank][startPos.file].get();
    if(piece == EMPTY || piece->getTeam() != currentTeamTurn || !Check::canMoveToSpot(*this, startPos, endPos)) return false;
    
    // Perform castling move and return early
    if(checkUtils::isCastlingMove(*this, startPos, endPos)) {
        return checkUtils::canCastle(*this, startPos, endPos) ? (checkUtils::performCastle(*this, startPos, endPos), true) : false;
    }

    // Mark if rook or king moves to prevent double castling
    if(piece->getType() == Type::KING || piece->getType() == Type::ROOK) {
       checkUtils::castlingMark(*this, startPos);
    }

    // Perform Move
    checkUtils::shiftPiece(*this, startPos, endPos);

    // Promote all pawns if there are any
    checkUtils::pawnPromotion(*this);

    return true;
}

void Board::rotateBoard() {
    for(size_t i{}; i < 4; i++) 
        for (size_t j{}; j < 8; j++) {
            std::swap(grid[i][j], grid[7 - i][j]);
        }

    for(size_t i{}; i < 8; i++) 
        for (size_t j{}; j < 4; j++) {
            std::swap(grid[i][j], grid[i][7 - j]);
        }
}

void Board::changeTurns() {
    currentTeamTurn = currentTeamTurn == Team::WHITE ? Team::BLACK : Team::WHITE;
}

Board::Board(const Board& board) : currentTeamTurn(board.getCurrentTurn()), castlingCheck(board.castlingCheck) {
    for(size_t i{}; i < 8; i++) {
        for(size_t j{}; j < 8; j++) {
            if (board.grid[i][j] != EMPTY) {
                (this->grid)[i][j] = std::make_unique<Piece>(*board.grid[i][j]);
            } else {
                (this->grid)[i][j] = EMPTY;
            }
        }
    }
}

Team Board::getCurrentTurn() const {
    return currentTeamTurn;
}

void Board::initializeMap() {
    castlingMap["whiteLeftRook"] = &castlingCheck.whiteLeftRook;
    castlingMap["whiteRightRook"] = &castlingCheck.whiteRightRook;
    castlingMap["whiteKing"] = &castlingCheck.whiteKing;
    castlingMap["blackLeftRook"] = &castlingCheck.blackLeftRook;
    castlingMap["blackRightRook"] = &castlingCheck.blackRightRook;
    castlingMap["blackKing"] = &castlingCheck.blackKing;
}

void Board::setMoved(const std::string& piece) {
    auto it = castlingMap.find(piece);
    if(it != castlingMap.end()) {
        *(it->second) = true;
    }
}
