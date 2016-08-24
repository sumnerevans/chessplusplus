# Tests
In the src/test-pgns folder, there are a few PGN files which can be used for testing so that you
don't have to play an entire game to reach a certain position.

1. Chess++ will not allow players to make invalid moves. This can be seen by loading the pin.pgn
   file. Note how Black is unable to move the Pawn that is blocking check from the Queen. If Black
   moves the King up, and White's Queen is not moved, the Black Pawn can move.

2. Chess++ allows castling both Queenside and Kingside. In both cases, the King should move over two
   squares and the Rook should move to the other side of the King. A Castling position can be seen
   for both sides if you load the castle.pgn file.

3. If a player achieves checkmate, the Chess++ board will close. If you load checkmate.pgn, and move
   White's Queen to capture the pawn right next to Black's King, white has won by checkmate. The
   Chess++ board will close and the result of the game will be shown in the console. (Note this was
   an actual game that I played on Chess.com where I demolished my opponent, checkmating him in 19
   moves.)
                                                
4. Chess++ supports En Passant captures. If an opponent's Pawn is moved two spaces up (on the first
   move) and the player has a Pawn that attacks the square that the opponent's Pawn jumped, the
   player can move his Pawn to the "jumped" square and the opponent's Pawn will be captured. An En
   Passant position can be seen by loading ep.pgn. White's Pawn can take Black's pawn because it
   just moved up two squares.

5. In a position where only the Kings are left on the board, the game ends in a draw. This position
   can be seen by loading king-king.pgn and capturing the one remaining Pawn. The Chess++ board
   should close and the user should be taken back to the console where "Draw by insufficient
   material" should be displayed.

6. If a player makes a move and then the other player is not in check, but also cannot make a move,
   the game ends in a draw by stalemate. This position can be seen by loading stalemate.pgn and
   moving White's King to the square right behind White's Pawn. Black is not in check, but also
   cannot make any legal move. After moving White's King, the Chess++ board should close and the
   user should be taken back to the console where "Draw by stalemate" should be displayed.
