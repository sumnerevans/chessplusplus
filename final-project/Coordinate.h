#pragma once

#include <string>

using namespace std;

// Chess uses an Algebraic Coordinate System which works like this:
// When looking at the board from White's position, each column of squares from
// left to right is called a File. Files are denoted by the letter a-h. Each row
// from bottom to top is called a Rank. Ranks are denoted by a number with the
// first rank being the one closest to the player (the rank with White's King).
// To denote a coordinate, the File character and the rank number are
// concatenated. Some examples are: d3 (4th column, 3 rows up), b7 (2nd column,
// 7 rows up), d8 (4th column, 8 rows up)
class Coordinate {
public:
    int file; // Vertical column
    int rank; // Horizontal row

    // Define the equivalence operator for Coordinate
    bool operator==(const Coordinate& rhs) const {
        return (file == rhs.file) && (rank == rhs.rank);
    }

    // ===== Constructors =====
    Coordinate() { }
    Coordinate(const int file, const  int rank) {
        this->file = file;
        this->rank = rank;
    }

    // See above for examples
    Coordinate(const string coordinateString) {
        this->file = (int)coordinateString[0] - 96;
        this->rank = (int)coordinateString[1] - 48;
    }
};