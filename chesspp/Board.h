#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "Enums.hpp"
#include "Piece.h"
#include "Square.h"

using namespace std;
using namespace sf;

// ===== Constants =====
const string ROOT_PIECE_IMG_DIR = "data/";

class Board {
public:
    // ===== Properties =====
    int boardSideLength;
    int squareSideLength;
    vector<Square*> squares;

    // ===== Piece Textures =====
    Texture whiteBishopTexture;
    Texture whiteKingTexture;
    Texture whiteKnightTexture;
    Texture whitePawnTexture;
    Texture whiteQueenTexture;
    Texture whiteRookTexture;
    Texture blackBishopTexture;
    Texture blackKingTexture;
    Texture blackKnightTexture;
    Texture blackPawnTexture;
    Texture blackQueenTexture;
    Texture blackRookTexture;

    // ===== Constructors =====
    Board();

    // Creates a board with the given side length
    Board(const int boardSideLength);

    // ===== Functions =====

    // Gets the reference to the square at a coordinate
    Square* GetSquareAtCoordinate(const Coordinate& coordinate) const;

    // Gets the square at a pixel. Note, the coordinate (0, 0) is the top-left
    // corner of the board.
    Square* GetSquareAtPixel(const int x, const int y) const;

    // Deselects the any highlighted squares then highlights the square at the
    // given square.
    void HighlightSquare(Square* square);

    // Deselects any highlighted squares
    void DeselectSquare();

    // Gets the sprites for the board and any pieces on the board.
    vector<Sprite> GetBoardSprites(const vector<Piece*>& piecesInPlay, const PlayerColour currentPlayerColour, const Vector2f offset) const;

    // Switches the board orientation. This is used to turn the board around
    // for the next player.
    void SwitchBoardOrientation();

private:
    // ===== Private Properties =====
    Square* selectedSquare;
    BoardOrientation boardOrientation;

    // ===== Private Functions =====

    // Gets the sprite for a given piece and places it at coordinate.
    Sprite getPieceSprite(const Vector2f coordinate, const Piece* piece) const;

    // Initialise the textures for the pieces
    void initialiseTextures();

    // Initialise the squares vector
    void initialiseSquares();
};

