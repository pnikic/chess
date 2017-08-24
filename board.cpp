#include "board.h"
#include <cctype>

Board::Board()
{
    fen = StartingBoardFEN;
    turn = WHITE;
    castlingRights = 15; // 0 0 0 0 1 1 1 1
    enPassant = NS;
    fullMoveNumber = 0;
    halfMoveClock = 0;
}

Board::Board(const std::string& longFen)
{
    std::copy(EmptyBoard, EmptyBoard + 8, board);
    SetBoardFen(longFen);
}

void Board::SetBoardFen(const std::string& f)
{
    size_t ws[5];
    for (int i = 0; i < 5; ++i)
    {
        ws[i] = f.find(' ', (i ? ws[i - 1] + 1 : 0));
        assert (ws[i] != std::string::npos);
    }
    
    fen = f.substr(0, ws[0]);
    assert(std::count(f.begin(), f.end(), '/') == 7);
    size_t it = 0;
    for (int i = 0; i < 8; ++i)
    {
        size_t stop = f.find_first_of("/ ", it + 1);
        int to8 = 0;
        while (it < stop)
        {
            size_t num = RankNames.find(f[it]);
            size_t let = PieceNames.find(toupper(f[it]));

            if (let != std::string::npos)
                board[7 - i][to8++] = f[it];
            else if (num != std::string::npos)
                to8 += num + 1;
            else
                assert(false);

            ++it;
        }
        
        assert(to8 == 8);
    }

    assert(f[ws[0] + 1] == 'w' || f[ws[0] + 1] == 'b');
    turn = f[ws[0] + 1] == 'w' ? WHITE : BLACK;

    size_t nr = ws[2] - ws[1] - 1;
    std::string castle = "KQkq-";
    castlingRights = 0;
    if (f[ws[1] + 1] != '-')
        for (int i = 0; i < nr; ++i)
        {
            size_t ind = castle.find(f[ws[1] + 1 + i]);
            castlingRights |= (1 << ind);
        }

    // Ispitati je li '-'
    //enPassant = Square(f.substr(ws[2] + 1));
}
