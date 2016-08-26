#pragma once

#include <string>

#include "Enums.hpp"

using namespace std;

struct Player {
    PlayerColour colour;
    bool hasWon = false;
    string name;
};