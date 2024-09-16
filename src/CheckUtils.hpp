#pragma once

#include "Board.hpp"
#include "Game.hpp"
#include "Check.hpp"
#include "GUI.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

enum class slideType {
    Rook, Bishop, Queen
};

namespace checkUtils {

using canMoveFunction = std::function<bool(const Board&, Position, Position)>;
using genMoveFunction = std::function<void(const Board&, Position, std::vector<Move>&)>;

// Indexing arrays for Knight moves
static const Position knightOffsets[] = {Position(2, 1), Position(1, 2), Position(2, -1), Position(1, -2),
                                         Position(-2, 1), Position(-1, 2), Position(-2, -1), Position(-1, -2)};

// Rook(0-3), bishop(4-7), and Queen(0-7)
static const Position slidingOffsets[] = {Position(0, 1), Position(0, -1), Position(1, 0), Position(-1, 0),
                                          Position(1, 1), Position(-1, 1), Position(1, -1), Position(-1, -1)}; 

// Single-Step(0), Double-Step(1), and Captures(2-3)
static const Position pawnOffsets[] = {Position(-1, 0), Position(-2, 0), Position(-1, 1), Position(-1, -1)};


// Move Checks
bool canMoveKing(const Board& board, Position startPos, Position endPos);
bool canMoveQueen(const Board& board, Position startPos, Position endPos);
bool canMoveRook(const Board& board, Position startPos, Position endPos);
bool canMoveKnight(const Board& board, Position startPos, Position endPos);
bool canMoveBishop(const Board& board, Position startPos, Position endPos);
bool canMovePawn(const Board& board, Position startPos, Position endPos);
bool checkBounds(const Board& board, Position startPos, Position endPos);
bool checkDiags(Position startPos, Position endPos);
bool checkAxially(Position startPos, Position endPos);
bool checkSliding(const Board& board, Position startPos, Position endPos, slideType type);
bool isOneTile(Position startPos, Position endPos);

// Move Generation
void genMovesKing(const Board& board, Position startPos, std::vector<Move>& moves);
void genMovesQueen(const Board& board, Position startPos, std::vector<Move>& moves);
void genMovesRook(const Board& board, Position startPos, std::vector<Move>& moves);
void genMovesKnight(const Board& board, Position startPos, std::vector<Move>& moves);
void genMovesBishop(const Board& board, Position startPos, std::vector<Move>& moves);
void genMovesPawn(const Board& board, Position startPos, std::vector<Move>& moves);
void genSliding(const Board& board, Position startPos, slideType type, std::vector<Move>& moves);

// Misc Functions
bool isKingInCheck(const Board& board, Position kingPos, Team team);
Position locateKing(const Board& board, Team team);
bool isKingSafe(const Board& board, Position startPos, Position endPos);
void castlingMark(Board& board, Position startPos);
bool canCastle(const Board& board, Position startPos, Position endPos);
std::string pieceToString(const Board& board, Position pos);
void performCastle(Board& board, Position startPos, Position endPos);
void shiftPiece(Board& board, Position startPos, Position endPos);
bool isCastlingMove(const Board& board, Position startPos, Position endPos);
void pawnPromotion(Board& board);
void assertWinner(const Team winningTeam);

static std::unordered_map<Type, canMoveFunction> canMoveFunctions = {
    {Type::KING, canMoveKing},
    {Type::QUEEN, canMoveQueen},
    {Type::ROOK, canMoveRook},
    {Type::KNIGHT, canMoveKnight},
    {Type::BISHOP, canMoveBishop},
    {Type::PAWN, canMovePawn}};

static std::unordered_map<Type, genMoveFunction> genMoveFunctions = {
    {Type::KING, genMovesKing},
    {Type::QUEEN, genMovesQueen},
    {Type::ROOK, genMovesRook},
    {Type::KNIGHT, genMovesKnight},
    {Type::BISHOP, genMovesBishop},
    {Type::PAWN, genMovesPawn}};
}