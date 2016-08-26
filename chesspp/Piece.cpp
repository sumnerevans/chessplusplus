#include "Piece.h"

using namespace std;

Piece::Piece() { }

Piece::Piece(const PlayerColour colour, Square* startingSquare, const PieceType type) {
    this->colour = colour;
    this->currentSquare = startingSquare;
    this->type = type;
}

void Piece::Capture() {
    this->isCaptured = true;
}

bool Piece::CoordinateIsOnSameDiagonal(const Coordinate& newCoordinate) const {
    // Note 8 is the max distance on a diagonal
    for (int i = 0; i < 8; i++) {
        if (newCoordinate.file + i == this->currentSquare->coordinate.file &&
            newCoordinate.rank + i == this->currentSquare->coordinate.rank) {
            // The newCoordinate is up to the right (when looking at the board from White's side).
            return true;
        } else if (newCoordinate.file - i == this->currentSquare->coordinate.file &&
            newCoordinate.rank - i == this->currentSquare->coordinate.rank) {
            // The newCoordinate is down to the left (when looking at the board from White's side).
            return true;
        } else if (newCoordinate.file + i == this->currentSquare->coordinate.file &&
            newCoordinate.rank - i == this->currentSquare->coordinate.rank) {
            // The newCoordinate is down to the right (when looking at the board from White's side).
            return true;
        } else if (newCoordinate.file - i == this->currentSquare->coordinate.file &&
            newCoordinate.rank + i == this->currentSquare->coordinate.rank) {
            // The newCoordinate is up to the left (when looking at the board from White's side).
            return true;
        }
    }

    return false;
}

bool Piece::CoordinateIsOnSameFile(const Coordinate& newCoordinate) const {
    return newCoordinate.file == this->currentSquare->coordinate.file;
}

bool Piece::CoordinateIsOnSameRank(const Coordinate& newCoordinate) const {
    return newCoordinate.rank == this->currentSquare->coordinate.rank;
}

Piece Piece::Copy() {
    Piece piece;
    piece.capturableEnPassant = this->capturableEnPassant;
    piece.colour = this->colour;
    piece.currentSquare = this->currentSquare;
    piece.hasMoved = this->hasMoved;
    piece.isCaptured = this->isCaptured;
    piece.isSelected = this->isSelected;
    piece.type = this->type;

    return piece;
}

char Piece::GetPieceCharRepresentation() const {
    switch (this->type) {
    case Pawn:
        return -1;
    case Knight:
        return 'N';
    case Bishop:
        return 'B';
    case Rook:
        return 'R';
    case Queen:
        return 'Q';
    case King:
        return 'K';
    }
}

void Piece::Promote() {
    if (this->type != Pawn) {
        throw 8;
    }

    this->type = Queen;
}

vector<Coordinate> Piece::ValidMoveCoordinates() const {
    vector<Coordinate> possibleMoveCoords;

    int currentRank = this->currentSquare->coordinate.rank;
    int currentFile = this->currentSquare->coordinate.file;

    // White pawns must move up the ranks (they start at the second rank
    // and go towards the eighth rank). Black pawns go the opposite direction.
    int moveDirection = this->colour == White ? 1 : -1;

    switch (this->type) {
    case Pawn:
        //The pawn can capture to the left.
        possibleMoveCoords.push_back(Coordinate(currentFile - 1, currentRank + moveDirection));

        // The pawn can always push forward one square. (The validation that
        // of whether or not there is a piece in the way is done elsewhere.)
        possibleMoveCoords.push_back(Coordinate(currentFile, currentRank + moveDirection));

        // The pawn can capture to the right.
        possibleMoveCoords.push_back(Coordinate(currentFile + 1, currentRank + moveDirection));


        // If the pawn hasn't been moved yet, it can be moved up two ranks.
        if (!this->hasMoved) {
            possibleMoveCoords.push_back(Coordinate(currentFile, currentRank + (moveDirection * 2)));
        }
        break;
    case Knight:
        // Knight can make L moves (two squares in one direction, one in the
        // other). There are only eight possibilities, so just add them
        // individually.
        possibleMoveCoords.push_back(Coordinate(currentFile + 2, currentRank + 1));
        possibleMoveCoords.push_back(Coordinate(currentFile - 2, currentRank + 1));
        possibleMoveCoords.push_back(Coordinate(currentFile + 2, currentRank - 1));
        possibleMoveCoords.push_back(Coordinate(currentFile - 2, currentRank - 1));

        possibleMoveCoords.push_back(Coordinate(currentFile + 1, currentRank + 2));
        possibleMoveCoords.push_back(Coordinate(currentFile - 1, currentRank + 2));
        possibleMoveCoords.push_back(Coordinate(currentFile + 1, currentRank - 2));
        possibleMoveCoords.push_back(Coordinate(currentFile - 1, currentRank - 2));
        break;
    case Bishop:
        // Loop through each coordinate. If the coordinate is on the same
        // diagonal, the Bishop could move there.
        for (int file = 1; file <= 8; file++) {
            for (int rank = 1; rank <= 8; rank++) {
                Coordinate coordinate = Coordinate(file, rank);

                if (this->CoordinateIsOnSameDiagonal(coordinate)) {
                    possibleMoveCoords.push_back(coordinate);
                }
            }
        }
        break;
    case Rook:
        // Rooks can move up and down and left and right. Add all of the squares
        // on each rank and file.
        for (int i = 1; i <= 8; i++) {
            possibleMoveCoords.push_back(Coordinate(this->currentSquare->coordinate.file, i));
            possibleMoveCoords.push_back(Coordinate(i, this->currentSquare->coordinate.rank));
        }
        break;
    case Queen:
        // Loop through each of the squares. If the coordinate is on the same
        // diagonal or rank or file, then the Queen can go to that coordinate.
        for (int file = 1; file <= 8; file++) {
            for (int rank = 1; rank <= 8; rank++) {
                Coordinate coordinate = Coordinate(file, rank);

                if (this->CoordinateIsOnSameDiagonal(coordinate) ||
                    this->CoordinateIsOnSameRank(coordinate) ||
                    this->CoordinateIsOnSameFile(coordinate)) {
                    possibleMoveCoords.push_back(coordinate);
                }
            }
        }
        break;
    case King:
        // Allow castling
        if (!this->hasMoved) {
            possibleMoveCoords.push_back(Coordinate(currentFile - 2, currentRank));
            possibleMoveCoords.push_back(Coordinate(currentFile + 2, currentRank));
        }

        // Squares above the King
        possibleMoveCoords.push_back(Coordinate(currentFile - 1, currentRank + 1));
        possibleMoveCoords.push_back(Coordinate(currentFile, currentRank + 1));
        possibleMoveCoords.push_back(Coordinate(currentFile + 1, currentRank + 1));

        // Squares to the sides of the King
        possibleMoveCoords.push_back(Coordinate(currentFile - 1, currentRank));
        possibleMoveCoords.push_back(Coordinate(currentFile + 1, currentRank));

        // Squares below the King
        possibleMoveCoords.push_back(Coordinate(currentFile - 1, currentRank - 1));
        possibleMoveCoords.push_back(Coordinate(currentFile, currentRank - 1));
        possibleMoveCoords.push_back(Coordinate(currentFile + 1, currentRank - 1));
    }

    return possibleMoveCoords;
}