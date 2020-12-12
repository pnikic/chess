#include "base.h"
#include "square.h"

#include <algorithm>

const std::string FileNames = "abcdefgh";
const std::string RankNames = "12345678";
const std::string PieceNames = "PNBRQK.";
const PieceType PieceTypes[] = {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NONE};
const std::string StartingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const std::string StartingBoardFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
const SquareType Squares[64] = {A1, B1, C1, D1, E1, F1, G1, H1, A2, B2, C2, D2, E2, F2, G2, H2,
                                A3, B3, C3, D3, E3, F3, G3, H3, A4, B4, C4, D4, E4, F4, G4, H4,
                                A5, B5, C5, D5, E5, F5, G5, H5, A6, B6, C6, D6, E6, F6, G6, H6,
                                A7, B7, C7, D7, E7, F7, G7, H7, A8, B8, C8, D8, E8, F8, G8, H8};

// Compass rose indices
//     1   2   3
//      \  |  /
//   0 <-  *  -> 4
//      /  |  \
//     7   6   5
const int dx[8] = {-1, -1, 0, 1, 1,  1,  0, -1};
const int dy[8] = { 0,  1, 1, 1, 0, -1, -1, -1};

// Compass rose indices for knight
//       1     2
//     0_|     |_3
//        \   /
//         >*<
//      7_/   \_4
//        |   |
//        6   5
const int dxN[8] = {-2, -1, 1, 2,  2,  1, -1, -2};
const int dyN[8] = { 1,  2, 2, 1, -1, -2, -2, -1};

const BitBoard BBEmpty = 0;
const BitBoard BBAll = 0xffffffffffffffff;
const BitBoard BBSquares[64] =
{
    1ull << 0, 1ull << 1, 1ull << 2, 1ull << 3, 1ull << 4, 1ull << 5, 1ull << 6, 1ull << 7,
    1ull << 8, 1ull << 9, 1ull << 10, 1ull << 11, 1ull << 12, 1ull << 13, 1ull << 14, 1ull << 15,
    1ull << 16, 1ull << 17, 1ull << 18, 1ull << 19, 1ull << 20, 1ull << 21, 1ull << 22, 1ull << 23,
    1ull << 24, 1ull << 25, 1ull << 26, 1ull << 27, 1ull << 28, 1ull << 29, 1ull << 30, 1ull << 31,
    1ull << 32, 1ull << 33, 1ull << 34, 1ull << 35, 1ull << 36, 1ull << 37, 1ull << 38, 1ull << 39,
    1ull << 40, 1ull << 41, 1ull << 42, 1ull << 43, 1ull << 44, 1ull << 45, 1ull << 46, 1ull << 47,
    1ull << 48, 1ull << 49, 1ull << 50, 1ull << 51, 1ull << 52, 1ull << 53, 1ull << 54, 1ull << 55,
    1ull << 56, 1ull << 57, 1ull << 58, 1ull << 59, 1ull << 60, 1ull << 61, 1ull << 62, 1ull << 63
};
const BitBoard aFile = 0x0101010101010101;
const BitBoard BBFiles[8] =
{
    aFile << 0, aFile << 1, aFile << 2, aFile << 3, aFile << 4, aFile << 5, aFile << 6, aFile << 7
};
const BitBoard firstRank = 0x00000000000000ff;
const BitBoard BBRanks[8] =
{
    firstRank << 0, firstRank << 8, firstRank << 16, firstRank << 24,
    firstRank << 32, firstRank << 40, firstRank << 48, firstRank << 56
};
const BitBoard BBBackRanks = BBRanks[0] | BBRanks[7];
BitBoard BBPawnAttacks[2][64];
BitBoard BBKnightAttacks[64];
BitBoard BBKingAttacks[64];
std::unordered_map<BitBoard, BitBoard> BBFileAttacks[64];
std::unordered_map<BitBoard, BitBoard> BBRankAttacks[64];
std::unordered_map<BitBoard, BitBoard> BBDiagAttacks[64];

BitBoard ScanForward(BitBoard &b)
{
    BitBoard r = b & -b;
    b ^= r;
    return r;
}

BitBoard Edges(const Square &sq)
{
    // Bitboard of edges (a- and h- file, 1st and 8th rank)
    //   excluding the one(s) on which the square is residing

    return ((BBRanks[0] | BBRanks[7]) & ~BBRanks[sq.Rank()]) |
        ((BBFiles[0] | BBFiles[7]) & ~BBFiles[sq.File()]);
}

void PrecomputePawnAttacks()
{
    for (int color = 0; color < 2; ++color)
    {
        int k = color ? 4 : 0;

        for (int s = 0; s < 64; ++s)
        {
            Square sq(static_cast<SquareType>(s));
            BitBoard BB = BBEmpty;

            for (int i : {k + 1, k + 3})
            {
                int xx = sq.Rank() + dy[i], yy = sq.File() + dx[i];
                if (Legal(xx, yy))
                    BB |= ToBBSquare(xx, yy);
            }

            BBPawnAttacks[color][s] = BB;
        }
    }
}

void PrecomputeNonSlidingPieceAttacks(BitBoard *storage, const int *dx, const int *dy)
{
    for (int s = 0; s < 64; ++s)
    {
        Square sq(static_cast<SquareType>(s));
        BitBoard BB = BBEmpty;

        for (int i = 0; i < 8; ++i)
        {
            int xx = sq.Rank() + dx[i], yy = sq.File() + dy[i];
            if (Legal(xx, yy))
            {
                BB |= ToBBSquare(xx, yy);
            }
        }

        storage[s] = BB;
    }
}

BitBoard SlidingAttacks(Square sq, int startDirInd, int stride, BitBoard occupied = 0)
{
    // Return the attacked squares from the sliding piece at `sq` that uses directions starting
    //   from `startDirInd` with a skip of `stride`. `occupied` is the occupancy bitboard.

    BitBoard BB = BBEmpty;
    for (int i = startDirInd; i < 8; i += stride)
    {
        for (int k = 1; k < 8; ++k)
        {
            int xx = sq.Rank() + k * dx[i], yy = sq.File() + k * dy[i];
            if (Legal(xx, yy))
            {
                BitBoard bbsq = ToBBSquare(xx, yy);
                BB |= bbsq;
                if (bbsq & occupied)
                    break;
            }
            else
            {
                break;
            }
        }
    }

    return BB;
}

inline BitBoard CarryRippler(BitBoard n, BitBoard d)
{
    // Carry-Rippler trick to enumerate all subsets of `d`;
    // Usage: repeatedly call on the return-value and d starting with n = 0,
    //   while the function does not return 0 again

    return (n - d) & d;
}

void PrecomputeSlidingPieceAttacks(std::unordered_map<BitBoard, BitBoard> *storage,
                                   int startDirInd, int stride)
{
    for (int s = 0; s < 64; ++s)
    {
        Square sq(static_cast<SquareType>(s));
        BitBoard BB = SlidingAttacks(sq, startDirInd, stride);

        // The most outer squares are redundant as there are no more ray-squares behind
        BB &= ~Edges(sq);

        // Save the "blockers & beyond" mask using the key of the full board
        storage[s][BBAll] = BB;

        // Iterate through all possible positions of other pieces
        BitBoard n = 0;
        do
        {
            storage[s][n] = SlidingAttacks(sq, startDirInd, stride, n);
            n = CarryRippler(n, BB);
        } while (n);
    }
}

bool piecesAttacksPrecomputed = false;
void PrecomputePiecesAttacks()
{
    if (piecesAttacksPrecomputed)
        return;

    PrecomputePawnAttacks();
    PrecomputeNonSlidingPieceAttacks(BBKingAttacks, dx, dy);
    PrecomputeNonSlidingPieceAttacks(BBKnightAttacks, dxN, dyN);
    PrecomputeSlidingPieceAttacks(BBFileAttacks, 0, 4);
    PrecomputeSlidingPieceAttacks(BBRankAttacks, 2, 4);
    PrecomputeSlidingPieceAttacks(BBDiagAttacks, 1, 2);
    piecesAttacksPrecomputed = true;
}

SquareType ToSquare(int rank, int file)
{
    return static_cast<SquareType>(rank * 8 + file);
}

SquareType ToSquare(int id)
{
    return static_cast<SquareType>(id);
}

Color Switch(Color c)
{
    return static_cast<Color>(1 - c);
}

bool Legal(int a)
{
    return a >= 0 && a < 8;
}

bool Legal(int a, int b)
{
    return Legal(a) && Legal(b);
}

std::string trim(const std::string &str, const std::string &ws /* = " \t" */)
{
    size_t first = str.find_first_not_of(ws);
    if (first == std::string::npos)
        return "";

    size_t last = str.find_last_not_of(ws);
    return str.substr(first, last - first + 1);
}

std::string reduce(const std::string &str,
                   const std::string &fill /* = " " */,
                   const std::string &ws /* = " \t" */)
{
    std::string result = trim(str, ws);

    size_t first = result.find_first_of(ws);
    while (first != std::string::npos)
    {
        size_t end = result.find_first_not_of(ws, first);
        result.replace(first, end - first, fill);
        first = result.find_first_of(ws, first + fill.length());
    }

    return result;
}
