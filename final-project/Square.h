#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "Coordinate.h"
#include "Enums.hpp"

using namespace std;
using namespace sf;

class Square {
public:
    // ===== Properties =====
    Coordinate coordinate;
    bool isSelected = false;
    bool hasPiece = false;

    // ===== Constructors =====
    Square();

    // Initialises a square with a given rank and file.
    Square(const int rank, const int file);

    // ===== Functions =====

    // Deselects the square.
    void Deselect();

    // Gets the aglebraic coordinate string for the square. Examples are: h3, b2, c7.
    string GetAlgebraicCoordinate() const;

    // Gets the Sprite for the square.
    Sprite GetSprite(const IntRect renderSquare) const;
    void Select();

    // ===== Equality Operators =====
    bool operator==(const Square& rhs) const {
        return coordinate == rhs.coordinate;
    }

    bool operator==(Square* rhs) const {
        if (rhs == NULL) {
            return false;
        }

        return (coordinate.file == rhs->coordinate.file) && (coordinate.rank == rhs->coordinate.rank);
    }

private:
    // ===== Private Functions =====

    // Gets the Color for the square.
    Color getColorForSquare() const;
};