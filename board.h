#ifndef BOARD_H
#define BOARD_H

#include "base.h"
#include "square.h"
#include "move.h"
#include "piece.h"
#include "setsqr.h"

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
    std::string FEN();
    
    // Parses a FEN and sets the board from it.
    void SetFEN(const std::string& fenStr);

    // Returns valid castling rights. This does not imply that castling is a legal move.
    std::string CastlingRights();

    // Checks if the given side can castle kingside.
    bool CanCastleKS(Color c);

    // Checks if the given side can castle queenside.
    bool CanCastleQS(Color c);
    // TODO: Assert regularity of position (other king not in check)

    // Checks if the given side attacks the given square.
    // Pinned pieces still count as attackers.
    bool IsAttackedBy(const Color& c, const Square& s);

    // Returns if the side to move is in check.
    bool IsCheck();
    //TODO: Assert regularity of position (no adjacent kings)

    // Checks if the side to move has a legal en passant.
    bool HasLegalEnPassant();

    // Returns a dynamic list of legal moves.
    std::vector<Move> PseudoLegalMoves();
    //TODO: Legalness question, pins etc.
    //TODO: Assert regularity of position (no pawns on ranks 0 and 7)

    // Returns the side to move.
    Color ToMove();
    
    friend std::ostream& operator<<(std::ostream& buf, const Board& b);
    
private:
    BoardArray board;
    Square enPassant;
    Color turn;
    uint8_t castlingRights; // bitmask (0 0 0 0 q k Q K)
    int fullMoveNumber;
    int halfMoveClock;
    
    std::vector<Move> moveStack;
    std::vector<Board> boardStack;
};

#endif //BOARD_H
