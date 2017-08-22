#include "base.h"

const std::string FileNames = "abcdefgh";
const std::string RankNames = "12345678";
const std::string PieceNames = "PNBRQK ";
const PieceType PieceTypes[] = {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NONE};
const std::string EmptyBoard[]= {"........", "........", "........", "........", "........", "........", "........", "........"};
const std::string StartingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const std::string StartingBoardFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
