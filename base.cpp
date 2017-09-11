#include "base.h"

const std::string FileNames = "abcdefgh";
const std::string RankNames = "12345678";
const std::string PieceNames = "PNBRQK ";
const PieceType PieceTypes[] = {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NONE};
const BoardArray EmptyBoard = {"........", "........", "........", "........",
                               "........", "........", "........", "........"};
const std::string StartingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const std::string StartingBoardFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
const SquareType Squares[64] = {A1, B1, C1, D1, E1, F1, G1, H1, A2, B2, C2, D2, E2, F2, G2, H2,
                              A3, B3, C3, D3, E3, F3, G3, H3, A4, B4, C4, D4, E4, F4, G4, H4,
                              A5, B5, C5, D5, E5, F5, G5, H5, A6, B6, C6, D6, E6, F6, G6, H6,
                              A7, B7, C7, D7, E7, F7, G7, H7, A8, B8, C8, D8, E8, F8, G8, H8};

const int dxN[8] = {-2, -1, 1, 2,  2,  1, -1, -2};
const int dyN[8] = { 1,  2, 2, 1, -1, -2, -2, -1};
const int dx[8] = {-1, -1, 0, 1, 1,  1,  0, -1};
const int dy[8] = { 0,  1, 1, 1, 0, -1, -1, -1};

std::string trim(const std::string& str, const std::string& ws /* = " \t" */)
{
    size_t first = str.find_first_not_of(ws);
    if (first == std::string::npos)
        return "";

    size_t last = str.find_last_not_of(ws);
    return str.substr(first, last - first + 1);
}

std::string reduce(const std::string& str,
                   const std::string& fill /* = " " */,
                   const std::string& ws /* = " \t" */)
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

bool Legal(int elem)
{
    if (elem >= 0 && elem < 8)
        return true;
    else
        return false;
}
