#pragma once

#include "Board.hpp"
#include <time.h>
#include <cstdlib>

class Game {
    private:
    Board board;
    int randMoves; // Number of times we want to play initial random moves

    public:
    Game(Team team) : board(team), randMoves(3) {};
    void startGame();
};