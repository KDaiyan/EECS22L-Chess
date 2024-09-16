#pragma once

#define EMPTY nullptr

enum class Team : int {
    WHITE = 0,
    BLACK = 1
};

enum class Type {
    KING, QUEEN, ROOK, KNIGHT, BISHOP, PAWN
};

struct Position {
    int rank, file;
    Position(int rank, int file);
    Position();
    
    bool operator==(const Position& other) const {
        return rank == other.rank && file == other.file;
    }
    bool operator!=(const Position& other) const {
        return rank != other.rank || file != other.file;
    }
};
static const Position INVALID_POS(-1,-1); 

struct Move {
    Position startPos, endPos;
    Move(Position start, Position end);
    Move();
    Move(const Move& move);

    bool operator==(const Move& other) const {
        return (startPos == other.startPos && endPos == other.endPos);
    }
};
static const Move INVALID_MOVE(INVALID_POS, INVALID_POS);

class Piece {
    private:
    Team team;
    Type pieceType;

    public:
    Piece(Type pieceType, Team team);
    Team getTeam() const;
    Type getType() const;
    void setType(Type type);
};