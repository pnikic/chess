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
    // Creates an empty board.
    Board();

    // Creates a board from FEN.
    // e.g. Board(StartingFEN);
    Board(const std::string &longFen);

    // Copy constructor
    Board(const Board &b);

    // Destructor
    ~Board();

    // Clears all the pieces (including kings) from the board.
    void ClearBoard();

    // Returns the piece type at the given square.
    PieceType PieceTypeAt(const Square &s) const;

    // Returns the piece at the given square.
    Piece PieceAt(const Square &s) const;

    // Finds the king square of the given side. Otherwise returns a null square.
    Square King(Color c) const;

    // Removes the piece from the given square. Returns the Piece or None if the square was
    //   already empty.
    Piece RemovePieceAt(const Square &s);

    // Sets a piece at the given square. An existing piece is replaced.
    void SetPieceAt(const Square &s, const Piece &p);

    // Gets the board FEN.
    std::string FEN() const;

    // Parses a FEN and sets the board from it.
    void SetFEN(const std::string &fenStr);

    // Returns valid castling rights FEN string. This does not imply that castling is a legal move.
    std::string CastlingRights() const;

    // Checks if the given side can castle (false = queenside, true = kingside).
    bool CanCastle(Color c, bool side) const;

    // Returns all the squares which the `attackers` attack, with regard to the occupany `occupied`
    BitBoard AttackedSquares(const BitBoard &attackers, const BitBoard &occupied) const;

    // Returns all the attackers of color `c` that attack the square `s`, with regard to the
    //   occupancy `occupied`.
    BitBoard AttackersMask(const Color &c, const Square &s, const BitBoard &occupied) const;

    // Returns the number of attackers of the given square from the given side.
    // Pinned pieces still count as attackers.
    int IsAttackedBy(const Color &c, const Square &s) const;

    // Returns the number of pieces giving check to the side to move.
    int IsCheck() const;

    // Checks if the current board position is checkmate.
    bool IsCheckmate() const;

    // Checks if a piece (or pawn) from the first square is attacking the second square.
    bool IsAttacking(const Square &s, const Square &t) const;

    // Checks if a piece (or pawn) from the first square can move to the second square.
    // Helper method intending to capture move semantics of pawns
    // Note: This function is not checking castling moves
    // Note: It is not checked whether the moving piece (or pawn) is pinned or
    //   if a king walks in a check by himself or is left in check otherwise
    bool CanMovePseudoLegal(const Square &s, const Square &t) const;

    // Checks if a piece (or pawn) from the first square can legally move to the second square.
    bool CanMove(const Square &s, const Square &t) const;

    // Returns a vector of all legal moves in the position
    std::vector<Move> LegalMoves() const;

    // Make a move based on the current board and return the resulting board
    // Note: This function does not check if the move is legal nor pseudo-legal
    Board* MakeMove(const Move& m) const;

    // Return to the board previous to the current move
    Board* UnmakeMove() const;

    // Detects whether moving the piece from `s` to `t` is impossible due to a pin,
    // under assumption that this move is pseudo-legal i.e. CanMove(s, t) == True
    bool IsPinned(const Square &s, const Square &t) const;

    // Checks if the given psuedo-legal move is an en passant capture.
    bool IsEnPassant(const Move &m) const;

    // Checks if the given psuedo-legal move is a capture.
    bool IsCapture(const Move &m) const;

    // Checks if the given psuedo-legal move is a pawn move or a capture.
    bool IsZeroing(const Move &m) const;

    // Checks if the given psuedo-legal move is castling.
    bool IsCastling(const Move &m) const;

    // Checks if the given pseudo-legal move is kingside castling.
    bool IsKSCastling(const Move &m) const;

    // Checks if the given pseudo-legal move is queenside castling.
    bool IsQSCastling(const Move &m) const;

    // Checks if the given pseudo-legal move is a pawn advance for two squares.
    bool IsPawnDoublePush(const Move &m) const;

    // Returns the SAN of the given a pseudo-legal move.
    // Note: Not implemented - to be implemented by inspecting the legal moves vector
    std::string SAN(const Move &m) const;

    // Gets a bitmask of possible problems with the position.
    // Move making, generation and validation are only guaranteed to work on a valid board.
    void Status();

    // Checks if the board status is valid
    bool IsValid();

    // Returns the side to move.
    Color ToMove() const;

    bool operator==(const Board &B) const;

    friend std::ostream& operator<<(std::ostream &buf, const Board &b);

private:
    // Bitboards information
    BitBoard pawns;
    BitBoard knights;
    BitBoard bishops;
    BitBoard rooks;
    BitBoard queens;
    BitBoard kings;
    BitBoard occupiedCol[2];
    BitBoard occupied;
    BoardArray board; // to delete

    // En passant target square in algebraic notation, if any
    Square enPassant;
    // Side to move next
    Color turn;
    // Castling rights bitmask         - 0 0 0 0 q k Q K
    //   e.g. at the start of the game - 0 0 0 0 1 1 1 1 = 15
    uint8_t castlingRights;
    int halfMoveClock;
    int fullMoveNumber;
    int status;

    Move lastMove;
    Board* lastBoard;
};

#endif //BOARD_H
