#include "base.h"

const std::string FileNames = "abcdefgh";
const std::string RankNames = "12345678";
const std::string PieceNames = "PNBRQK ";
const PieceType PieceTypes[] = {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NONE};
const BoardArray EmptyBoard = {"........", "........", "........", "........",
                               "........", "........", "........", "........"};
const std::string StartingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const std::string StartingBoardFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

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
