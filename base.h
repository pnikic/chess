#ifndef BASE_H
#define BASE_H

#include <iostream>
#include <cassert>
#include <cstdint>
#include <string>
#include <algorithm>

typedef bool Color;
typedef uint8_t PieceType;
typedef uint8_t SquareType;
typedef uint8_t FileType;
typedef uint8_t RankType;
//leftDiagT, rightDiagT

const PieceType PAWN = 0, KNIGHT = 1, BISHOP = 2, ROOK = 3, QUEEN = 4, KING = 5, NONE = 6;
const Color WHITE = true, BLACK = false;
const SquareType A1 = 0, A2 = 1, A3 = 2, A4 = 3, A5 = 4, A6 = 5, A7 = 6, A8 = 7, 
    B1 = 8, B2 = 9, B3 = 10, B4 = 11, B5 = 12, B6 = 13, B7 = 14, B8 = 15, 
    C1 = 16, C2 = 17, C3 = 18, C4 = 19, C5 = 20, C6= 21, C7 = 22, C8 = 23, 
    D1 = 24, D2 = 25, D3 = 26, D4 = 27, D5 = 28, D6 = 29, D7 = 30, D8 = 31, 
    E1 = 32, E2 = 33, E3 = 34, E4 = 35, E5 = 36, E6 = 37, E7 = 38, E8 = 39, 
    F1 = 40, F2 = 41, F3 = 42, F4 = 43, F5 = 44, F6 = 45, F7 = 46, F8 = 47, 
    G1 = 48, G2 = 49, G3 = 50, G4 = 51, G5 = 52, G6 = 53, G7 = 54, G8 = 55,
    H1 = 56, H2 = 57, H3 = 58, H4 = 59, H5 = 60, H6 = 61, H7 = 62, H8 = 63,  NS = 64; // NS = null square
//Squares[A1, ..., H8]
extern const std::string FileNames;
extern const std::string RankNames;
extern const std::string PieceNames;
extern const PieceType PieceTypes[];
extern const std::string EmptyBoard[];
extern const std::string StartingFEN;
extern const std::string StartingBoardFEN;

#endif //BASE_H
