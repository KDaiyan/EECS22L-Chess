#include "Game.hpp"
#include "Check.hpp"
#include "AI.hpp"
#include "GUI.hpp"
#include <iostream>
#include <queue>

// Primary Game Loop
void Game::startGame() {
    GUI::initialize();
    GUI::drawBoard(this->board.getCurrentTurn());
    GUI::onUpdate();

    std::queue<Position> moveQueue;
    SDL_Event event;
    bool selected = false;
    srand(static_cast<int>(time(0)));

    // This should check for a win condition
    while(!Check::isCheckMate(board)) {

        bool status = true;
        while(status) {
            while(SDL_PollEvent(&event)) {
                switch(event.type) {
                    case SDL_QUIT:
                        return;
                    case SDL_MOUSEBUTTONDOWN:
                        Position boardPos = GUI::evaluateClick(event, this->board.getCurrentTurn());
                        if(boardPos == INVALID_POS) {
                            continue;
                        } else {
                            status = false;
                            moveQueue.push(boardPos);
                            break;
                        }
                }
            }
        }


        if(moveQueue.size() == 1) {
            Position pos = moveQueue.front();
            if(board[pos.rank][pos.file] == EMPTY || board[pos.rank][pos.file]->getTeam() != board.getCurrentTurn()) {
                moveQueue.pop();
                GUI::drawBoard(board);
                GUI::onUpdate();
                continue;
            }
            if(board.getCurrentTurn() == Team::BLACK) {
                pos.file = 7 - pos.file;
                pos.rank = 7 - pos.rank;
            }
            if(board[pos.rank][pos.file] != EMPTY && board.getCurrentTurn() == board[pos.rank][pos.file]->getTeam()) {
                GUI::drawMoves(board, pos);
                GUI::onUpdate();
            }
        } else if(moveQueue.size() == 2) {
            Position startPos = moveQueue.front();
            Position endPos = moveQueue.back();
            while (!moveQueue.empty()) moveQueue.pop();

            if(!board.movePiece(startPos, endPos)) {
                while (!moveQueue.empty()) moveQueue.pop();
                GUI::drawBoard(this->board);
                GUI::onUpdate();
                continue;
            }

            GUI::drawBoard(this->board);
            GUI::onUpdate();

            board.changeTurns();
            board.rotateBoard();

            Move bestMove;
            if(randMoves-- <= 0) {
                bestMove = AI::genAIMove(board, 2, board.getCurrentTurn());
            } else {
                bestMove = AI::genRandomMove(board, board.getCurrentTurn());
            }

            if(bestMove == INVALID_MOVE) break;
            checkUtils::shiftPiece(board, bestMove.startPos, bestMove.endPos);
            checkUtils::pawnPromotion(board);

            board.changeTurns();
            board.rotateBoard();

            GUI::drawBoard(this->board);
            GUI::onUpdate();
        }
    }
    GUI::exit();
}
