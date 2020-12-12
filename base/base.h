#ifndef NDEBUG
#define CRASH(msg) assert(false && msg);
#define ASSERT(cond, msg) assert((cond) && msg)
#else
#define CRASH(msg) do { (void)sizeof(msg); } while(0)
#define ASSERT(cond, msg) do { (void)sizeof(msg); } while(0)
#endif

#ifndef BASE_H
#define BASE_H

#include <array>
#include <cassert>
#include <unordered_map>
#include <string>

class Square;

// Global definitions
enum PieceType { PAWN = 0, KNIGHT = 1, BISHOP = 2, ROOK = 3, QUEEN = 4, KING = 5, NONE = 6 };
enum Color { WHITE = 0, BLACK = 1 };
enum SquareType
{
    A1 = 0, B1 = 1, C1 = 2, D1 = 3, E1 = 4, F1 = 5, G1 = 6, H1 = 7,
    A2 = 8, B2 = 9, C2 = 10, D2 = 11, E2 = 12, F2 = 13, G2 = 14, H2 = 15,
    A3 = 16, B3 = 17, C3 = 18, D3 = 19, E3 = 20, F3 = 21, G3 = 22, H3 = 23,
    A4 = 24, B4 = 25, C4 = 26, D4 = 27, E4 = 28, F4 = 29, G4 = 30, H4 = 31,
    A5 = 32, B5 = 33, C5 = 34, D5 = 35, E5 = 36, F5 = 37, G5 = 38, H5 = 39,
    A6 = 40, B6 = 41, C6 = 42, D6 = 43, E6 = 44, F6 = 45, G6 = 46, H6 = 47,
    A7 = 48, B7 = 49, C7 = 50, D7 = 51, E7 = 52, F7 = 53, G7 = 54, H7 = 55,
    A8 = 56, B8 = 57, C8 = 58, D8 = 59, E8 = 60, F8 = 61, G8 = 62, H8 = 63,
    NS = 64 // NS = NULL SQUARE
};
enum Status
{
    VALID = 0,
    NO_WHITE_KING = 1 << 0,
    NO_BLACK_KING = 1 << 1,
    TOO_MANY_KINGS = 1 << 2,
    TOO_MANY_WHITE_PAWNS = 1 << 3,
    TOO_MANY_BLACK_PAWNS = 1 << 4,
    PAWNS_ON_BACK_RANK = 1 << 5,
    TOO_MANY_WHITE_PIECES = 1 << 6,
    TOO_MANY_BLACK_PIECES = 1 << 7,
    INVALID_CASTLING_RIGHTS = 1 << 8,
    INVALID_EP_SQUARE = 1 << 9,
    OPPOSITE_CHECK = 1 << 10,
    EMPTY = 1 << 11,
    TOO_MANY_CHECKERS = 1 << 12,
    ADJACENT_KINGS = 1 << 13
};

using BoardArray = std::array<std::string, 8>; // delete
using BitBoard = uint64_t;

// Global variables (extern is for external linkage)
extern const SquareType Squares[64];
extern const std::string FileNames;
extern const std::string RankNames;
extern const std::string PieceNames;
extern const PieceType PieceTypes[];
extern const std::string StartingFEN;
extern const std::string StartingBoardFEN;
extern const int dxN[8];
extern const int dyN[8];
extern const int dx[8];
extern const int dy[8];

// Bitboard utilities
extern const BitBoard BBEmpty, BBAll;
extern const BitBoard BBSquares[64];
extern const BitBoard BBFiles[8];
extern const BitBoard BBRanks[8];
extern const BitBoard BBBackRanks;
// Return and turn off the lowest active bit in `b`
// Note: this function is modifiying the original BitBoard
BitBoard ScanForward(BitBoard &b);

// Precomputed squares attacked by a non-sliding piece
extern BitBoard BBPawnAttacks[2][64];
extern BitBoard BBKnightAttacks[64];
extern BitBoard BBKingAttacks[64];
// Precomputed squares attacked by a sliding piece according to board occupancy.
//   For a given map (square), the map keys are occupancy "blockers & beyond" bitboards of the
//   file / rank / diagonal. Useable keys are calculated by ANDing the actual occupancy bitboard
//   with BB{File,Rank,Diag}Attacks[BBAll] (the "blockers & beyond" mask)
extern std::unordered_map<BitBoard, BitBoard> BBFileAttacks[64];
extern std::unordered_map<BitBoard, BitBoard> BBRankAttacks[64];
extern std::unordered_map<BitBoard, BitBoard> BBDiagAttacks[64];

void PrecomputePiecesAttacks();

inline int LSB(BitBoard b)
{
    return b ? __builtin_ctzll(b) : -1;
}

inline int MSB(BitBoard b)
{
    return b ? sizeof(uint64_t) * 8 - __builtin_clzll(b) - 1 : -1;
}

inline int countOnes(BitBoard b)
{
    return __builtin_popcountll(b);
}

inline BitBoard ToBBSquare(int rank, int file)
{
    return BBSquares[rank * 8 + file];
}


inline BitBoard ShiftUp(BitBoard b)
{
    return b << 8;
}

inline BitBoard ShiftDown(BitBoard b)
{
    return b >> 8;
}

inline BitBoard ShiftLeft(BitBoard b)
{
    return (b >> 1) & ~BBFiles[7];
}

inline BitBoard ShiftRight(BitBoard b)
{
    return (b << 1) & ~BBFiles[0];
}

// General utilities
SquareType ToSquare(int rank, int file);
SquareType ToSquare(int id);
Color Switch(Color c);
bool Legal(int a);
bool Legal(int a, int b);

// String utilities
std::string trim(const std::string &str, const std::string &ws = " \t");
std::string reduce(const std::string &str, const std::string &fill = " ", const std::string &whitespace = " \t");

// Custom exception handling class
class Exception : public std::exception
{
public:
    Exception(const std::string &m) : m(m) {}
    virtual const char* what() const throw()
    {
        return m.c_str();
    }
private:
    std::string m;
};

#endif //BASE_H
