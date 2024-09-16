#pragma once

#include "Board.hpp"
#include "Game.hpp"
#include <vector>

namespace Check {
    bool canMoveToSpot(const Board& board, Position startPos, Position endPos);
    bool isCheckMate(const Board& board);
    std::vector<Move> genAllMoves(const Board& board, Team team);
    std::vector<Move> genAllSafeMoves(const Board& board, Team team);
};