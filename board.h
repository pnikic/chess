#ifndef BOARD_H
#define BOARD_H

#include "base.h"
#include "square.h"
#include "move.h"
#include "piece.h"
#include "setsqr.h"
#include <vector>

class Board
{
public:    
    // Creates a board with the starting position.
    Board();

    // Creates an empty board.
    // Board(...);
    
    // Creates a board from FEN.
    Board(const std::string& longFen);
    
    Board(const Board& b);

    // Clears all the pieces (including kings) from the board.
    void ClearBoard();
    
    // Gets pieces of the given type and color.
    SetSquares Pieces(const Piece& p);

    // Returns the piece at the given square.
    Piece PieceAt(const Square& s);

    // Returns the piece type at the given square.
    PieceType PieceTypeAt(const Square& s);
    
    // Finds the king square of the given side.
    Square King(Color c);
    
    // Removes the piece from the given square. Returns the Piece or None if the square was already empty.
    Piece RemovePieceAt(const Square& s);
    
    // Sets a piece at the given square. An existing piece is replaced.
    void SetPieceAt(const Square& s, const Piece& p);
    
    // Gets the board FEN.
    std::string BoardFen();
    
    // Parses a FEN and sets the board from it.
    void SetBoardFen(const std::string& fenStr);

    // Returns (not necessary valid) castling rights.
    std::string CastlingRights();
    
    friend std::ostream& operator<<(std::ostream& buf, const Board& b);
    
private:
    BoardArray board;
    Square enPassant;
    Color turn;
    uint8_t castlingRights; // bitmask (0 0 0 0 q k Q K)
    int fullMoveNumber;
    int halfMoveClock;
    
    //std::vector<Move> moveStack;
};

#endif //BOARD_H
