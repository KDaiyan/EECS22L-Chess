#include "GUI.hpp"
#include "CheckUtils.hpp"
#include "Check.hpp"
#include <iostream>

SDL_Window* GUI::window = nullptr;
SDL_Renderer* GUI::renderer = nullptr;
SDL_Texture* GUI::bgSurface = nullptr;
SDL_Texture* GUI::blackTile = nullptr;
SDL_Texture* GUI::whiteTile = nullptr;
SDL_Texture* GUI::overlay = nullptr;
SDL_Texture* GUI::pieces = nullptr;
SDL_Texture* GUI::bgWin = nullptr;
SDL_Texture* GUI::winTeam = nullptr;
SDL_Texture* GUI::whiteLetters = nullptr;
SDL_Texture* GUI::blackLetters = nullptr;

// Initializes all SDL elements
void GUI::initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return;
    }

    window = SDL_CreateWindow("EECS 22L Chess Remake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, GUIConstants::windowWidth, GUIConstants::windowHeight, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }
	
    bgSurface = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("../assets/Background.bmp"));
    blackTile = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("../assets/Black-Tile.bmp"));
    whiteTile = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("../assets/White-Tile.bmp"));
    pieces = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("../assets/Pieces.bmp"));
    bgWin = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("../assets/Win-Screen.bmp"));
    whiteLetters = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("../assets/white-letters.bmp"));
    blackLetters = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("../assets/black-letters.bmp"));
    overlay = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("../assets/Overlay.bmp"));
    SDL_SetTextureBlendMode(overlay, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(overlay, GUIConstants::overlayOpacity);

    if(!bgSurface || !blackTile || !whiteTile || !pieces || !overlay || !bgWin || !blackLetters || !whiteLetters) {
        perror("Error loading assets!\n");
    }
}

// Returns a SDL_Rect based on where the piece is located in the image
SDL_Rect GUI::findPiece(Team team, Type pieceType) {
    SDL_Rect pieceRect;
    pieceRect.w = GUIConstants::tileDimensions;
    pieceRect.h = GUIConstants::tileDimensions;

    // Locate Y-Coord
    pieceRect.y = static_cast<int>(team) * GUIConstants::tileDimensions;

    // Locate X-Coord
    switch (pieceType) {
        case Type::KING: pieceRect.x = 0; break;
        case Type::QUEEN: pieceRect.x = 100; break;
        case Type::BISHOP: pieceRect.x = 200; break;
        case Type::KNIGHT: pieceRect.x = 304; break;
        case Type::ROOK: pieceRect.x = 400; break;
        case Type::PAWN: pieceRect.x = 500; break;
        default: pieceRect.x = -1;
    }
    return pieceRect;
}

// Draws background image outside of board
void GUI::drawBackground() {
    SDL_RenderCopy(renderer, bgSurface, NULL, NULL);
}

void GUI::drawTiles(Team team) {
    SDL_Rect dstRect;
    dstRect.x = GUIConstants::tileOffset;
    dstRect.y = GUIConstants::tileOffset;
    dstRect.w = GUIConstants::tileDimensions;
    dstRect.h = GUIConstants::tileDimensions;

    // Locates tile position based on increments times dimensions and determines tile color using modulo sum
    int counter = static_cast<int>(team) + 1;
    for(size_t i{}; i < 8; i++, counter++) {
        dstRect.x = GUIConstants::tileOffset;
        for(size_t j{}; j < 8; j++, counter++) {
            dstRect.x = GUIConstants::tileOffset + (j * GUIConstants::tileDimensions);
            SDL_RenderCopy(renderer, counter % 2 ? whiteTile : blackTile, NULL, &dstRect);
        }
        dstRect.y += GUIConstants::tileDimensions;
    }
}

void GUI::drawLetters(Team team) {
    if(team == Team::WHITE) {
        SDL_RenderCopy(renderer, whiteLetters, NULL, NULL);
    } else {
        SDL_RenderCopy(renderer, blackLetters, NULL, NULL);
    }
}

void GUI::drawBoard(const Board& board) {
    drawBackground();
    drawTiles(board.getCurrentTurn());
    drawLetters(board.getCurrentTurn());
    for(size_t i{}; i < 8; i++) {
        for(size_t j{}; j < 8; j++) {
            if(board[i][j] != EMPTY) {
                drawPiece(i, j, board[i][j]->getTeam(), board[i][j]->getType());
            }
        }
    }
}

void GUI::drawMoves(const Board& board, Position piecePos) {
    using namespace checkUtils;

    std::vector<Move> moves;
    Piece* piece = board[piecePos.rank][piecePos.file];
    if(piece == EMPTY) return;

    auto it = genMoveFunctions.find(piece->getType());
    if(it != genMoveFunctions.end()) {
        it->second(board, piecePos, moves);
    }

    SDL_Rect dstRect;
    for(const auto& move : moves) {
        if(Check::canMoveToSpot(board, move.startPos, move.endPos)) {
            dstRect.x = GUIConstants::tileOffset + (move.endPos.file * GUIConstants::tileDimensions);
            dstRect.y = GUIConstants::tileOffset + (move.endPos.rank * GUIConstants::tileDimensions);
            dstRect.w = GUIConstants::tileDimensions;
            dstRect.h = GUIConstants::tileDimensions;
            SDL_RenderCopy(renderer, overlay, NULL, &dstRect);
        }
    }
}

Position GUI::evaluateClick(const SDL_Event& event, Team team) {
    int x_val {event.button.x};
    int y_val {event.button.y};

    // Determine if its within the grid, if so, determine position
    if(x_val < 25 || x_val > 825 || y_val < 25 || y_val > 825) return Position(-1, -1);

    int j = static_cast<int>((x_val - 25)/100);
    int i = static_cast<int>((y_val - 25)/100);

    Position pos(i, j);
    if(team == Team::BLACK) {
        pos.rank = 7 - pos.rank;
        pos.file = 7 - pos.file;
    }
    return pos;
}

void GUI::drawPiece(int x, int y, Team team, Type pieceType) {
    SDL_Rect pieceRect = findPiece(team, pieceType);
    SDL_Rect dstRect;
    dstRect.x = GUIConstants::tileOffset + (y * GUIConstants::tileDimensions); // X position
    dstRect.y = GUIConstants::tileOffset + (x * GUIConstants::tileDimensions); // Y position
    dstRect.w = GUIConstants::tileDimensions; // Width of the tile
    dstRect.h = GUIConstants::tileDimensions; // Height of the tile
    SDL_RenderCopy(renderer, pieces, &pieceRect, &dstRect);
}

void GUI::drawWinner(Team winningTeam) {
    SDL_Rect dstRect;
    // Render Winning Background
    dstRect.x = GUIConstants::tileOffset + GUIConstants::winBgOffset;
    dstRect.y = GUIConstants::tileOffset + GUIConstants::winBgOffset;
    SDL_QueryTexture(bgWin, NULL, NULL, &(dstRect.w), &(dstRect.h));
    SDL_RenderCopy(renderer, bgWin, NULL, &dstRect);

    // Render name of winning Team
    winTeam = winningTeam == Team::WHITE ? SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("../assets/White.bmp")) :
                                     SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("../assets/Black.bmp"));
    SDL_RenderCopy(renderer, winTeam, NULL, &dstRect);
}

void GUI::onUpdate() {
    SDL_RenderPresent(renderer);
}

void GUI::exit() {
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(bgSurface);
    SDL_DestroyTexture(blackTile);
    SDL_DestroyTexture(whiteTile);
    SDL_DestroyTexture(overlay);
    SDL_DestroyTexture(bgWin);
    SDL_DestroyTexture(pieces);
    SDL_DestroyTexture(whiteLetters);
    SDL_DestroyTexture(blackLetters);
    if(winTeam != nullptr) SDL_DestroyTexture(winTeam); // This is the only one that may be null
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}