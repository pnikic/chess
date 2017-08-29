#include <iostream>
#include "base.h"
#include "square.h"
#include "setsqr.h"
#include "piece.h"
#include "move.h"
#include "board.h"
using namespace std;

int main()
{
    //First test
/*  Square e4(3, 4);
    Square d5("d5");
    Square a1(A1);
    cout << e4 << "\n" << d5.Name() << endl;
    cout << "Distance: " << e4.Distance(a1);
    
    Piece A(ROOK, BLACK);
    Piece B('Q');
    Piece C(B);
    cout << C.Symbol() << " " << A.Symbol() << endl;

    Move m(e4, d5);
    Move n("E4e5");
    Move o("a7a8N");
    cout <<  m.UCI() << " " << n.UCI() << " " << o.UCI() << endl;
    
    SetSquares sA((unsigned long long)7);
    cout << sA << endl;
    sA.Add(e4);
    sA.Add(e4);
    sA.Add(d5);
    sA.Remove(Square(B1));
    sA.Remove(Square(0, 2));
    sA.Add(Square(1, 6));
    cout << sA << endl;
    cout << sA.Count() << endl;
    cout << sA.IsEmpty(Square(B1)) << endl;
*/  

    //Second test
/*    
      std::string a = "   rnbqkbnr/pppppppp/8/8/8/5N2/PPPPPPPP/RNBQKB1R     b KQkq    -   ";
      cout << a << endl;
      a = trim(a);
      cout << a << endl;
      a = reduce(a);
      cout << a << endl;
*/

    //Third test

    std::string testFen[] = {"r1bq1b1r/ppp2kpp/2n5/3np3/2B5/8/PPPP1PPP/RNBQK2R w KQ - 0 7",
                             "rnbqk1nr/ppp2ppp/8/4P3/1BP5/8/PP2K1PP/RN1Q1BnR w kq - 0 8",
                             "rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq d3",
                             "rnbqkbnr/p6p/2pp2p1/Pp2pp2/4P1P1/2P2N2/1P1P1P1P/RNBQKB1R w KQkq b6 0 7",
                             "rn1qkbn1/pP5r/3pN1p1/4ppPp/4P3/2Np4/1p1KBP1P/R1BQ3R b q - 0 17"};
    for (string s : testFen)
    {
        Board brd(s);
        cout << brd << endl;
        cout << brd.CastlingRights() << endl;
        cout << brd.BoardFen() << endl;
        /*
        cout << "Kings: " << brd.King(WHITE).Name() << ", " << brd.King(BLACK).Name() << endl;
        cout << "PieceAt: e4 = " << brd.PieceAt(Square("e4")).Symbol() <<
            ", f8 = " << brd.PieceAt(Square("f8")).Symbol() << endl;
        cout << "PieceTypeAt: e4 = " << (int)brd.PieceTypeAt(Square("e4")) <<
            ", f8 = " << (int)brd.PieceTypeAt(Square("f8")) << endl;
        cout << "White N's: \n" << brd.Pieces(Piece('N')) << endl;

        if (s == testFen[3])
            brd.ClearBoard(), cout << "ClearBoard:\n" << brd << "\n " << brd.CastlingRights() << "\n " << brd.BoardFen() << endl;
        */
        std::vector<Move> mvs = brd.LegalMoves();
        for (Move mov : mvs)
            cout << mov.UCI() << " ";
        cout << endl << endl;
        
    }

}
