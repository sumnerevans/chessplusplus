#include "Square.h"

using namespace std;
using namespace sf;

Square::Square() { }

Square::Square(const int rank, const int file) {
    this->coordinate = Coordinate(rank, file);
}

void Square::Deselect() {
    this->isSelected = false;
}

string Square::GetAlgebraicCoordinate() const {
    // Since the minimum number for a file is 1, and 1 corresponds to a, add 96
    // and convert it to a char.
    return (char)(96 + this->coordinate.file) + to_string(this->coordinate.rank);
}

Sprite Square::GetSprite(const IntRect renderSquare) const {
    Sprite squareSprite = Sprite(Texture(), renderSquare);
    squareSprite.setColor(this->getColorForSquare());
    return squareSprite;
}

void Square::Select() {
    this->isSelected = true;
}

// Colour palete from: http://www.color-hex.com/color-palette/8548
Color Square::getColorForSquare() const {
    SquareColour squareColour;

    // Determine the square's colour
    if (this->isSelected) {
        squareColour = Selected;
    } else if (this->coordinate.file % 2 == 0) {
        // This is an even file. The even ranks on even files are dark.
        squareColour = this->coordinate.rank % 2 == 0 ? Dark : Light;
    } else {
        // This is an odd file. The even ranks on odd files are light.
        squareColour = this->coordinate.rank % 2 == 0 ? Light : Dark;
    }

    // Return the appropriate Color.
    switch (squareColour) {
    case Selected:
        return Color(186, 202, 68);
    case Light:
        return Color(238, 238, 210);
    case Dark:
        return Color(118, 150, 86);
    default:
        throw 10;
    }
}