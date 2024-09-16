#include "Piece.hpp"

Team Piece::getTeam() const {
    return team;
}

Type Piece::getType() const {
    return pieceType;
}

void Piece::setType(Type type) {
    this->pieceType = type;
}

Piece::Piece(Type pt, Team tm) : pieceType(pt), team(tm) {}

Position::Position(int r, int f)  : rank(r), file(f) {}

Position::Position() : rank(-1), file(-1) {}

Move::Move(Position start, Position end) : startPos(start), endPos(end) {}

Move::Move() : startPos(INVALID_POS), endPos(INVALID_POS) {}

Move::Move(const Move& move) : startPos(move.startPos), endPos(move.endPos) {}