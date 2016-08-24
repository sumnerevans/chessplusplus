#pragma once

#include <fstream>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "Board.h"
#include "Enums.hpp"
#include "Square.h"
#include "Move.h"
#include "PgnManager.h"
#include "Player.h"

using namespace std;
using namespace sf;

class Game {
public:
    // ===== Properties =====
    bool IsComplete = false;
    bool IsDraw = false;
    vector<Move> Moves;
    Player Players[2];
    string Termination = "Incomplete";

    // ===== Constructors =====
    Game();

    // Loads an existing game from a PGN file.
    Game(const string pgnFileName, const int boardSideLength, const Vector2f boardOffset);

    // Loads a new game given the player's names.
    Game(const string player1Name, const string player2Name, const int boardSideLength, const Vector2f boardOffset);

    // ===== Functions =====

    // Handles a user's click on the window. If the click was on the board,
    // it moves the piece or (de)selects the piece as required.
    void HandleClick(const int x, const int y);

    // Gets the sprites for the game. Currently, this only gets the board's
    // sprites, but in the future, it could get sprites for other UI elements
    // such as information about the players, captured pieces, etc.
    vector<Sprite> GetGameSprites() const;

private:
    // ===== Private Properties =====
    Board board;
    Vector2f boardOffset;
    PlayerColour currentPlayerColour = White;
    vector<Piece*> pieces;

    // ===== Private Functions =====

    // Determines if the castle is valid.
    bool canCastle(const Square* newSquare) const;

    // Finishes the player's turn, checking for checkmate or a draw.
    void completeTurn();

    // Deselects the given piece.
    void deselectPiece(Piece* piece);

    // Gets the rook that would castle.
    Piece* getCastlingRook(const bool isKingsideCastle, const bool isQueensideCastle) const;

    // Gets the square that the rook will land on after castling.
    Square* getCastlingRookFinalSquare(const bool isKingsideCastle, const bool isQueensideCastle) const;

    // Gets the square that the king is on.
    Piece* getCurrentPlayersKing() const;

    // Gets the square that the king is on.
    Square* getCurrentPlayersKingsSquare() const;

    vector<Piece*> getCurrentPlayersPieces() const;

    // Gets all of the opponents pieces.
    vector<Piece*> getOpponentPieces() const;

    // Gets all of the non-captured pieces.
    vector<Piece*> getPiecesInPlay() const;

    // Gets the piece on the given square.
    Piece* getPieceOnSquare(const Square* square) const;

    // Gets the selected piece.
    Piece* getSelectedPiece() const;

    // Initialises the game, adding players,
    void initialise(const string player1Name, const string player2Name, const int boardSideLength, const Vector2f boardOffset);

    // Determines if insufficient material exists (partially implemented, see
    // implementation or final.txt for details.
    bool insufficientMaterialExists() const;

    // Determines if a move is valid.
    bool moveIsValid(Piece* piece, Square* newSquare, bool& isKingsideCastle, bool& isQueensideCastle);

    // Performs the actual move.
    void movePiece(Piece* piece, Square* newSquare);

    // Verifies that the move is valid then, if it is valid, moves the piece.
    bool movePieceIfValid(Piece* piece, Square* newSquare);

    // Determines if there are pieces in between a piece and a destination square
    bool piecesAreInTheWay(const Piece* piece, const Square* destination) const;

    // Sets up all of the major and minor p
    void placeMinorAndMajorPieces(const PlayerColour colour);

    // Sets up all of the pawns for a given colour.
    void placePawns(const PlayerColour colour);

    // Sets up all of the pieces in the game.
    void placePieces();

    // Determines if the player has any valid moves (moves that will not result
    // in check)
    bool playerHasValidMoves();

    // Populates a Move struct with data about the piece, square, and castling status.
    Move populateMove(Piece* piece, Square* newSquare, const bool isKingsideCastle, const bool isQueensideCastle);

    // Undoes a Move, setting everything back to its original state.
    void revertMove(Move move);

    // Determines if the current player's king is in check.
    bool squareIsAttacked(const Square* square) const;
};

