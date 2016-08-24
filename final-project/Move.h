#pragma once

#include "Piece.h"
#include "Square.h"

struct Move {
    Piece* capturedPiece;               // A pointer to the captured piece.
    bool isCapture = false;
    bool isCheck = false;
    bool isCheckmate = false;
    bool isKingsideCastle = false;
    bool isQueensideCastle = false;
    bool isEnPassant = false;
    bool isFirstMove = false;
    bool isPromotion = false;
    bool isValid = false;
    Square* newSquare;	                // A pointer to the old square
    Square* oldSquare;	                // A pointer to the new square
    Piece piece;		                // A copy of the piece that was moved
    Piece* piecePointer;                // A pointer to the piece that was moved
};