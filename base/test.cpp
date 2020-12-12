#include "board.h"
#include "game.h"
#include "test.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <unordered_map>

#include <boost/process.hpp>
using namespace boost::process;

std::string test2Fen[] =
{
    "r1bq1b1r/ppp2kpp/2n5/3np3/2B5/8/PPPP1PPP/RNBQK2R w KQ - 0 7",
    "rnbqk1nr/ppp2ppp/8/4P3/1BP5/8/PP2K1PP/RN1Q1BnR w kq - 0 8",
    "rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq d3",
    "rnbqkbnr/p6p/2pp2p1/Pp2pp2/4P1P1/2P2N2/1P1P1P1P/RNBQKB1R w KQkq b6 0 7",
    "             rn1qkbn1/pP5r/3pN1p1/4ppPp/4P3/2Np4/1p1KBP1P/R1BQ3R     b   q    - 0   17      ",
    "r3k2r/8/2Q5/8/8/8/8/4K3 b kq -",
    "4k3/6q1/8/8/8/8/8/R3K2R w KQ -",
    "4k3/6q1/8/8/8/8/8/RN2K2R w KQ"};

std:: string test3Fen[] =
{
    "rnb1k2r/ppp2ppp/4p3/3p4/1bPPnB1q/2N1P3/PPQ2PPP/R3KBNR w KQkq - 0 1",
    "r1bqk2r/ppp1n1pp/2nb4/1B1p1P2/3p4/5N2/PPP2PPP/RNBQR1K1 b Qkq - 0 1",
    "6k1/pp1n2pp/3bN3/3P1p2/1PP5/4rBqr/P2Q2P1/R4RK1 w - - 0 27",
    "4k3/1p6/8/4q3/8/8/1P6/4RK2 b - - 0 1",
    "1r4Q1/Pqkb4/1pnr4/2P1P3/8/2N5/6Q1/R3K3 w Q - 0 1",
    "r4bkb/1q6/8/1n6/4R3/1K3b1r/3r4/2r5 b - - 0 1",
    "6n1/5P1k/7r/8/2B5/1P6/PBP5/1K6 w - - 0 1",
    "7B/1qR5/8/1n6/4R3/1K3b1r/8/7k b - - 0 1"
};

std::string castlingTestFEN[] =
{
    "r3k2r/ppp2ppp/8/8/8/8/PPP2PPP/R3K2R w KQkq - 0 1",
    "rn2k2r/ppp2ppp/6n1/8/4Q3/8/PPP2PPP/R3K2R b KQkq - 0 1",
    "r3k2r/ppp2p1p/5Q2/8/8/8/PPP2PPP/R3K2R b KQkq - 0 1",
    "8/8/8/8/4bbr1/4pkN1/7P/4K2R w K - 0 1"
};

void play()
{
    std::cout << "Type 'END' at any time to end the game.\n\n";

    Game G;
    std::cout << *(G.mainLineEnd->board) << "\n";
    std::vector<std::string> LM = G.LegalMoves();
    for (auto s : LM)
        std::cout << s << " ";
    std::cout << "\nYour move (UCI notation):" << std::flush;

    std::string s;
    while (std::cin >> s)
    {
        if (s == "END")
            break;

        if (std::find(LM.begin(), LM.end(), s) != LM.end())
        {
            G.MakeMove(s);
            LM = G.LegalMoves();
            std::cout << *(G.mainLineEnd->board) << "\n";
            std::vector<std::string> LM = G.LegalMoves();
            for (auto s : LM)
                std::cout << s << " ";
            std::cout << "\nYour move (UCI notation):" << std::endl;;
        }
        else
        {
            std::cout << "Invalid move. Try again!\nYour move (UCI notation):" << std::endl;
        }
    }
}

// Perft utilities
int finalDepth = 5;
unsigned long long captures[10], ep[10], castles[10], promotions[10], checks[10], checkmates[10];
Board* B;

unsigned long long Perft(int depth)
{
    unsigned long long nodes = 0;

    std::vector<Move> M = B->LegalMoves();

    // Faster, less info
    if (depth == 1)
        return M.size();

    // Slower, more info
    // if (depth == 0)
    //     return 1ull;

    for (Move m : M)
    {
        // captures[finalDepth - depth] += B->IsCapture(m);
        // ep[finalDepth - depth] += B->IsEnPassant(m);
        // castles[finalDepth - depth] += B->IsCastling(m);
        // promotions[finalDepth - depth] += m.Promotion() != NONE;

        B = B->MakeMove(m);
        // checks[finalDepth - depth] += B->IsCheck();
        // checkmates[finalDepth - depth] += B->IsCheckmate();
        nodes += Perft(depth - 1);
        B = B->UnmakeMove();
    }

    return nodes;
}

// Perft with results divided according to first move
std::map<std::string, int> Perftd(int depth)
{
    std::map<std::string, int> R;
    std::vector<Move> M = B->LegalMoves();

    for (Move m : M)
    {
        B = B->MakeMove(m);
        R[m.UCI()] = Perft(depth - 1);
        B = B->UnmakeMove();
    }

    return R;
}

// Perftd by Stockfish
std::map<std::string, int> stockfish_perft(std::string fen, int depth)
{
    // Path to stockfish executable
    std::string stockfish = "stockfish";
    opstream in;
    ipstream out;

    child c(stockfish, std_out > out, std_in < in);

    in << "position fen " <<  fen << "\n"
       << "go perft " << depth << "\n"
       << "quit" << std::endl;

    std::map<std::string, int> M;
    std::string value;
    while (getline(out, value))
    {
        size_t colon = value.find_first_of(':');
        if (colon == std::string::npos || colon > 5)
            continue;

        std::string move = value.substr(0, colon), cnt = value.substr(colon + 2);
        M[move] = std::stoi(cnt);
    }

    c.terminate();
    return M;
}

// Compare perftd results
bool compare(std::map<std::string, int> a, std::map<std::string, int> b)
{
    bool ret = true;
    if (a.size() != b.size())
        ret = false, std::cout << "Different sizes! " << a.size() << " vs " << b.size() << std::endl;

    for (auto &it : a)
    {
        const std::string &key = it.first;
        if (b.find(key) != b.end() && a[key] != b[key])
            ret = false, std::cout << key << ' ' << a[key] << "!=" << b[key] << '\n';
    }

    return ret;
}

int main()
{
    // Perft
    // B = new Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // B = new Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    // B = new Board("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    // B = new Board("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    // B = new Board("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

    // Perft with more info
    // std::cout << Perft(finalDepth) << " " << captures[finalDepth - 1] << " " << ep[finalDepth - 1]
    //           << " " << castles[finalDepth - 1] << " " << promotions[finalDepth - 1]
    //           << " " << checks[finalDepth - 1] << " " << checkmates[finalDepth - 1] << "\n";


    // int depth = 6;
    // auto M = stockfish_perft(B->FEN(), depth);
    // auto N = Perftd(depth);
    // std::cout << compare(M, N) << "\n";

    play();
}
