#pragma once

#include "Piece.hpp"
#include <memory>
#include <string>
#include <unordered_map>

class Board {
    private:
    Team currentTeamTurn;

    struct castlingBool {
        bool whiteLeftRook, whiteRightRook, whiteKing,
             blackLeftRook, blackRightRook, blackKing;
        castlingBool() 
        : whiteLeftRook(false), whiteRightRook(false), whiteKing(false),
          blackLeftRook(false), blackRightRook(false), blackKing(false) {}
    } castlingCheck;

    void initializeMap();

    struct Row {
        std::unique_ptr<Piece>* row;
        Piece* operator[](int col) {return row[col].get();} // uses array notation on row pointer to retreive specific element
        Piece* operator[](int col) const {return row[col].get();}
    };

    public:
    std::unordered_map<std::string, bool*> castlingMap;
    std::unique_ptr<Piece> grid[8][8];
    Row operator[](int row) {return Row{grid[row]};} // initializes row in Row class to grid[row], which is a piece* (recall grid is a piece**)
    const Row operator[](int row) const {return Row{const_cast<std::unique_ptr<Piece>*>(grid[row])};}

    Board(Team team);
    Board(const Board& board);
    void setMoved(const std::string& piece);
    void rotateBoard();
    bool movePiece(Position start, Position end);
    void changeTurns();
    Team getCurrentTurn() const;
};
