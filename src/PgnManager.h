#pragma once

#include <ctime>
#include <string>
#include <sstream>
#include <vector>

#include "Enums.hpp"
#include "Game.h"

using namespace std;

// Forward declaration of Game because we have circular dependencies.
class Game;

class PgnManager {
public:
    // ===== Properties =====
    // These will be populated via the one-parameter constructor.
    string EventName;
    string Date;
    string Round;
    string White;
    string Black;
    string Result;
    vector<string> EncodedMoves;

    // ===== Constructors =====
    PgnManager();
    PgnManager(ifstream& pgnFileStream);

    // ===== Functions =====

    // Saves a PGN for the given game to the given filePath.
    static void SavePgn(const Game& game, const string filePath);

private:
    // ===== Private Functions =====

    // Adds to the EncodedMoves vector from movetext.
    void addMoves(const string moveText);

    // Gets a string with todays date, formatted "2016.07.30".
    static string getTodayString();

    // Gets a string with the result of the game.
    // Possible options are:
    // "*" - incomplete game
    // "1/2-1/2" - draw
    // "1-0" - white won
    // "0-2" - black won
    static string getResultString(const Game& game);

    // Populates the given property with the value. If a property with the
    // given propertyName does not exist, nothing will happen.
    void populateProperty(const string propertyName, const string value);
};

