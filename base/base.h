#ifndef NDEBUG
#define CRASH(msg) assert(false && msg);
#define ASSERT(cond, msg) assert((cond) && msg)
#else
#define CRASH(msg) do { (void)sizeof(msg); } while(0)
#define ASSERT(cond, msg) do { (void)sizeof(msg); } while(0)
#endif

#ifndef BASE_H
#define BASE_H

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

enum PieceType { PAWN = 0, KNIGHT = 1, BISHOP = 2, ROOK = 3, QUEEN = 4, KING = 5, NONE = 6 };
enum Color { WHITE = 0, BLACK = 1 };
enum SquareType { A1 = 0, B1 = 1, C1 = 2, D1 = 3, E1 = 4, F1 = 5, G1 = 6, H1 = 7,
                  A2 = 8, B2 = 9, C2 = 10, D2 = 11, E2 = 12, F2 = 13, G2 = 14, H2 = 15,
                  A3 = 16, B3 = 17, C3 = 18, D3 = 19, E3 = 20, F3 = 21, G3 = 22, H3 = 23,
                  A4 = 24, B4 = 25, C4 = 26, D4 = 27, E4 = 28, F4 = 29, G4 = 30, H4 = 31,
                  A5 = 32, B5 = 33, C5 = 34, D5 = 35, E5 = 36, F5 = 37, G5 = 38, H5 = 39,
                  A6 = 40, B6 = 41, C6 = 42, D6 = 43, E6 = 44, F6 = 45, G6 = 46, H6 = 47,
                  A7 = 48, B7 = 49, C7 = 50, D7 = 51, E7 = 52, F7 = 53, G7 = 54, H7 = 55,
                  A8 = 56, B8 = 57, C8 = 58, D8 = 59, E8 = 60, F8 = 61, G8 = 62, H8 = 63, NS = 64 };
                  // NS = NULL SQUARE
enum Status { Valid = 0, NoWhiteKing = 1 << 0, NoBlackKing = 1 << 1, TooManyKings = 1 << 2,
              TooManyWhitePawns = 1 << 3, TooManyBlackPawns = 1 << 4, PawnsOnBackRank = 1 << 5,
              TooManyWhitePieces = 1 << 6, TooManyBlackPieces = 1 << 7, AdjacentKings = 1 << 8,
              InvalidEpSquare = 1 << 9, OppositeCheck = 1 << 10, Empty = 1 << 11 };

typedef std::array<std::string, 8> BoardArray;

extern const SquareType Squares[64];
extern const std::string FileNames;
extern const std::string RankNames;
extern const std::string PieceNames;
extern const PieceType PieceTypes[];
extern const std::string StartingFEN;
extern const std::string StartingBoardFEN;
extern const BoardArray EmptyBoard;
extern const int dxN[8];
extern const int dyN[8];
extern const int dx[8];
extern const int dy[8];

std::string trim(const std::string& str, const std::string& ws = " \t");
std::string reduce(const std::string& str, const std::string& fill = " ", const std::string& whitespace = " \t");

SquareType ToSquare(int rank, int file);
SquareType ToSquare(int id);
Color Switch(Color c);
bool Legal(int a);
bool Legal(int a, int b);

class Exception : public std::exception
{
public:
    Exception(const std::string& m) : m(m) {}
    virtual const char* what() const throw()
    {
        return m.c_str();
    }
private:
    std::string m;
};

#endif //BASE_H
