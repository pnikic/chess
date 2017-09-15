#include "test.h"

Test::Test() : testFen{"r1bq1b1r/ppp2kpp/2n5/3np3/2B5/8/PPPP1PPP/RNBQK2R w KQ - 0 7",
        "rnbqk1nr/ppp2ppp/8/4P3/1BP5/8/PP2K1PP/RN1Q1BnR w kq - 0 8",
        "rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq d3",
        "rnbqkbnr/p6p/2pp2p1/Pp2pp2/4P1P1/2P2N2/1P1P1P1P/RNBQKB1R w KQkq b6 0 7",
        "             rn1qkbn1/pP5r/3pN1p1/4ppPp/4P3/2Np4/1p1KBP1P/R1BQ3R     b   q    - 0   17      ",
        "r3k2r/8/2Q5/8/8/8/8/4K3 b kq -",
        "4k3/6q1/8/8/8/8/8/R3K2R w KQ -",
        "4k3/6q1/8/8/8/8/8/RN2K2R w KQ"}
{
    e4 = Square(3, 4);
    d5 = Square("d5");
    a1 = Square(A1);
    A = Piece(ROOK, BLACK);
    B = Piece('Q');
    C = Piece(B);
    m = Move(e4, d5);
    n = Move("E4e5");
    o = Move("a7a8N");
    sA = SetSquares((unsigned long long)7);
}

void Test::test1()
{
    std::cout << "Test #1 started..." << std::endl;
    std::stringstream ss;
    ss << e4;
    ASSERT(ss.str() == "........\n........\n........\n........\n....1...\n........\n........\n........\n",
           "Square::operator<< for the e4 square");
    ASSERT(d5.Name() == "d5", "Square::Name for the d5 square");
    ASSERT(e4.Distance(a1) == 4, "Square::Name for the d5 square");

    ASSERT(A.Symbol() == 'r', "Piece::Symbol");
    ASSERT(B.Symbol() == 'Q', "Piece::Symbol");
    ASSERT(C.Symbol() == 'Q', "Piece::Symbol");

    ASSERT(m.UCI() == "e4d5", "Move::UCI");
    ASSERT(n.UCI() == "e4e5", "Move::UCI");
    ASSERT(o.UCI() == "a7a8n", "Move::UCI");
    
    ss.str(std::string());
    ss << sA;
    ASSERT(ss.str() == "........\n........\n........\n........\n........\n........\n........\n111.....\n",
           "SetSquares::operator<<");
    sA.Add(e4);
    sA.Add(e4);
    sA.Add(d5);
    sA.Remove(Square(B1));
    sA.Remove(Square(0, 2));
    sA.Add(Square(1, 6));

    ss.str(std::string());
    ss << sA;
    ASSERT(ss.str() == "........\n........\n........\n...1....\n....1...\n........\n......1.\n1.......\n",
           "SetSquares::Add, SetSquares::Remove, SetSquares::operator<<");
    ASSERT(sA.Count() == 4, "SetSquares::Count");
    ASSERT(sA.IsEmpty(Square(B1)), "SetSquares::IsEmpty");

    std::cout << "Test #1 succesfully finished.\n" << std::endl;
}

void Test::test2()
{
    std::cout << "Test #2 started...(8 subtests)" << std::endl;
    std::string sol[9][8] = {{"r1bq1b1r/ppp2kpp/2n5/3np3/2B5/8/PPPP1PPP/RNBQK2R w KQ - 0 7",
                              "rnbqk1nr/ppp2ppp/8/4P3/1BP5/8/PP2K1PP/RN1Q1BnR w kq - 0 8",
                              "rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq d3 0 1",
                              "rnbqkbnr/p6p/2pp2p1/Pp2pp2/4P1P1/2P2N2/1P1P1P1P/RNBQKB1R w KQkq b6 0 7",
                              "rn1qkbn1/pP5r/3pN1p1/4ppPp/4P3/2Np4/1p1KBP1P/R1BQ3R b q - 0 17",
                              "r3k2r/8/2Q5/8/8/8/8/4K3 b kq - 0 1",
                              "4k3/6q1/8/8/8/8/8/R3K2R w KQ - 0 1",
                              "4k3/6q1/8/8/8/8/8/RN2K2R w KQ - 0 1"},
                             {"KQ", "kq", "KQkq", "KQkq", "q", "kq", "KQ", "KQ"},
                             {"e1", "e2", "e1", "e1", "d2", "e1", "e1", "e1"},
                             {"f7", "e8", "e8", "e8", "e8", "e8", "e8", "e8"},
                             {" ", " ", " ", "P", "P", " ", " ", " "},
                             {"p", "p", "p", " ", "P", " ", " ", " "},
                             {"........\n........\n........\n........\n........\n........\n........\n.1......\n",
                              "........\n........\n........\n........\n........\n........\n........\n.1......\n",
                              "........\n........\n........\n........\n........\n........\n........\n.1....1.\n",
                              "........\n........\n........\n........\n........\n.....1..\n........\n.1......\n",
                              "........\n........\n....1...\n........\n........\n..1.....\n........\n........\n",
                              "........\n........\n........\n........\n........\n........\n........\n........\n",
                              "........\n........\n........\n........\n........\n........\n........\n........\n",
                              "........\n........\n........\n........\n........\n........\n........\n.1......\n"},
                             {"........\n........\n1.......\n.1.1...1\n......1.\n11111111\n111111.1\n.111111.\n",
                              "...1.1..\n...11...\n...1.1..\n1111....\n1..1....\n11111111\n1.111111\n.111111.\n",
                              ".111111.\n11111111\n11111111\n........\n........\n........\n........\n........\n",
                              "........\n........\n.1......\n11.11111\n1111...1\n11111111\n11111111\n.111111.\n",
                              ".11111.1\n1111111.\n1111.1.1\n1.1.1111\n...1111.\n........\n..1.1...\n1.1.....\n",
                              ".111111.\n1..111.1\n1......1\n1......1\n1......1\n1......1\n1......1\n1......1\n",
                              "1......1\n1......1\n1......1\n1......1\n1......1\n1......1\n1..111.1\n.111111.\n",
                              "1......1\n1......1\n1......1\n1......1\n1......1\n1.1....1\n1..111.1\n.1.1111.\n"},
                             {"n", "y", "n", "n", "n", "y", "n", "n"}};
    for (int i = 0; i < 8; ++i)
    {
        brd = Board(testFen[i]);
        ASSERT(brd.FEN() == sol[0][i], "Board::SetFEN, Board::FEN");
        ASSERT(brd.CastlingRights() == sol[1][i], "Board::CastlingRights");
        ASSERT(brd.King(WHITE).Name() == sol[2][i], "Board::King");
        ASSERT(brd.King(BLACK).Name() == sol[3][i], "Board::King");
        ASSERT(std::string(1, brd.PieceAt(Square("e4")).Symbol()) == sol[4][i], "Board::PieceAt");
        ASSERT(std::string(1, brd.PieceAt(Square("b7")).Symbol()) == sol[5][i], "Board::PieceAt");

        std::stringstream ss;
        ss << brd.Pieces(Piece('N'));
        ASSERT(ss.str() == sol[6][i], "Board::Pieces");
        ss.str(std::string());
        
        sB = SetSquares();
        for (const SquareType sq : Squares)
        {
            Square s(sq);
            if (brd.IsAttackedBy(brd.ToMove(), s))
                sB.Add(s);
        }        
        ss << sB;
        ASSERT(ss.str() == sol[7][i], "Board::IsAttackedBy, SetSqures::Add");
        ss.str(std::string());

        ASSERT((brd.IsCheck() ? "y" : "n") == sol[8][i], "Board::IsCheck");

        int cnt = 0;
        std::cout << brd << std::endl;
        for (auto& move : brd.PseudoLegalMoves())
        {
            std::cout << " " << move.UCI();
            if (++cnt > 4)
                cnt = 0, std::cout << std::endl;
        }
        if (cnt)
            std::cout << std::endl;
        std::cout << std::endl;

        ASSERT(brd.IsValid(), "Board::IsValid, Board::Status");
    }
    std::cout << std::endl << "Test #2 succesfully finished.\n" << std::endl;

}
    
int main()
{
    Test T;
    T.test1();
    T.test2();

    // More tests: Board::PseudoLegalMoves, Board::Pieces, Board::ClearBoard, Board::CanCastleQS / KS
}
