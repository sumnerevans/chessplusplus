/* CSCI 261 Final Project: Chess++
 *
 * Author: Jonathan "Sumner" Evans
 *
 * This program allows to players to play a game of chess with another player
 * on the same computer.
 *
 */

#include <iostream>
#include <SFML/Graphics.hpp>

#include "Game.h"

using namespace std;
using namespace sf;

const int BOARD_SIDE_LENGTH = 1000;

int main() {
    // Local variables for setting up the game
    int userChoice;
    bool inputValid = false;
    string player1Name, player2Name, pgnFileName;
    Game game;
    Vector2f boardOffset = Vector2f(0, 0);

    cout << "Welcome to Chess++" << endl;
    cout << "==================" << endl << endl;

    // Determine if the user wants to start a new game or load an existing one from a PGN file.
    do {
        cout << "Please select an option: " << endl;
        cout << "  [1] Start new game" << endl;
        cout << "  [2] Load game from PGN file" << endl << endl;
        cout << "Enter an option: ";
        cin >> userChoice;

        if (userChoice == 1 || userChoice == 2) {
            inputValid = true;
        } else {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid option. Please enter a 1 or 2." << endl;
        }
    } while (!inputValid);

    // Setup the game according to the user's choice.
    switch (userChoice) {
    case 1: // New Game
        cout << "Please enter name of player with White pieces: ";
        cin >> player1Name;

        cout << "Please enter name of player with Black pieces: ";
        cin >> player2Name;

        game = Game(player1Name, player2Name, BOARD_SIDE_LENGTH, boardOffset);
        break;
    case 2: // Load Game
        cout << "Please enter PGN file name: ";
        cin >> pgnFileName;

        try {
            game = Game(pgnFileName, BOARD_SIDE_LENGTH, boardOffset);
        } catch (exception ex) {
            cerr << ex.what() << endl;
            return 1;
        }
        break;
    }

    // Open a window to render Chess++ in.
    RenderWindow window(VideoMode(BOARD_SIDE_LENGTH, BOARD_SIDE_LENGTH), "Chess++");

    while (window.isOpen()) {
        window.clear();

        // If the game is over, close the window.
        if (game.IsComplete) {
            window.close();
        }

        // Get and draw each of the game's sprites.
        vector<Sprite> sprites = game.GetGameSprites();

        for (int i = 0; i < sprites.size(); i++) {
            window.draw(sprites.at(i));
        }

        window.display();

        // Check for any events 
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            } else if (event.type == Event::MouseButtonPressed) {
                if (Mouse::isButtonPressed(Mouse::Left)) {
                    Vector2i mousePosition = Mouse::getPosition(window);

                    // Let the Game handle the click event.
                    game.HandleClick(mousePosition.x, mousePosition.y);
                }
            }
        }
    }

    // If the game is over, display how it terminated.
    if (game.IsComplete) {
        cout << endl;
        cout << "================================================" << endl;
        cout << game.Termination << endl;
        cout << "================================================" << endl;
        cout << endl;
    }

    // Allow the user to save a PGN of the game.
    char saveCoice;
    string filePath;

    cout << "Would you like to save the game as a PGN file? (Y/N): ";
    cin >> saveCoice;
    if (toupper(saveCoice) == 'Y') {
        cout << "Where would you like to save the file? ";
        cin >> filePath;

        PgnManager::SavePgn(game, filePath);
    }

    return EXIT_SUCCESS;
}
