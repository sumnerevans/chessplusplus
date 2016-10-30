#include "Board.h"

using namespace std;
using namespace sf;

/*Board::~Board() {
    for (Square *s : this->squares) {
        delete s;
    }
}*/

Board::Board() { }

Board::Board(const int boardSideLength) {
    // Initialise the board's properties, piece textures, and squares.
    this->boardSideLength = boardSideLength;
    this->squareSideLength = boardSideLength / 8;
    this->boardOrientation = WhiteUp;
    this->initialiseTextures();
    this->initialiseSquares();
}

Square* Board::GetSquareAtCoordinate(const Coordinate& coordinate) const {
    for (int i = 0; i < this->squares.size(); i++) {
        if (this->squares.at(i)->coordinate == coordinate) {
            return this->squares.at(i);
        }
    }

    return NULL;
}

Square* Board::GetSquareAtPixel(const int x, const int y) const {
    Coordinate coordinate;

    // Intentionally using integer division here to determine the coordinate
    // because we don't care about the difference between the middle of the
    // square and the beginning of the square.
    if (this->boardOrientation == WhiteUp) {
        // a8 is in the top left. The files are in order left to right, the
        // ranks are in order bottom to top.

        coordinate.file = (x / this->squareSideLength) + 1;
        coordinate.rank = 8 - (y / this->squareSideLength);
    } else {
        // h1 is in the top left. The files are in order right to left, the
        // ranks are in order top to bottom.

        coordinate.file = 8 - (x / this->squareSideLength);
        coordinate.rank = (y / this->squareSideLength) + 1;
    }

    return this->GetSquareAtCoordinate(coordinate);
}

void Board::HighlightSquare(Square* square) {
    this->selectedSquare = square;
    square->Select();
}

void Board::DeselectSquare() {
    this->selectedSquare = NULL;

    for (int i = 0; i < this->squares.size(); i++) {
        this->squares.at(i)->Deselect();
    }
}

vector<Sprite> Board::GetBoardSprites(const vector<Piece*>& piecesInPlay, const PlayerColour currentPlayerColour, const Vector2f offset) const {
    vector<Sprite> boardSprites;

    // Create a square of the proper size
    IntRect renderSquare;
    renderSquare.height = this->squareSideLength;
    renderSquare.width = this->squareSideLength;

    // Loop through each rank and file (there are eight ranks and eight files)
    // see Coordinate.h or final.txt for more information about the standard
    // Chess coordinate system.
    for (int file = 1; file <= 8; file++) {
        for (int rank = 1; rank <= 8; rank++) {
            Coordinate coordinate(file, rank);
            Square* square = this->GetSquareAtCoordinate(coordinate);
            Vector2f position;

            if (this->boardOrientation == WhiteUp) {
                // a8 is in the top left. The files are in order left to right, the
;
                // ranks are in order bottom to top.

                position.x = this->squareSideLength * (square->coordinate.file - 1) + offset.x;
                position.y = this->squareSideLength * (8 - square->coordinate.rank) + offset.y;
            } else {
                // h1 is in the top left. The files are in order right to left, the
                // ranks are in order top to bottom.

                position.x = this->squareSideLength * (8 - square->coordinate.file) + offset.x;
                position.y = this->squareSideLength * (square->coordinate.rank - 1) + offset.y;
            }

            // Add Square Sprite
            Sprite squareSprite = square->GetSprite(renderSquare);
            squareSprite.setPosition(position);
            boardSprites.push_back(squareSprite);

            // Add Piece Sprite if applicable
            for (int k = 0; k < piecesInPlay.size(); k++) {
                if (piecesInPlay.at(k)->currentSquare == square) {
                    boardSprites.push_back(this->getPieceSprite(position, piecesInPlay.at(k)));
                }
            }
        }
    }

    return boardSprites;
}

void Board::SwitchBoardOrientation() {
    this->boardOrientation = this->boardOrientation == WhiteUp ? BlackUp : WhiteUp;
}

Sprite Board::getPieceSprite(const Vector2f position, const Piece* piece) const {
    Sprite sprite;

    switch (piece->type) {
    case Bishop:
        sprite.setTexture(piece->colour == White ? whiteBishopTexture : blackBishopTexture);
        break;
    case King:
        sprite.setTexture(piece->colour == White ? whiteKingTexture : blackKingTexture);
        break;
    case Knight:
        sprite.setTexture(piece->colour == White ? whiteKnightTexture : blackKnightTexture);
        break;
    case Pawn:
        sprite.setTexture(piece->colour == White ? whitePawnTexture : blackPawnTexture);
        break;
    case Queen:
        sprite.setTexture(piece->colour == White ? whiteQueenTexture : blackQueenTexture);
        break;
    case Rook:
        sprite.setTexture(piece->colour == White ? whiteRookTexture : blackRookTexture);
        break;
    }

    sprite.setPosition(position);

    return sprite;
}

void Board::initialiseTextures() {
    IntRect rectangle = IntRect(0, 0, squareSideLength, squareSideLength);
    Image bishopImage;
    Image kingImage;
    Image knightImage;
    Image pawnImage;
    Image queenImage;
    Image rookImage;

    // Load the piece images
    if (!bishopImage.loadFromFile(ROOT_PIECE_IMG_DIR + "bishop.png")) {
        cerr << "Couldn't load bishop.png!" << endl;
        throw 7;
    }

    if (!kingImage.loadFromFile(ROOT_PIECE_IMG_DIR + "king.png")) {
        cerr << "Couldn't load king.png!" << endl;
        throw 7;
    }

    if (!knightImage.loadFromFile(ROOT_PIECE_IMG_DIR + "knight.png")) {
        cerr << "Couldn't load knight.png!" << endl;
        throw 7;
    }

    if (!pawnImage.loadFromFile(ROOT_PIECE_IMG_DIR + "pawn.png")) {
        cerr << "Couldn't load bishop.png!" << endl;
        throw 7;
    }

    if (!queenImage.loadFromFile(ROOT_PIECE_IMG_DIR + "queen.png")) {
        cerr << "Couldn't load bishop.png!" << endl;
        throw 7;
    }

    if (!rookImage.loadFromFile(ROOT_PIECE_IMG_DIR + "rook.png")) {
        cerr << "Couldn't load bishop.png!" << endl;
        throw 7;
    }

    // Load textures for White
    this->whiteBishopTexture.loadFromImage(bishopImage, rectangle);
    this->whiteKingTexture.loadFromImage(kingImage, rectangle);
    this->whiteKnightTexture.loadFromImage(knightImage, rectangle);
    this->whitePawnTexture.loadFromImage(pawnImage, rectangle);
    this->whiteQueenTexture.loadFromImage(queenImage, rectangle);
    this->whiteRookTexture.loadFromImage(rookImage, rectangle);

    // Load textures for Black
    // Each image has both the white piece and the black piece, shift over to the black piece.
    rectangle.left = squareSideLength;
    this->blackBishopTexture.loadFromImage(bishopImage, rectangle);
    this->blackKingTexture.loadFromImage(kingImage, rectangle);
    this->blackKnightTexture.loadFromImage(knightImage, rectangle);
    this->blackPawnTexture.loadFromImage(pawnImage, rectangle);
    this->blackQueenTexture.loadFromImage(queenImage, rectangle);
    this->blackRookTexture.loadFromImage(rookImage, rectangle);
}

void Board::initialiseSquares() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            this->squares.push_back(new Square(i + 1, j + 1));
        }
    }
}
