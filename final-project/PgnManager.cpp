#include "PgnManager.h"

using namespace std;

PgnManager::PgnManager() { }

PgnManager::PgnManager(ifstream& pgnFileStream) {
    // Initialise the EncodedMoves vector with a single string.
    this->EncodedMoves = vector<string>(1);

    // Read each line. If the line has a tag pair (metadata), populate the
    // appropriate property. If it's a comment line, ignore it, if it's
    // movetext, add the moves to the EncodedMoves vector.
    string nextLine;
    while (getline(pgnFileStream, nextLine)) {
        string metadataName;
        string value;
        int spacePosition;
        int firstQuotePosition;
        int secondQuotePosition;

        switch (nextLine[0]) {
        case '[':
            // This is a tag pair line
            spacePosition = nextLine.find(' ');
            firstQuotePosition = nextLine.find('"');
            secondQuotePosition = nextLine.find('"', firstQuotePosition + 1);

            metadataName = nextLine.substr(1, spacePosition - 1);
            value = nextLine.substr(firstQuotePosition + 1, (secondQuotePosition - 1) - firstQuotePosition);

            this->populateProperty(metadataName, value);
            break;
        case '%':
            // Do nothing. This is an escape character and the rest of the line
            // should be ignored.
            break;
        default:
            // This is movetext.
            this->addMoves(nextLine);
            break;
        }
    }

    // Couldn't parse all of the data required to load the game.
    if (this->White.size() < 1 || this->Black.size() < 1 || this->Result.size() < 1) {
        throw exception("Could not parse PGN file");
    }

    if (this->Result != "*") {
        throw exception("Cannot resume completed game.");
    }
}

void PgnManager::SavePgn(const Game& game, const string filePath) {
    ofstream pgnOutputFileStream(filePath);

    // Check for errors
    if (pgnOutputFileStream.fail()) {
        cerr << "Error opening file: " << filePath << " for write" << endl;
        throw 7;
    }

    // Output information about the match.
    pgnOutputFileStream << "[Event \"Chess++ Game\"]" << endl;
    pgnOutputFileStream << "[Site \"Computer\"]" << endl;
    pgnOutputFileStream << "[Date \"" << getTodayString() << "\"]" << endl;
    pgnOutputFileStream << "[Round \"1\"]" << endl;
    pgnOutputFileStream << "[White \"" << game.Players[0].name << "\"]" << endl;
    pgnOutputFileStream << "[Black \"" << game.Players[1].name << "\"]" << endl;
    pgnOutputFileStream << "[Result \"" << getResultString(game) << "\"]" << endl;
    pgnOutputFileStream << "[Termination \"" << game.Termination << "\"]" << endl;
    pgnOutputFileStream << endl;

    // Iterate through all of the moves, outputing the movetext for each one.
    int moveNum = 0;
    for (int i = 0; i < game.Moves.size(); i++) {
        Move move = game.Moves.at(i);
        if (i % 2 == 0) {
            moveNum++;
            pgnOutputFileStream << moveNum << ".";
        }

        // If the move was a castle, output the appropriate movetext and go to
        // the next move.
        if (move.isKingsideCastle) {
            pgnOutputFileStream << "O-O ";
            continue;
        } else if (move.isQueensideCastle) {
            pgnOutputFileStream << "O-O-O ";
            continue;
        }

        char pieceChar = move.piece.GetPieceCharRepresentation();

        // pieceChar will be -1 if it is a Pawn.
        if (pieceChar > 0) {
            pgnOutputFileStream << pieceChar;
        }

        // FUTURE: Handle Ambiguous Moves
        // Occasionally, two pieces of the same type can move to the same
        // square. To solve this, there are rules around how ambiguities should
        // be resolved. These rules are extremely complicated, and the
        // ambiguities occur so rarely, that I decided it wasn't worth
        // implementing.

        // If it is a capture, note it with an "x".
        if (move.isCapture) {
            if (pieceChar == -1) {
                // This is a Pawn capture, append the Pawn's original file.
                pgnOutputFileStream << (char)(move.oldSquare->coordinate.file + 96);
            }

            pgnOutputFileStream << "x";
        }

        pgnOutputFileStream << move.newSquare->GetAlgebraicCoordinate();

        // If this move is Checkmate, append #. If it's check, append, +.
        if (move.isCheckmate) {
            pgnOutputFileStream << "#";
        } else if (move.isCheck) {
            pgnOutputFileStream << "+";
        }

        // Append a space to the end of every move
        pgnOutputFileStream << " ";
    }

    // After all of the movetext, append the game termination marker.
    pgnOutputFileStream << getResultString(game);

    // Close the file, we are done with it.
    pgnOutputFileStream.close();
}

void PgnManager::addMoves(const string moveText) {
    string moveTextWithoutMoveNumberIndications;

    for (int i = 0; i < moveText.size(); i++) {
        // If the character is a move indicator, skip it.
        if (isdigit(moveText.at(i))) {
            // The only place where two numbers will appear together is in a
            // move indicator. Check on both sides to see if the character is a
            // digit. Also, if the next character is a period, then this
            // character is part of the move indicator.
            if (i - 1 >= 0 && isdigit(moveText.at(i - 1))) {
                continue;
            } else if (i + 1 < moveText.size()) {
                if (moveText.at(i + 1) == '.' || isdigit(moveText.at(i + 1))) {
                    continue;
                }
            }
        } else if (moveText.at(i) == '.') {
            // If the character is a period, skip it.
            continue;
        }

        moveTextWithoutMoveNumberIndications.push_back(moveText.at(i));
    }

    // Split up the move text into the individualMoves vector.
    for (int i = 0; i < moveTextWithoutMoveNumberIndications.size(); i++) {
        // If the character is a space, create a new move string if there isn't
        // one already and move to the next character.
        if (moveTextWithoutMoveNumberIndications.at(i) == ' ') {
            if (this->EncodedMoves.back() != "") {
                this->EncodedMoves.push_back("");
            }

            continue;
        }

        // Append the current character to the most recent encoded move.
        this->EncodedMoves.back().push_back(moveTextWithoutMoveNumberIndications.at(i));
    }
}

// Time manipulation based on example at http://www.tutorialspoint.com/cplusplus/cpp_date_time.htm
// Used answer at http://stackoverflow.com/questions/14386923/localtime-vs-localtime-s-and-appropriate-input-arguments
// to fix deprication error.
string PgnManager::getTodayString() {
    time_t today = time(0);
    ostringstream dateString;
    struct tm timeinfo;

    localtime_s(&timeinfo, &today);
    dateString << 1900 + timeinfo.tm_year << ".";
    dateString << 1 + timeinfo.tm_mon << ".";
    dateString << timeinfo.tm_mday;

    return dateString.str();
}

string PgnManager::getResultString(const Game& game) {
    if (!game.IsComplete) {
        return "*";
    } else if (game.IsDraw) {
        return "1/2-1/2";
    } else if (game.Players[0].hasWon) {
        // The player at index 0 is always White, so white has won.
        return "1-0";
    } else {
        // If we got here, Black has won.
        return "0-1";
    }
}

void PgnManager::populateProperty(const string propertyName, const string value) {
    // We are currently only handling Event, Date, Round, White, Black, and Result.
    if (propertyName == "Event") {
        this->EventName = value;
    } else if (propertyName == "Date") {
        this->Date = value;
    } else if (propertyName == "Round") {
        this->Round = value;
    } else if (propertyName == "White") {
        this->White = value;
    } else if (propertyName == "Black") {
        this->Black = value;
    } else if (propertyName == "Result") {
        this->Result = value;
    }
}
