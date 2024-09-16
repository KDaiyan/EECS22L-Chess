#pragma once

#include "Board.hpp"
#include "Game.hpp"
#include "Check.hpp"
#include "CheckUtils.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <limits>

class AI {
    private:
    struct treeNode {
        Board board;
        Move move;
        int score;
        std::vector<std::unique_ptr<treeNode>> children;
        treeNode(Board newBoard, Move newMove) : board(newBoard),  move (newMove), score(0) {}
    };

    static int evaluateBoard(const Board& board, const Move& move);
    static int minMax(const std::unique_ptr<treeNode>& node, int depth, Team isMaximizingPlayer, int alpha, int beta);
    static int getPieceValue(Piece& piece);
    static void generateTree(const std::unique_ptr<treeNode>& node, int depth, Team isMaximizingPlayer);

    public:
    static Move genAIMove(const Board& board, int depth, Team team);
    static Move genRandomMove(const Board& board, Team team);
};
