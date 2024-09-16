#pragma once

#include "Piece.hpp"
#include "Board.hpp"
#include <SDL.h>

namespace GUIConstants {
    constexpr int windowHeight = 850;
    constexpr int windowWidth = 850;
    constexpr int tileOffset = 25;
    constexpr int winBgOffset = 200;
    constexpr int tileDimensions = 100;
    constexpr Uint8 overlayOpacity = 156;
}

class GUI {
    private:
    static SDL_Window* window;
    static SDL_Renderer* renderer;
    static SDL_Texture* bgSurface, *blackTile, *whiteTile, *overlay, *pieces, *bgWin, *winTeam, *whiteLetters, *blackLetters;

    static SDL_Rect findPiece(Team team, Type pieceType);
    static void drawPiece(int x, int y, Team team, Type pieceType);

    public:
    static void initialize();
    static Position evaluateClick(const SDL_Event& event, Team team);
    static void drawBackground();
    static void drawTiles(Team team);
    static void drawLetters(Team team);
    static void drawBoard(const Board& board);
    static void drawMoves(const Board& board, Position piecePos);
    static void drawWinner(Team winningTeam);
    static void onUpdate();
    static void exit();
};
