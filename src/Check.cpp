#include "Check.hpp"
#include "CheckUtils.hpp"
#include <iostream>
#include <functional>

// Determines whether a piece can move to a certain position. Performs theoretical move to ensure no check.
bool Check::canMoveToSpot(const Board& board, Position startPos, Position endPos) {
    using namespace checkUtils;

    bool isValidMove = false;
    auto it = canMoveFunctions.find(board[startPos.rank][startPos.file]->getType());
    if(it != canMoveFunctions.end()) {
        isValidMove = it->second(board, startPos, endPos);
    }

    // If its a valid move and we perform the move, will the King be in check
    if(isValidMove && (isCastlingMove(board, startPos, endPos) || isKingSafe(board, startPos, endPos))) {
        return true;
    }
    return false;
}

// Determines if either team has been checkmated
bool Check::isCheckMate(const Board& board) {
    using namespace checkUtils;

    Team teams[] = {Team::WHITE, Team::BLACK};
    for(Team team : teams) {
        bool isCheckMate = true;
        Position kingPos = locateKing(board, team);
        if(!isKingInCheck(board, kingPos, team)) continue;

        // If any move results in King being safe, its not a checkmate
        std::vector<Move> moves = genAllMoves(board, team);
        for(auto move : moves) {
            if(isKingSafe(board, move.startPos, move.endPos)) {
                isCheckMate = false;
                break;
            }
        }
        
        if(isCheckMate) {
            return true;
        }
    }
    return false;
}

// Generates all moves for a specific team. Does not check for King safety
std::vector<Move> Check::genAllMoves(const Board& board, Team team) {
    using namespace checkUtils;

    std::vector<Move> moves;
    std::unordered_map<Type, genMoveFunction>::iterator it;

    for(size_t i{}; i < 8; i++) {
        for(size_t j{}; j < 8; j++) {
            if(board[j][i] == EMPTY || board[j][i]->getTeam() != team) continue;

            it = genMoveFunctions.find(board.grid[j][i].get()->getType());
            if(it != genMoveFunctions.end()) {
                it->second(board, Position(j, i), moves);
            }
        }
    }
    return moves;
}

// Generates all safe moves. Slower than normal gen all moves
std::vector<Move> Check::genAllSafeMoves(const Board& board, Team team) {
    std::vector<Move> moves = Check::genAllMoves(board, team);

    auto it = moves.begin();
    while(it != moves.end()) {
        if(!Check::canMoveToSpot(board, it->startPos, it->endPos)) {
            it = moves.erase(it);
        } else {
            ++it;
        }
    }
    return moves;
}