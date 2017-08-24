#ifndef BOARD_H
#define BOARD_H

#include "base.h"
#include "square.h"
#include "piece.h"

class Board
{
public:
    Board();
    // Create an empty board;
    Board(const std::string& longFen);
    Board(std::string shortFen, ...);
    Board(const Board& b);
    ~Board();
    
    void ClearBoard();
    Piece* Pieces(const Piece& p); // Gets pieces of the given type and color
    Piece PieceAt(const Square& s);
    PieceType PieceTypeAt(const Square& s);
    Square King(const Color& c);   // Finds the king square of the given side
    Piece RemovePieceAt(const Square& s); // Removes the piece from the given square. Returns the Piece or None if the square was already empty.
    Piece SetPieceAt(const Square& s); //     Sets a piece at the given square. An existing piece is replaced. Setting piece to None is equivalent to remove_piece_at().
    std::string BoardFen(); // Gets the board FEN
    void SetBoardFen(const std::string& f); // Parses a FEN and sets the board from it. Raises:	ValueError if the FEN string is invalid.
    
private:
    std::string fen;
    std::string board[8];
    Square enPassant;
    Color turn;
    uint8_t castlingRights; // bitmask (0 0 0 0 q k Q K)
    int fullMoveNumber;
    int halfMoveClock;

};

#endif //BOARD_H
