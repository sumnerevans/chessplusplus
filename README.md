# Chess++
An implementation of standard Chess in C++ using SFML.

## About
Chess++ was my final project for CSCI 261 at Colorado School of Mines during the Summer II Semester
of 2016.

Chess++ allows two players to play a standard Chess game against one another on the computer.
Gameplay follows standard Chess rules. At the start of the game, the user will have the option to
load a starting position via a Portable Game Notation (PGN) file. At the end of the game, the user
will have the option to save the game to a Portable Game Notation (PGN) file.

## Documentation
### Game Setup/Load
The setup process for Chess++ occurs in the command line. Chess++ prompts the user to choose to
either (1) start a new game or (2) load an existing game from a PGN file.

After the user chooses one of the options and enters the required information for that option
(names of the players) for the first option and PGN file name for the second option) the chess board
will be displayed.

### Gameplay
Gameplay in Chess++ follows the normal chess rules. The following types of moves are supported:

- Normal moves
- Moves that capture pieces (normal captures and En Passant captures are both supported)
- Castling
- Promotion of Pawns to Queen

Chess++ prevents illegal moves such as moving a piece (besides the Knight) through pieces or moving
a piece that is blocking check to that player's king. If the player attempts to make an illegal
move, the move will not be made and the piece they tried to move will be deselected.

### Game Termination
If a player wins by checkmate, the game is drawn by stalemate, or the game is drawn by insufficient
material\* the Chess++ game board will close bringing the user back to the command line. From there,
the user will be able to turn the see the result of the game and will be given the option to save
the game as a PGN file.

\* See Future Features -> Drawing Situations.

### Future Features
There are a few features that I would have liked to implement but could not due to time/complexity
constraints. Below are a few of those features/scenarios.

#### Pawn Promotion
It is technically legal to promote a pawn to a Knight, Bishop, Rook or Queen. Promotion to Queen is
by far the most common scenario so I decided to only implement that. If I were to implement
promotion to the other pieces, I would display a menu for the user to select the piece to promote
to.

#### Drawing Situations
If almost all of the pieces are traded off, it is possible for a situation to exist where it is
impossible for either player to create checkmate. These situations are counted as draws. Because of
the complexity of the drawing rules, and the rarity of such drawing situations, I decided not to
deal with them.

#### PGN Movetext
Occasionally, two pieces of the same type and colour can move to the same square. There are rules
about how such ambiguities should be noted in PGN movetext. These rules are extremely complicated,
and the ambiguities occur so rarely, that I decided it wasn't worth implementing.

### PGN Documenation
Portable Game Notation (PGN) format is a standard chess-game format which contains game metadata
such as the player names as well as every move made in the game. The PGN spec can be found at
https://www.chessclub.com/user/help/pgn-spec.
