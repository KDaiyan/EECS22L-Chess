#include "AI.hpp"
#include <iostream>

// Switch statement is faster than map for short cases and we need performance here
int AI::getPieceValue(Piece& piece) {
    switch (piece.getType()) {
        case Type::PAWN:   return 1;
        case Type::KNIGHT: return 3;
        case Type::BISHOP: return 3;
        case Type::ROOK:   return 5;
        case Type::QUEEN:  return 9;
        case Type::KING:   return 100;
        default: return 0;
    }
}

// We are in a state where the move has already happened
int AI::evaluateBoard(const Board& board, const Move& move) {
    // Send extremely low value if white loses, or extremely high score if black loses
    if(move == INVALID_MOVE) {
        Team losingTeam = board.getCurrentTurn();
        return losingTeam == Team::WHITE ?  std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
    }
    Team currentTeam = board[move.endPos.rank][move.endPos.file]->getTeam();
    int value, score = 0;
    for(size_t i{}; i < 8; i++) {
        for(size_t j{}; j < 8; j++) {
            if(board[i][j] != BLANK) {
                value = getPieceValue(*(board[i][j]));
                score += board[i][j]->getTeam() == currentTeam ? value : (-1 * value);
            }
        }
    }
    return score;
}

// Primary Min-Max algorithm
int AI::minMax(const std::unique_ptr<treeNode>& node, int depth, Team isMaximizingPlayer, int alpha, int beta) {
    if (depth == 0 || node->children.empty()) {
        return evaluateBoard(node->board, node->move);  // Leaf node: evaluate board
    }

    // Max Team is White
    if (isMaximizingPlayer == Team::WHITE) {
        int maxEval = std::numeric_limits<int>::min();
        for (const auto& child : node->children) {
            int eval = minMax(child, depth - 1, Team::BLACK, alpha, beta);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if(beta <= alpha) break;
        }
        node->score = maxEval;
        return maxEval;
    } // Max Team is Black
    else {
        int minEval = std::numeric_limits<int>::max();
        for (const auto& child : node->children) {
            int eval = minMax(child, depth - 1, Team::WHITE, alpha, beta);
            minEval = std::min(minEval, eval);
            beta = std::min(minEval, eval);
            if(beta <= alpha) break;
        }
        node->score = minEval;
        return minEval;
    }
}

// Generate entire tree based on all possible moves
void AI::generateTree(const std::unique_ptr<treeNode>& node, int depth, Team isMaximizingPlayer) {
    if (depth == 0) return;

    std::vector<Move> possibleMoves = Check::genAllSafeMoves(node->board, isMaximizingPlayer);

    for (const Move& move : possibleMoves) {
        if(move.endPos == INVALID_POS || move.startPos == INVALID_POS) continue;
        Board newBoard = node->board;
        checkUtils::shiftPiece(newBoard, move.startPos, move.endPos);

        std::unique_ptr<treeNode> child = std::make_unique<treeNode>(newBoard, move);
        node->children.push_back(std::move(child));

        generateTree(node->children.back(), depth - 1, isMaximizingPlayer == Team::BLACK ? Team::WHITE : Team::BLACK);
    }
}

Move AI::genAIMove(const Board& board, int depth, Team team) {
    std::unique_ptr<treeNode> root = std::make_unique<treeNode>(board, Move());
    generateTree(root, depth, team);

    int bestScore = minMax(root, depth, team, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    std::vector<Move> bestMoves;
    Move bestMove;

    if (bestScore == std::numeric_limits<int>::max()) {
        checkUtils::assertWinner(Team::WHITE);
        return(Move());
    } else if (bestScore == std::numeric_limits<int>::min()) {
        checkUtils::assertWinner(Team::BLACK);
        return(Move());
    } 

    for(const auto& child : root->children) {
        if(child->score == bestScore) {
            bestMoves.push_back(child->move);
            break;
        }
    }

    if(!bestMoves.empty()) {
        bestMove = bestMoves[rand() % bestMoves.size()];
    }

    return bestMove;
}

Move AI::genRandomMove(const Board& board, Team team) {
    std::vector<Move> moves = Check::genAllSafeMoves(board, team);
    return moves.at(rand() % moves.size());
}
