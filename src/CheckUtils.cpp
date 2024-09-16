#include "CheckUtils.hpp"

bool checkUtils::canMoveKing(const Board& board, Position startPos, Position endPos) {
    Team kingTeam = board[startPos.rank][startPos.file]->getTeam();
    // Check if King falls into check at end position
    if(checkBounds(board, startPos, endPos) && !isKingInCheck(board, endPos, kingTeam) && isOneTile(startPos, endPos)) {
        return true;
    }
    return false;
}

bool checkUtils::canMoveQueen(const Board& board, Position startPos, Position endPos) {
    // Check if endPos is out of bounds and if positions share a row/column/diagonal
    if(checkBounds(board, startPos, endPos) && (checkAxially(startPos, endPos) || checkDiags(startPos, endPos))) {
        return(checkSliding(board, startPos, endPos, slideType::Queen));
    }
    return false;
}

// Shouldnt have can castle inside it
bool checkUtils::canMoveRook(const Board& board, Position startPos, Position endPos) {
    // Check if endPos is out of bounds and if positions share a row/column
    if((checkBounds(board, startPos, endPos) && checkAxially(startPos, endPos)) || isCastlingMove(board, startPos, endPos)) { // these checks here conflict with castling one
        return(checkSliding(board, startPos, endPos, slideType::Rook) || canCastle(board, startPos, endPos));
    }
    return false;
}

bool checkUtils::canMoveBishop(const Board& board, Position startPos, Position endPos) {
    // Check if endPos is out of bounds and if positions share a row/column
    if(checkBounds(board, startPos, endPos) && checkDiags(startPos, endPos)) {
        return(checkSliding(board, startPos, endPos, slideType::Bishop));
    }
    return false;
}

bool checkUtils::canMoveKnight(const Board& board, Position startPos, Position endPos) {
    if(!checkBounds(board, startPos, endPos)) return false;
    // Check to see if the end position is any 8 of the possible knights' spots
    for(size_t i{}; i < 8; i++) {
        if(startPos.rank + knightOffsets[i].rank == endPos.rank && startPos.file + knightOffsets[i].file == endPos.file) {
            return true;
        }
    }
    return false;
}


bool checkUtils::canMovePawn(const Board& board, Position startPos, Position endPos) {
    if(!checkBounds(board, startPos, endPos)) return false;

    Piece* startPosPiece = board[startPos.rank][startPos.file];
    Piece* endPosPiece = board[endPos.rank][endPos.file];

    Team startTeam = startPosPiece->getTeam();
    int rankMoveDirection = (board.getCurrentTurn() == Team::WHITE) 
                ? (startTeam == Team::WHITE ? -1 : 1)
                : (startTeam == Team::WHITE ? 1 : -1);


    bool isSameFile = (endPos.file == startPos.file);
    bool isEndPositionEmpty = (endPosPiece == EMPTY);
    bool isJumpTileEmpty = (board[startPos.rank + (1 * rankMoveDirection)][startPos.file] == EMPTY);
    bool isSingleStepMove = (endPos.rank == startPos.rank - 1);
    bool isDoubleStepMove = (endPos.rank == startPos.rank - 2 && startPos.rank == 6);
    bool isDiagonalMove = (isSingleStepMove && (abs(endPos.file - startPos.file) == 1));
    bool isCaptureMove = (!isEndPositionEmpty && endPosPiece->getTeam() != startPosPiece->getTeam());
    bool forwardMove = isSameFile && isEndPositionEmpty && (isSingleStepMove || (isDoubleStepMove && isJumpTileEmpty));
    bool captureMove = isDiagonalMove && isCaptureMove;

    if(forwardMove || captureMove) {
        return true;
    }
    return false;
}

// Returns true if Positions are within the board and not moving to spot taken by teammate
bool checkUtils::checkBounds(const Board& board, Position startPos, Position endPos) {
    Piece* endPosPiece = board[endPos.rank][endPos.file];
    Piece* startPosPiece = board[startPos.rank][startPos.file];

    // Out of bounds
    if (endPos.file > 7 || endPos.file < 0 || endPos.rank > 7 || endPos.rank < 0 ) {
        return false;
    } // Moving to spot where its not null and spot occupied by teammate
    else if (endPosPiece != EMPTY && endPosPiece->getTeam() == startPosPiece->getTeam()) {
        return false;
    } // Else within Bounds
    else return true;
}

// Returns true if positions are alligned vertically and/or horizontally
bool checkUtils::checkAxially(Position startPos, Position endPos) {
    return (endPos.rank == startPos.rank || startPos.file == endPos.file);
}

// Returns true if positions are alligned diagonally
bool checkUtils::checkDiags(Position startPos, Position endPos) {
    return (abs(endPos.rank - startPos.rank) == abs(startPos.file - endPos.file));
}

bool checkUtils::checkSliding(const Board& board, Position startPos, Position endPos, slideType type) {
    // Obtain direction vector (Ex. gives (1,0) if going from (3,4) to (7,4))
    int rankDirection = (endPos.rank > startPos.rank) - (endPos.rank < startPos.rank);
    int fileDirection = (endPos.file > startPos.file) - (endPos.file < startPos.file);

    // Determine range of indexing arrays based on piece type
    int start = type == slideType::Bishop ? 4 : 0;
    int end = type == slideType::Rook ? 4 : 8;

    for(int i = start; i < end; i++) {
        if(rankDirection == slidingOffsets[i].rank && fileDirection == slidingOffsets[i].file) {
            Team startPosTeam = board[startPos.rank][startPos.file]->getTeam();
            Team enemyTeam = startPosTeam == Team::WHITE ? Team::BLACK : Team::WHITE;

            // Step incrementally from startPos to endPos and see if any obstructions
            do {
                startPos.rank += rankDirection;
                startPos.file += fileDirection;

                Piece* currentPiece = board[startPos.rank][startPos.file];
                // Check Obstruction: If space is not null and occupied by teammate OR occupied by enemy BUT is not the final position, there is an obstruction
                if (currentPiece != EMPTY && (currentPiece->getTeam() == startPosTeam || 
                   (currentPiece->getTeam() == enemyTeam && startPos.rank != endPos.rank && startPos.file != endPos.file)))
                    return false;
            } 
            while (startPos != endPos);

            return true;
        }
    }
    return false;
}

bool checkUtils::isOneTile(Position startPos, Position endPos) {
    int xDiff = abs(endPos.rank - startPos.rank);
    int yDiff = abs(endPos.file - startPos.file);
    return (xDiff == 1 && yDiff == 0) || (xDiff == 0 && yDiff == 1) || (xDiff == 1 && yDiff == 1);
}

void checkUtils::genMovesKing(const Board& board, Position startPos, std::vector<Move>& moves) {
    for(int i = -1; i < 2; i++) {
        for(int j = -1; j < 2; j++) {
            Position tempPos(startPos.rank + i, startPos.file + j);
            if(checkBounds(board, startPos, tempPos)) {
                moves.push_back(Move(startPos, tempPos));
            }
        }
    }
}

void checkUtils::genMovesQueen(const Board& board, Position startPos, std::vector<Move>& moves) {
    genSliding(board, startPos, slideType::Queen, moves);
}

void checkUtils::genMovesRook(const Board& board, Position startPos, std::vector<Move>& moves) {
    genSliding(board, startPos, slideType::Rook, moves);
}

void checkUtils::genMovesBishop(const Board& board, Position startPos, std::vector<Move>& moves) {
    genSliding(board, startPos, slideType::Bishop, moves);
}

void checkUtils::genMovesKnight(const Board& board, Position startPos, std::vector<Move>& moves) {
    Team startPosTeam = board[startPos.rank][startPos.file]->getTeam();

    for(size_t i{}; i < 8; i++) {
        Position tempPos(startPos.rank + knightOffsets[i].rank, startPos.file + knightOffsets[i].file);
        Piece* currentPiece = board[tempPos.rank][tempPos.file];
        if(!checkBounds(board, startPos, tempPos)) continue;

        // Valid move if moving to empty spot or enemy's spot
        if((currentPiece == EMPTY) || (currentPiece->getTeam() != startPosTeam)) {
            moves.push_back(Move(startPos, tempPos));
        }
    }
}

void checkUtils::genMovesPawn(const Board& board, Position startPos, std::vector<Move>& moves) {
    Team startTeam = board[startPos.rank][startPos.file]->getTeam();

    int rankMoveDirection = (board.getCurrentTurn() == Team::BLACK)
                    ? (startTeam == Team::WHITE ? -1 : 1)
                    : (startTeam == Team::WHITE ? 1 : -1);

    for(size_t i{}; i < 4; i++) {
        Position tempPos(startPos.rank + (pawnOffsets[i].rank * rankMoveDirection), startPos.file + pawnOffsets[i].file);
        if(!checkBounds(board, startPos, tempPos)) continue;

        // i indices represent the type of move from offset array
        Piece* currentPiece = board[tempPos.rank][tempPos.file];
        bool isSingleStep = (i == 0) && (currentPiece == EMPTY);
        bool isDoubleStep = (i == 1) && (startPos.rank == 6) && (currentPiece == EMPTY);
        bool isCapture = ((i == 2) || (i == 3)) && (currentPiece != EMPTY) && (currentPiece->getTeam() != startTeam);

        if(isSingleStep || isDoubleStep || isCapture) {
            moves.push_back(Move(startPos, tempPos));
        }
    }
}

void checkUtils::genSliding(const Board& board, Position startPos, slideType type, std::vector<Move>& moves) {
    // Determine range of indexing arrays based on piece type
    int start = type == slideType::Bishop ? 4 : 0;
    int end = type == slideType::Rook ? 4 : 8;
    Team startPosTeam = board[startPos.rank][startPos.file]->getTeam();

    for(int i = start; i < end; i++) {
        int rankDirection = slidingOffsets[i].rank;
        int fileDirection = slidingOffsets[i].file;
        Position tempPos(startPos.rank + rankDirection, startPos.file + fileDirection);

        while(tempPos.file < 8 && tempPos.file >= 0 && tempPos.rank < 8 && tempPos.rank >= 0) {
            Piece* currentPiece = board[tempPos.rank][tempPos.file];
            // Empty Space
            if(currentPiece == EMPTY) {
                moves.push_back(Move(startPos, tempPos));
            } // Obstructed by teammate
            else if (currentPiece->getTeam() == startPosTeam) {
                break;
            } // Obstructed by enemy
            else if (currentPiece->getTeam() != startPosTeam) {
                moves.push_back(Move(startPos, tempPos));
                break;
            } 
            else perror("Reached unknown state!\n");

            tempPos.rank += rankDirection;
            tempPos.file += fileDirection;
        }
    }
}

// Checks if any piece from opponent's team can move to King's position
bool checkUtils::isKingInCheck(const Board& board, Position kingPos, Team team) {
    Team enemyTeam = team == Team::WHITE ? Team::BLACK : Team::WHITE;
    std::vector<Move> moves = Check::genAllMoves(board, enemyTeam);

    // Check if any enemy moves reach King's position
    for(const auto move : moves) {
        if(move.endPos == kingPos) {
            return true;
        }
    }
    return false;
}

Position checkUtils::locateKing(const Board& board, Team team) {
    for(size_t i{}; i < 8; i++) {
        for(size_t j{}; j < 8; j++) {
            if(board[j][i] != EMPTY && board[j][i]->getType() == Type::KING && board[j][i]->getTeam() == team)
                return(Position(j,i));
        }
    }
    perror("Unable to locate King\n");
    return Position();
}

// Simulates a move and then determines if the King is in check
bool checkUtils::isKingSafe(const Board& board, Position startPos, Position endPos) {
    Board tempBoard(board);
    tempBoard.grid[endPos.rank][endPos.file] = std::move(tempBoard.grid[startPos.rank][startPos.file]);
    tempBoard.grid[startPos.rank][startPos.file] = EMPTY;

    Team currentTeam = board[startPos.rank][startPos.file]->getTeam();
    Position kingPos = locateKing(tempBoard, currentTeam);
    return(!isKingInCheck(tempBoard, kingPos, currentTeam));
}

std::string checkUtils::pieceToString(const Board& board, Position pos) {
    std::string pieceName;
    board[pos.rank][pos.file]->getTeam() == Team::WHITE ? pieceName += "white" : pieceName += "black";

    Type pieceType = board[pos.rank][pos.file]->getType();
    if(pieceType == Type::ROOK && pos == Position(7, 0)) {
        pieceName += "LeftRook";
    } else if (pieceType == Type::ROOK && pos == Position(7, 7)) {
        pieceName += "RightRook";
    } else if (pieceType == Type::KING) {
        pieceName += "King";
    } else {
        pieceName.clear();
    }
    return pieceName;
}

void checkUtils::castlingMark(Board& board, Position startPos) {
    std::string pieceName = pieceToString(board, startPos);

    if(!pieceName.empty()) {
        auto it = board.castlingMap.find(pieceName);
        if(it != board.castlingMap.end()) {
            board.setMoved(pieceName);
        }
    }
}

bool checkUtils::canCastle(const Board& board, Position startPos, Position endPos) {
    Piece* startPiece = board[startPos.rank][startPos.file];
    Piece* endPiece = board[endPos.rank][endPos.file];
    if(startPiece == EMPTY || startPiece->getType() != Type::ROOK || endPiece->getType() != Type::KING || 
    startPiece->getTeam() != endPiece->getTeam()) return false;

    std::string pieces[] {pieceToString(board, startPos), pieceToString(board, endPos)};
    for(auto piece : pieces) {
        if(!piece.empty()) {
            auto it = board.castlingMap.find(piece);
            if(it != board.castlingMap.end() && *(it->second) == true) {
                return false;
            }
        }
    }

    Position adjacentSpot(endPos.rank, startPos.file == 7 ? endPos.file + 1 : endPos.file - 1);
    return checkSliding(board, startPos, adjacentSpot, slideType::Rook);
}

// Positions represent Rook's positions
void checkUtils::performCastle(Board& board, Position startPos, Position endPos) {
    castlingMark(board, startPos);
    castlingMark(board, endPos);

    Position adjacentSpot(endPos.rank, startPos.file == 7 ? endPos.file + 1 : endPos.file - 1); // where castled rook will go
    Position kingPos = locateKing(board, board[startPos.rank][startPos.file]->getTeam());
    Position kingShift(adjacentSpot.rank, startPos.file == 7 ? adjacentSpot.file + 1 : adjacentSpot.file - 1);

    shiftPiece(board, startPos, adjacentSpot);
    shiftPiece(board, kingPos, kingShift);
}

void checkUtils::shiftPiece(Board& board, Position startPos, Position endPos) {
    board.grid[endPos.rank][endPos.file] = std::move(board.grid[startPos.rank][startPos.file]);
    board.grid[startPos.rank][startPos.file] = EMPTY;
}

// Note: Check that both start and endpos are not null
bool checkUtils::isCastlingMove(const Board& board, Position startPos, Position endPos) {
    Piece* startPiece = board[startPos.rank][startPos.file];
    Piece* endPiece = board[endPos.rank][endPos.file];
    if(startPiece == EMPTY || endPiece == EMPTY) return false;

    if(startPiece->getType() == Type::ROOK && endPiece->getType() == Type::KING && startPiece->getTeam() == endPiece->getTeam()) {
        return true;
    }
    return false;
}

// If any Pawn reaches other side, promote it to a Queen
void checkUtils::pawnPromotion(Board& board) {
    const int ranks[] = {0, 7};
    for(const auto rank : ranks) {
        for (size_t i{}; i < 8; i++) {
            if(board[rank][i] != EMPTY && board[rank][i]->getType() == Type::PAWN) {
                board[rank][i]->setType(Type::QUEEN);
            } 
        }
    }
}

void checkUtils::assertWinner(const Team winningTeam) {
    GUI::drawWinner(winningTeam);
    GUI::onUpdate();
    SDL_Event event;

    bool status = true;
    while(status) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                status = false;
                break;
            }
        }
    }
    return;
}