#pragma once

#include <vector>

#include "Enums.hpp"
#include "Square.h"

using namespace std;

class Piece {
public:
    // ===== Properties =====
    bool capturableEnPassant = false;
    PlayerColour colour;
    Square* currentSquare;
    bool hasMoved = false;
    bool isCaptured = false;
    bool isSelected = false;
    PieceType type;

    // ===== Constructors =====
    Piece();
    Piece(const PlayerColour colour, Square* startingSquare, const PieceType type);

    // ===== Functions =====

    // Captures the piece.
    void Capture();

    // True if the given coordinate is on the same diagonal as the piece.
    bool CoordinateIsOnSameDiagonal(const Coordinate& newSquare) const;

    // True if the given coordinate is on the same file as the piece.
    bool CoordinateIsOnSameFile(const Coordinate& newSquare) const;

    // True if the given coordinate is on the same rank as the piece.
    bool CoordinateIsOnSameRank(const Coordinate& newSquare) const;

    // Creates a copy of the piece.
    Piece Copy();

    // Gets the char-representation of the piece (-1 if a pawn, which has no
    // char representation).
    char GetPieceCharRepresentation() const;

    // Promotes the piece to a queen.
    void Promote();

    // Gets the valid move coordinates for the piece. (This function ignores
    // pieces that are in the way.)
    vector<Coordinate> ValidMoveCoordinates() const;
};

