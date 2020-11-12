#include "Game.h"

using namespace std;
using namespace sf;

/*Game::~Game() {
    for (Piece *p : this->pieces) {
        delete p;
    }
}*/

Game::Game() { }

Game::Game(const string pgnFileName, const int boardSideLength, const Vector2f boardOffset) {
    // Open the file stream
    ifstream pgnFileStream(pgnFileName);

    // Check for failure
    if (pgnFileStream.fail()) {
        throw 1;
    }

    // Initialise the game using the data extracted by the PgnManager
    PgnManager pgnManager(pgnFileStream);
    this->initialise(pgnManager.White, pgnManager.Black, boardSideLength, boardOffset);

    // Make the moves to get to the given starting position.
    for (int i = 0; i < pgnManager.EncodedMoves.size(); i++) {
        string encodedMove = pgnManager.EncodedMoves.at(i);
        Piece* pieceToMove = NULL;
        Square* newSquare = NULL;

        if (encodedMove.at(encodedMove.size() - 2) == '=') {
            // This is a promotion. Chop off the promotion text.
            if (encodedMove.at(encodedMove.size() - 1) != 'Q') {
                throw 2;
            }

            encodedMove = encodedMove.substr(0, encodedMove.size() - 2);
        } else if (encodedMove.at(encodedMove.size() - 1) == '+' || encodedMove.at(encodedMove.size() - 1) == '#') {
            // This move gave check. Chop it off as we don't handle that here.
            encodedMove = encodedMove.substr(0, encodedMove.size() - 1);
        }

        if (encodedMove == "O-O") {
            // Castling Kingside
            pieceToMove = this->getCurrentPlayersKing();

            // Castling Kingside, regardless of colour, moves the King to the g
            // file (the 7th file)
            Coordinate newCoordinate(7, pieceToMove->currentSquare->coordinate.rank);
            newSquare = this->board.GetSquareAtCoordinate(newCoordinate);
        } else if (encodedMove == "O-O-O") {
            // Castling Queenside
            pieceToMove = this->getCurrentPlayersKing();

            // Castling Queenside, regardless of colour, moves the King to the c
            // file (the 3rd file)
            Coordinate newCoordinate(3, pieceToMove->currentSquare->coordinate.rank);
            newSquare = this->board.GetSquareAtCoordinate(newCoordinate);
        } else {
            // The last two characters will always be the final coordinate of the move.
            string newCoordinateString = encodedMove.substr(encodedMove.size() - 2, 2);
            newSquare = this->board.GetSquareAtCoordinate(Coordinate(newCoordinateString));

            // Chop off the last two characters of the string, they aren't needed anymore.
            encodedMove = encodedMove.substr(0, encodedMove.size() - 2);

            // Determine the piece to move
            vector<Piece*> currentPlayersPieces = this->getCurrentPlayersPieces();

            if (encodedMove.size() == 0) {
                // This is a normal pawn push. Find the pawn on the file that can
                // move to the newCoordinate.
                for (int j = 0; j < currentPlayersPieces.size(); j++) {
                    Piece* piece = currentPlayersPieces.at(j);

                    if (piece->type == Pawn && piece->currentSquare->coordinate.file == newSquare->coordinate.file) {
                        // This piece is a pawn, the correct colour, and the correct file.
                        // If the piece can move to the newSquare, use it as the pieceToMove.
                        vector<Coordinate> validMoveCoordinates = piece->ValidMoveCoordinates();

                        for (int k = 0; k < validMoveCoordinates.size(); k++) {
                            if (validMoveCoordinates.at(k) == newSquare->coordinate) {
                                pieceToMove = piece;
                                break;
                            }
                        }
                    }
                }
            } else {
                // If this is a capture, chop off the "x" since we don't handle it here.
                if (encodedMove.at(encodedMove.size() - 1) == 'x') {
                    encodedMove = encodedMove.substr(0, encodedMove.size() - 1);
                }

                if (encodedMove.size() != 1) {
                    throw 3;
                }

                // If we got here, this is either a pawn capture, or a normal
                // major/minor piece move.

                if (islower(encodedMove.at(0))) {
                    // This is a pawn capture. encodedMove.at(0) will always
                    // contain the char representation of the old file.
                    string oldCoordinateString;
                    oldCoordinateString.push_back(encodedMove.at(0));

                    // The rank is always "back" one square. (For Black, "back"
                    // means an increase the rank.)
                    if (this->currentPlayerColour == White) {
                        oldCoordinateString += to_string(newSquare->coordinate.rank - 1);
                    } else {
                        oldCoordinateString += to_string(newSquare->coordinate.rank + 1);
                    }

                    Square* oldSquare = this->board.GetSquareAtCoordinate(Coordinate(oldCoordinateString));
                    pieceToMove = this->getPieceOnSquare(oldSquare);
                } else {
                    // This is a normal major/minor piece move. Find the piece with the correct
                    // char representation and, if it can move to the newSquare, use
                    // it as the piece to move.
                    for (int j = 0; j < currentPlayersPieces.size(); j++) {
                        Piece* piece = currentPlayersPieces.at(j);

                        if (piece->GetPieceCharRepresentation() == encodedMove.at(0)) {
                            vector<Coordinate> validMoveCoordinates = piece->ValidMoveCoordinates();

                            for (int k = 0; k < validMoveCoordinates.size(); k++) {
                                if (validMoveCoordinates.at(k) == newSquare->coordinate) {
                                    pieceToMove = piece;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        // If we weren't able to find the piece that should move, blow up.
        if (pieceToMove == NULL) {
            throw 4;
        }

        // Move the piece. If the move is invalid, then the movetext is invalid, blow up.
        if (this->movePieceIfValid(pieceToMove, newSquare)) {
            this->completeTurn();
        } else {
            throw 5;
        }
    }
}

Game::Game(const string player1Name, const string player2Name, const int boardSideLength, const Vector2f boardOffset) {
    this->initialise(player1Name, player2Name, boardSideLength, boardOffset);
}

void Game::HandleClick(const int x, const int y) {
    // If the click was off the board, don't want to do anything. (Currently,
    // this will never happen, but in the future, if there are more UI elements,
    // the click could be off the board.
    if (x < this->boardOffset.x ||
        y < this->boardOffset.y ||
        x > this->boardOffset.x + this->board.boardSideLength ||
        y > this->boardOffset.y + this->board.boardSideLength) {
        return;
    }

    // Determine what square was clicked and what piece is on the square.
    Square* clickedSquare = this->board.GetSquareAtPixel(x - this->boardOffset.x, y - this->boardOffset.y);
    Piece* pieceOnSquare = this->getPieceOnSquare(clickedSquare);
    Piece* selectedPiece = this->getSelectedPiece();

    if (pieceOnSquare != NULL && pieceOnSquare->colour == this->currentPlayerColour) {
        // The user clicked on a different one of their pieces. Select it.
        this->deselectPiece(selectedPiece);
        this->board.DeselectSquare();
        pieceOnSquare->isSelected = true;
        this->board.HighlightSquare(clickedSquare);
    } else if (selectedPiece != NULL) {
        // A piece is currently selected. Try to move it, but if it cannot be
        // moved, deselect the piece.
        if (this->movePieceIfValid(selectedPiece, clickedSquare)) {
            // Move successful. Switch players.
            this->completeTurn();
        } else {
            // Can't move the piece to the clicked coordinate, deselect the piece.
            this->deselectPiece(selectedPiece);
            this->board.DeselectSquare();
        }
    }
}

vector<Sprite> Game::GetGameSprites() const {
    return board.GetBoardSprites(this->getPiecesInPlay(), currentPlayerColour, this->boardOffset);
}

bool Game::canCastle(const Square* newSquare) const {
    // Castling can only occur only if:
    //  1) the King has not moved earlier in the game
    //  2) the Rook that castles has not moved earlier in the game
    //  3) there are no pieces between the King and the Rook
    //  4) the King is not in Check
    //  5) the King does not move through a square that is attacked by an opponent's piece
    //  6) the King would not be in check after castling

    Piece* king = this->getCurrentPlayersKing();

    if (king->hasMoved || this->squareIsAttacked(king->currentSquare)) {
        return false;
    }

    // Determine which side the castle is on. If Kingside, the rank will
    // always be 7. If Queenside, the rank will always be 3. This is the
    // case regardless of whether the player is Black or White. If it was a
    // normal King move (only one space), the move will be validated elsewhere.

    // Find the castling Rook
    Square* passThroughSquare;
    Piece* castlingRook = NULL;

    // Determine the file where the Rook should be
    if (newSquare->coordinate.file == 7) { // Kingside
        passThroughSquare = this->getCastlingRookFinalSquare(true, false);
        castlingRook = this->getCastlingRook(true, false);
    } else if (newSquare->coordinate.file == 3) { // Queenside
        passThroughSquare = this->getCastlingRookFinalSquare(false, true);
        castlingRook = this->getCastlingRook(false, true);
    } else {
        throw 6;
    }

    // If there is no piece on the castling Rook's square or if the
    // piece on the square is not a Rook, or if the rook has moved,
    // this is an invalid move so return false.
    if (castlingRook == NULL || castlingRook->type != Rook || castlingRook->hasMoved) {
        return false;
    }

    // If there are pieces between the King and the castling Rook,
    // the castle is invalid, return false.
    if (this->piecesAreInTheWay(king, castlingRook->currentSquare)) {
        return false;
    }

    // If the square that the King would pass through is being attacked,
    // the castle is invalid so return false.
    if (this->squareIsAttacked(passThroughSquare)) {
        return false;
    }

    // If the square that the King would land on is being attacked, the
    // castle is invalid so return false.
    if (this->squareIsAttacked(newSquare)) {
        return false;
    }

    // If we got here, the castle is valid.
    return true;
}

void Game::completeTurn() {
    // Swap the curent player
    this->currentPlayerColour = currentPlayerColour == White ? Black : White;

    // If the King is in check, the last move was a checking move.
    if (this->squareIsAttacked(this->getCurrentPlayersKingsSquare())) {
        this->Moves.back().isCheck = true;
    }

    // Check for checkmate and stalemate
    if (!this->playerHasValidMoves()) {
        if (this->squareIsAttacked(this->getCurrentPlayersKingsSquare())) {
            // Loss by Checkmate.

            // White is always at index 0 so if White lost by checkmate, Black
            // (at index 1) has won the game.
            int winnerIndex = this->currentPlayerColour == White ? 1 : 0;

            this->Players[winnerIndex].hasWon = true;
            this->Moves.back().isCheckmate = true;
            this->IsComplete = true;

            this->Termination = this->Players[winnerIndex].name;
            this->Termination.append(" won by checkmate.");
        } else {
            // Draw by Stalemate
            this->IsComplete = true;
            this->IsDraw = true;
            this->Termination = "Draw by stalemate";
        }
    }

    // Check for draw by insufficient materiel (not enough pieces to generate checkmate)
    if (this->insufficientMaterialExists()) {
        this->IsComplete = true;
        this->IsDraw = true;
        this->Termination = "Draw by insufficient material";
    }

    this->board.SwitchBoardOrientation();
}

void Game::deselectPiece(Piece* piece) {
    if (piece != NULL) {
        piece->isSelected = false;
    }
}

Piece* Game::getCastlingRook(const bool isKingsideCastle, const bool isQueensideCastle) const {
    Coordinate castlingRookCordinate;
    castlingRookCordinate.rank = this->currentPlayerColour == White ? 1 : 8;

    if (isKingsideCastle) {
        // The rook will always be on the h file (the 8th file)
        castlingRookCordinate.file = 8;
    } else if (isQueensideCastle) {
        // The rook will always be on the a file (the 1st file)
        castlingRookCordinate.file = 1;
    }

    return this->getPieceOnSquare(this->board.GetSquareAtCoordinate(castlingRookCordinate));
}

Square* Game::getCastlingRookFinalSquare(const bool isKingsideCastle, const bool isQueensideCastle) const {
    Coordinate castlingRookFinalCordinate;
    castlingRookFinalCordinate.rank = this->currentPlayerColour == White ? 1 : 8;

    if (isKingsideCastle) {
        castlingRookFinalCordinate.file = 6;
    } else if (isQueensideCastle) {
        castlingRookFinalCordinate.file = 4;
    }

    return this->board.GetSquareAtCoordinate(castlingRookFinalCordinate);
}

Piece* Game::getCurrentPlayersKing() const {
    // The king can never be captured, so this will always return a piece. If
    // it doesn't, that's really bad.
    vector<Piece*> currentPlayersPieces = this->getCurrentPlayersPieces();

    for (int i = 0; i < currentPlayersPieces.size(); i++) {
        if (currentPlayersPieces.at(i)->type == King) {
            return currentPlayersPieces.at(i);
        }
    }

    return NULL;
}

Square* Game::getCurrentPlayersKingsSquare() const {
    // The king can never be captured, so this will always return a square. If
    // it doesn't, that's really bad.
    return this->getCurrentPlayersKing()->currentSquare;
}

vector<Piece*> Game::getCurrentPlayersPieces() const {
    vector<Piece*> nonCapturedPieces = this->getPiecesInPlay();
    vector<Piece*> currentPlayerPieces;

    for (int i = 0; i < nonCapturedPieces.size(); i++) {
        if (nonCapturedPieces.at(i)->colour == this->currentPlayerColour) {
            currentPlayerPieces.push_back(nonCapturedPieces.at(i));
        }
    }

    return currentPlayerPieces;
}

vector<Piece*> Game::getOpponentPieces() const {
    vector<Piece*> nonCapturedPieces = this->getPiecesInPlay();
    vector<Piece*> opponentPieces;

    for (int i = 0; i < nonCapturedPieces.size(); i++) {
        if (nonCapturedPieces.at(i)->colour != this->currentPlayerColour) {
            opponentPieces.push_back(nonCapturedPieces.at(i));
        }
    }

    return opponentPieces;
}

vector<Piece*> Game::getPiecesInPlay() const {
    vector<Piece*> nonCapturedPieces;

    for (int i = 0; i < this->pieces.size(); i++) {
        if (!pieces.at(i)->isCaptured) {
            nonCapturedPieces.push_back(pieces.at(i));
        }
    }
    return nonCapturedPieces;
}

Piece* Game::getPieceOnSquare(const Square* square) const {
    vector<Piece*> nonCapturedPieces = this->getPiecesInPlay();

    for (int i = 0; i < nonCapturedPieces.size(); i++) {
        if (nonCapturedPieces.at(i)->currentSquare == square) {
            return nonCapturedPieces.at(i);
        }
    }

    return NULL;
}

Piece* Game::getSelectedPiece() const {
    vector<Piece*> nonCapturedPieces = this->getPiecesInPlay();

    for (int i = 0; i < nonCapturedPieces.size(); i++) {
        if (nonCapturedPieces.at(i)->isSelected) {
            return nonCapturedPieces.at(i);
        }
    }

    return NULL;
}

void Game::initialise(const string player1Name, const string player2Name, const int boardSideLength, const Vector2f boardOffset) {
    this->board = Board(boardSideLength);
    this->boardOffset = boardOffset;

    // Setup Players
    this->Players[0].name = player1Name;
    this->Players[0].colour = White;

    this->Players[1].name = player2Name;
    this->Players[1].colour = Black;

    this->placePieces();
}

/* Insufficient material exists if:
* a) both sides have a bare King
* b) one side has a King and a minor piece (Bishop or Knight) against a bare King
* c) one side has two Knights against a bare King
* d) both sides have a King and a Bishop, the Bishops being the same colour
*
* See: https://chessprogramming.wikispaces.com/Draw+Evaluation
*/
bool Game::insufficientMaterialExists() const {
    vector<Piece*> piecesInPlay = this->getPiecesInPlay();

    // If there are more than six pieces in play, there is always sufficient material.
    if (piecesInPlay.size() > 6) {
        return false;
    }

    // If each player only has one piece, then only the Kings are left on the board.
    if (piecesInPlay.size() == 2) {
        return true;
    }

    // FUTURE: Implement the rest of the Insufficient Material cases. The other
    // cases are fairly rare, and the user can exit the game manually by
    // closing the window.
    return false;
}

bool Game::moveIsValid(Piece* piece, Square* newSquare, bool& isKingsideCastle, bool& isQueensideCastle) {
    // Verify that the direction is correct
    vector<Coordinate> validMoveCoordinates = piece->ValidMoveCoordinates();
    bool canMoveToNewSquare = false;

    for (int i = 0; i < validMoveCoordinates.size(); i++) {
        Coordinate validMoveCoordinate = validMoveCoordinates.at(i);

        if (validMoveCoordinate == newSquare->coordinate) {
            canMoveToNewSquare = true;
            break;
        }
    }

    if (!canMoveToNewSquare) {
        return false;
    }

    // Don't allow the move to go through another piece
    if (this->piecesAreInTheWay(piece, newSquare)) {
        return false;
    }

    // Pawns have some interesting characteristics, deal with them here.
    if (piece->type == Pawn) {
        // Pawns can't capture forward
        if (piece->CoordinateIsOnSameFile(newSquare->coordinate) && newSquare->hasPiece) {
            return false;
        }

        // If this is a diagonal Pawn move, ensure that there is a piece there
        // or in the En Passant coordinate.
        if (!piece->CoordinateIsOnSameFile(newSquare->coordinate)) {
            // Find the piece on the square that the Pawn is moving to.
            Piece* pieceToCapture = this->getPieceOnSquare(newSquare);

            // If there is no piece, see if there is a Pawn in the En Passant
            // coordinate (the one right behind the square that the Pawn lands
            // on.
            if (pieceToCapture == NULL) {
                Coordinate enPassantCoordinate(newSquare->coordinate.file, piece->currentSquare->coordinate.rank);
                pieceToCapture = this->getPieceOnSquare(this->board.GetSquareAtCoordinate(enPassantCoordinate));

                // If the piece is not capturable En Passant, reset pieceToCapture.
                if (pieceToCapture != NULL && !pieceToCapture->capturableEnPassant) {
                    pieceToCapture = NULL;
                }
            }

            // If there is no piece to capture, then the move is invalid.
            if (pieceToCapture == NULL) {
                return false;
            }
        }
    }

    // Handle Castling
    if (piece->type == King && !piece->hasMoved) {
        if (newSquare->coordinate.file == 7) { // Kingside Castle
            isKingsideCastle = true;

            if (!this->canCastle(newSquare)) {
                return false;
            }
        } else if (newSquare->coordinate.file == 3) { // Queenside Castle
            isQueensideCastle = true;

            if (!this->canCastle(newSquare)) {
                return false;
            }
        }
    }

    // Don't allow the move if it would create check to the King
    Move move = this->populateMove(piece, newSquare, isKingsideCastle, isQueensideCastle);
    this->movePiece(piece, newSquare);

    // If the current player's King would be in check because of the move, the
    // move is invalid so revert it.
    if (this->squareIsAttacked(this->getCurrentPlayersKingsSquare())) {
        this->revertMove(move);
        return false;
    }

    // Always revert the move, the actual move will be done later.
    this->revertMove(move);

    // If we've gotten here, the move is valid.
    return true;
}

void Game::movePiece(Piece* piece, Square* newSquare) {
    piece->currentSquare->hasPiece = false;
    piece->currentSquare = newSquare;
    piece->hasMoved = true;
    newSquare->hasPiece = true;
}

bool Game::movePieceIfValid(Piece* piece, Square* newSquare) {
    Move move;
    vector<Piece*> piecesInPlay = this->getPiecesInPlay();
    bool isKingsideCastle = false;
    bool isQueensideCastle = false;

    if (!this->moveIsValid(piece, newSquare, isKingsideCastle, isQueensideCastle)) {
        return false;
    }

    move = this->populateMove(piece, newSquare, isKingsideCastle, isQueensideCastle);

    // Do the actual move
    this->movePiece(piece, newSquare);

    // Move the Rook if this is a castle
    if (isKingsideCastle || isQueensideCastle) {
        Piece* rook = this->getCastlingRook(isKingsideCastle, isQueensideCastle);
        Square* square = this->getCastlingRookFinalSquare(isKingsideCastle, isQueensideCastle);

        this->movePiece(rook, square);
    }

    // Clear the captureableEnPassant status of all of the pieces except for
    // the one that was just moved.
    for (int i = 0; i < piecesInPlay.size(); i++) {
        if (piecesInPlay.at(i) != piece) {
            piecesInPlay.at(i)->capturableEnPassant = false;
        }
    }

    // Deselect the piece.
    this->deselectPiece(piece);
    this->board.DeselectSquare();

    // Add the move to the Moves vector.
    this->Moves.push_back(move);

    return true;
}

bool Game::piecesAreInTheWay(const Piece* piece, const Square* destination) const {
    // Nothing is ever in the way of Knight
    if (piece->type == Knight) {
        return false;
    }

    int currentRank = piece->currentSquare->coordinate.rank;
    int currentFile = piece->currentSquare->coordinate.file;
    int destinationRank = destination->coordinate.rank;
    int destinationFile = destination->coordinate.file;
    Coordinate coordinate;

    if (piece->CoordinateIsOnSameDiagonal(destination->coordinate)) {
        if (currentRank < destinationRank) {
            // Destination square is above (for white) the current square.

            if (currentFile < destinationFile) {
                // Destination is to the right of the current piece. Iterate
                // from the file right after the current one, to the file right
                // before the destination file.
                for (int i = 1; i < destinationFile - currentFile; i++) {
                    coordinate.file = currentFile + i;
                    coordinate.rank = currentRank + i;

                    if (this->board.GetSquareAtCoordinate(coordinate)->hasPiece) {
                        // A piece exists on the diagonal.
                        return true;
                    }
                }
            } else {
                // Destination is to the left of the current piece. Iterate
                // from the file right after the destination to the file right
                // before the current one.
                for (int i = 1; i < currentFile - destinationFile; i++) {
                    coordinate.file = destinationFile + i;
                    coordinate.rank = destinationRank - i;

                    if (this->board.GetSquareAtCoordinate(coordinate)->hasPiece) {
                        // A piece exists on the diagonal.
                        return true;
                    }
                }
            }
        } else {
            // Destination square is below (for white) the current square.

            if (currentFile < destinationFile) {
                // Destination is to the right of the current piece. Iterate
                // from the file right after the current one, to the file right
                // before the destination file.
                for (int i = 1; i < destinationFile - currentFile; i++) {
                    coordinate.file = currentFile + i;
                    coordinate.rank = currentRank - i;

                    if (this->board.GetSquareAtCoordinate(coordinate)->hasPiece) {
                        // A piece exists on the diagonal.
                        return true;
                    }
                }
            } else {
                // Destination is to the left of the current piece. Iterate
                // from the file right after the destination to the file right
                // before the current one.
                for (int i = 1; i < currentFile - destinationFile; i++) {
                    coordinate.file = destinationFile + i;
                    coordinate.rank = destinationRank + i;

                    if (this->board.GetSquareAtCoordinate(coordinate)->hasPiece) {
                        // A piece exists on the diagonal.
                        return true;
                    }
                }
            }
        }
    } else if (piece->CoordinateIsOnSameRank(destination->coordinate)) {
        // The rank will always be the same
        coordinate.rank = destinationRank;

        if (currentFile < destinationFile) {
            // Destination is to the right.
            for (int i = 1; i < destinationFile - currentFile; i++) {
                coordinate.file = currentFile + i;

                if (this->board.GetSquareAtCoordinate(coordinate)->hasPiece) {
                    // A piece exists on the rank between the current file and
                    // the destination file.
                    return true;
                }
            }
        } else {
            // Destination is to the left.
            for (int i = 1; i < currentFile - destinationFile; i++) {
                coordinate.file = destinationFile + i;

                if (this->board.GetSquareAtCoordinate(coordinate)->hasPiece) {
                    // A piece exists on the rank between the current file and
                    // the destination file.
                    return true;
                }
            }
        }
    } else if (piece->CoordinateIsOnSameFile(destination->coordinate)) {
        // The file will always be the same
        coordinate.file = destinationFile;

        if (currentRank < destinationRank) {
            // Destination is above.
            for (int i = 1; i < destinationRank - currentRank; i++) {
                coordinate.rank = currentRank + i;

                if (this->board.GetSquareAtCoordinate(coordinate)->hasPiece) {
                    // A piece exists on the file between the current rank and
                    // the destination rank.
                    return true;
                }
            }
        } else {
            // Destination is below.
            for (int i = 1; i < currentRank - destinationRank; i++) {
                coordinate.rank = destinationRank + i;

                if (this->board.GetSquareAtCoordinate(coordinate)->hasPiece) {
                    // A piece exists on the file between the current rank and
                    // the destination rank.
                    return true;
                }
            }
        }
    }

    // If we got here, nothing is in the way.
    return false;
}

// Note, Minor Pieces refer to Bishops and Knights, major pieces refer to rooks and the queen
void Game::placeMinorAndMajorPieces(PlayerColour colour) {
    for (int i = 0; i < 8; i++) {
        Coordinate startingCoordinate;
        startingCoordinate.file = i + 1;

        // All White major/minor pieces start on the first rank. All Black
        // major/minor pieces start on the eighth rank.
        startingCoordinate.rank = colour == White ? 1 : 8;

        PieceType type;
        switch (startingCoordinate.file) {
        case 1:
        case 8:
            type = Rook;
            break;
        case 2:
        case 7:
            type = Knight;
            break;
        case 3:
        case 6:
            type = Bishop;
            break;
        case 4:
            type = Queen;
            break;
        case 5:
            type = King;
            break;
        }

        Square* startingSquare = this->board.GetSquareAtCoordinate(startingCoordinate);
        startingSquare->hasPiece = true;
        pieces.push_back(new Piece(colour, startingSquare, type));
    }
}

void Game::placePawns(PlayerColour colour) {
    for (int i = 0; i < 8; i++) {
        Coordinate startingCoordinate;
        startingCoordinate.file = i + 1;

        // All White pawns start on the second rank. All Black pawns start on the seventh rank.
        startingCoordinate.rank = colour == White ? 2 : 7;

        Square* startingSquare = this->board.GetSquareAtCoordinate(startingCoordinate);
        startingSquare->hasPiece = true;
        pieces.push_back(new Piece(colour, startingSquare, Pawn));
    }
}

void Game::placePieces() {
    // Setup Pawns
    placePawns(White);
    placePawns(Black);

    // Setup Major and Minor Pieces
    placeMinorAndMajorPieces(White);
    placeMinorAndMajorPieces(Black);
}

bool Game::playerHasValidMoves() {
    vector<Piece*> currentPlayersPieces = this->getCurrentPlayersPieces();

    // Loop through all of the pieces, check each of the valid move coordinates
    // for each. If there are any valid moves, the player has valid moves.
    for (int i = 0; i < currentPlayersPieces.size(); i++) {
        Piece* piece = currentPlayersPieces.at(i);
        vector<Coordinate> validMoveCoordinates = piece->ValidMoveCoordinates();

        for (int j = 0; j < validMoveCoordinates.size(); j++) {
            Square* square = this->board.GetSquareAtCoordinate(validMoveCoordinates.at(j));

            // This could happen with pawns if the coordinate is outside the board.
            if (square == NULL) {
                continue;
            }

            // We aren't actually doing anything with these, but they are
            // required for moveIsValid.
            bool isKingsideCastle, isQueensideCastle;

            if (this->moveIsValid(piece, square, isKingsideCastle, isQueensideCastle)) {
                return true;
            }
        }
    }

    return false;
}

Move Game::populateMove(Piece* piece, Square* newSquare, const bool isKingsideCastle, const bool isQueensideCastle) {
    Move move;

    move.isFirstMove = !piece->hasMoved; // This is the first move if the piece hasn't moved yet.
    move.newSquare = newSquare;
    move.oldSquare = piece->currentSquare;
    move.piecePointer = piece;
    move.piece = piece->Copy();

    // If it is a castle, just set that property and return the move, nothing
    // more is required.
    if (isKingsideCastle) {
        move.isKingsideCastle = true;
        return move;
    } else  if (isQueensideCastle) {
        move.isQueensideCastle = true;
        return move;
    }

    // Capture the piece on the new square if it exists.
    Piece* pieceToCapture = this->getPieceOnSquare(newSquare);

    // Handle En Passant capture
    if (piece->type == Pawn && pieceToCapture == NULL) {
        // If there is a Pawn to the right or left of the current Pawn that is
        // capturable En Passant, use that as the pieceToCapture.
        Coordinate enPassantCoordinate(piece->currentSquare->coordinate.rank, newSquare->coordinate.file);
        pieceToCapture = this->getPieceOnSquare(this->board.GetSquareAtCoordinate(enPassantCoordinate));

        if (pieceToCapture != NULL && !pieceToCapture->capturableEnPassant) {
            pieceToCapture = NULL;
        }
    }

    // If there is apiece to capture, capture it.
    if (pieceToCapture != NULL) {
        pieceToCapture->Capture();
        move.isCapture = true;
        move.capturedPiece = pieceToCapture;
    }

    // If the Pawn moves up two spaces on the first move, it is capturable En Passant
    if (piece->type == Pawn && !piece->hasMoved) {
        if ((this->currentPlayerColour == White && newSquare->coordinate.rank == 4) ||
            (this->currentPlayerColour == Black && newSquare->coordinate.rank == 5)) {
            piece->capturableEnPassant = true;
        }
    }

    // Promote the Pawn if necessary
    if (piece->type == Pawn) {
        if ((piece->colour == White && newSquare->coordinate.rank == 8) ||
            (piece->colour == Black && newSquare->coordinate.rank == 1)) {
            piece->Promote();
            move.isPromotion = true;
        }
    }

    return move;
}

void Game::revertMove(Move move) {
    // Revert promotion
    if (move.isPromotion) {
        move.piecePointer->type = Pawn;
    }

    // Revert capture
    if (move.isCapture) {
        move.capturedPiece->isCaptured = false;
    }

    // Revert hasMoved
    if (move.isFirstMove) {
        move.piecePointer->hasMoved = false;
    }

    // Revert the actual move
    move.piecePointer->capturableEnPassant = false;
    move.piecePointer->currentSquare->hasPiece = false;
    move.piecePointer->currentSquare = move.oldSquare;
    move.oldSquare->hasPiece = true;
}

bool Game::squareIsAttacked(const Square* square) const {
    vector<Piece*> opponentPieces = this->getOpponentPieces();

    // Loop through each of the opponents pieces. If they can move to the
    // square that that the king is on, check to ensure that there is no piece
    // blocking the check. If there isn't a piece blocking the check, the king
    // is in check.
    for (int i = 0; i < opponentPieces.size(); i++) {
        vector<Coordinate> validMoveCoordinates = opponentPieces.at(i)->ValidMoveCoordinates();

        for (int j = 0; j < validMoveCoordinates.size(); j++) {
            if (validMoveCoordinates.at(j) == square->coordinate) {
                // If the square is right in front of the pawn, the pawn cannot be giving check.
                if (opponentPieces.at(i)->type == Pawn &&
                    opponentPieces.at(i)->currentSquare->coordinate.file == square->coordinate.file) {
                    continue;
                }

                // This piece could move to the king's square. Determine if
                // there are any pieces in the way of the piece that would
                // prevevent check.
                if (!this->piecesAreInTheWay(opponentPieces.at(i), square)) {
                    return true;
                }
            }
        }
    }

    return false;
}
