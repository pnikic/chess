#include "board.h"
#include <cctype>

Board::Board()
{
    SetFEN(StartingFEN);
    turn = WHITE;
    castlingRights = 15; // 0 0 0 0 1 1 1 1
    enPassant = Square(NS);
    halfMoveClock = 0;
    fullMoveNumber = 1;
}

Board::Board(const std::string& longFen)
{
    castlingRights = 0; // 0 0 0 0 0 0 0 0
    enPassant = Square(NS);
    fullMoveNumber = 1;
    halfMoveClock = 0;
    SetFEN(longFen);
}

Board::Board(const Board& b)
{
    board = b.board;
    castlingRights = b.castlingRights;
    turn = b.turn;
    enPassant = b.enPassant;
    fullMoveNumber = b.fullMoveNumber;
    halfMoveClock = b.halfMoveClock;
}

void Board::ClearBoard()
{
    board = EmptyBoard;
    castlingRights = 0;
    turn = WHITE;
    enPassant = Square(NS);
    fullMoveNumber = 1;
    halfMoveClock = 0;
}

SetSquares Board::Pieces(const Piece& p)
{
    SetSquares ss;
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            if (board[i][j] == p.Symbol())
                ss.Add(Square(i, j));
    
    return ss;
}

Piece Board::PieceAt(const Square& s)
{
    if (s.Id() != NS)
    {
        char at = board[s.Rank()][s.File()];
        return at != '.' ? Piece(at) : Piece();
    }
    else
        return Piece();
}

PieceType Board::PieceTypeAt(const Square& s)
{
    if (s.Id() != NS)
    {
        char at = board[s.Rank()][s.File()];
        if (at != '.')
        {
            size_t f = PieceNames.find(toupper(at));
            ASSERT(f != std::string::npos, "Invalid piece type!");
            return PieceTypes[f];
        }
        else
            return NONE;
    }
    else
        return NONE;
}

Square Board::King(Color c)
{
    char king = c ? 'k' : 'K';
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            if (board[i][j] == king)
                return Square(i, j);

    CRASH("Invalid board: No king(s)!");
}

Piece Board::RemovePieceAt(const Square& s)
{
    if (s.Id() != NS)
    {
        char at = board[s.Rank()][s.File()];
        board[s.Rank()][s.File()] = '.';
    
        if (at != '.')
            return Piece(at);
        else
            return Piece();
    }
    else
        return Piece();
}

void Board::SetPieceAt(const Square& s, const Piece& p)
{
    if (s.Id() != NS)
    {
        char sym = p.Symbol();
        board[s.Rank()][s.File()] = sym != ' ' ? sym : '.';
    }
}

std::string Board::FEN()
{
    std::string fen = "";
    for (int i = 7; i >= 0; --i)
    {
        if (i < 7)
            fen += '/';
            
        int pawns = 0;
        for (char c : board[i])
        {
            if (c == '.')
                ++pawns;
            else
            {
                if (pawns)
                    fen += pawns + '0';
                pawns = 0;
                fen += c;
            }
        }

        // Trailing unprinted pawns
        if (pawns)
            fen += pawns + '0';
    }
    
    fen += turn == WHITE ? " w " : " b ";

    std::string castle = "KQkq";
    for (int i = 0; i < 4; ++i)
        if (castlingRights & (1 << i))
            fen += castle[i];

    fen += fen.back() == ' ' ? "- " : " ";
    fen += enPassant.Id() != NS ? enPassant.Name() : "-";
    fen += " " + std::to_string(halfMoveClock) + " " + std::to_string(fullMoveNumber);
    return fen;
}

void Board::SetFEN(const std::string& fenStr)
{
    std::string f = trim(fenStr);
    f = reduce(f);

    board = EmptyBoard;
    size_t ws[5];
    int i = 0;
    do
        ws[i] = f.find(' ', (i ? ws[i - 1] + 1 : 0));
    while (ws[i] != std::string::npos && ++i < 5);

    ASSERT(ws[0] != std::string::npos,
           "Invalid FEN: Missing whitespace after piece placement!");
    ASSERT(std::count(f.begin(), f.end(), '/') == 7,
           "Invalid FEN: Input must containt seven '/' characters!");
    
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
                CRASH("Invalid FEN: Irregular letter or number in the piece placement!");

            it++;
        }

        ASSERT(to8 == 8, "Invalid FEN: Not enoguh pieces on a rank!");
        it++;
    }

    ASSERT(f[ws[0] + 1] == 'w' || f[ws[0] + 1] == 'b',
           "Invalid FEN: Side to move must be 'w' or 'b'!");
    turn = f[ws[0] + 1] == 'w' ? WHITE : BLACK;

    if (ws[1] != std::string::npos)
    {
        size_t nr;
        if (ws[2] != std::string::npos)
            nr = ws[2] - ws[1] - 1;
        else
            nr = f.size() - ws[1] - 1;
        
        std::string castle = "KQkq";
        castlingRights = 0;
        if (f[ws[1] + 1] != '-')
        {
            for (int i = 0; i < nr; ++i)
            {
                size_t ind = castle.find(f[ws[1] + 1 + i]);
                ASSERT(ind != std::string::npos,
                       "Invalid FEN: Castling supports only 'K', 'Q', 'k', 'q' or '-'!");
                castlingRights |= (1 << ind);
            }
        }
    }
    else
        return;

    if (ws[2] != std::string::npos)
    {
        if (f[ws[2] + 1] != '-')
            // TODO: Assert this is a regular square (not for instance z9)
            enPassant = Square(f.substr(ws[2] + 1, 2));
        else
            enPassant = Square(NS);
    }
    else
        return;

    // TODO: Assert that there is a number (Enforce a number arg to stoi)
    if (ws[3] != std::string::npos)
        halfMoveClock = stoi(f.substr(ws[3] + 1, ws[4] - ws[3] - 1));
    else
        return;

    if (ws[4] != std::string::npos)
        fullMoveNumber = stoi(f.substr(ws[4] + 1, f.size() - ws[4] - 1));
}

std::string Board::CastlingRights()
{
    std::string castle = "KQkq", res = "";
    for (int i = 0; i < 4; ++i)
        if (castlingRights & (1 << i))
            res += castle[i];

    return res.size() ? res : "-";
}

bool Board::CanCastleKS(Color c)
{
    int firstRank = c == WHITE ? 0 : 7;
    char rook = c == WHITE ? 'R' : 'r';
    int i = c == WHITE ? 0 : 2;

    // We assume that the neither the king nor the choosen rook has previously moved.
    // If they had moved before, the corresponding entry castlingRights would be 0.
    if ((1 << i) & castlingRights)
    {
        Square kingSq = King(c);
        bool flag = true;
        // The king and the choosen rook are on the player's first rank. (1)
        // There are no pieces between the king and the rook. (2)
        // The king does not start/pass/land from/over/on a square that is attack by an enemy piece (3,4,5)        
        if (kingSq.Rank() != firstRank || kingSq.File() != 4 || board[firstRank][7] != rook ||
            board[firstRank][5] != '.' || board[firstRank][6] != '.' ||
            IsAttackedBy(Switch(c), Square(firstRank, 4)) ||
            IsAttackedBy(Switch(c), Square(firstRank, 5)) ||
            IsAttackedBy(Switch(c), Square(firstRank, 6)))
            flag = false;
        return flag;
    }
    else
        return false;    
}

bool Board::CanCastleQS(Color c)
{
    int firstRank = c == WHITE ? 0 : 7;
    char rook = c == WHITE ? 'R' : 'r';
    int i = c == WHITE ? 1 : 3;

    if ((1 << i) & castlingRights)
    {
        Square kingSq = King(c);
        bool flag = true;
        if (kingSq.Rank() != firstRank || kingSq.File() != 4 || board[firstRank][0] != rook ||
            board[firstRank][1] != '.' || board[firstRank][2] != '.' || board[firstRank][3] != '.' ||
            IsAttackedBy(Switch(c), Square(firstRank, 2)) ||
            IsAttackedBy(Switch(c), Square(firstRank, 3)) ||
            IsAttackedBy(Switch(c), Square(firstRank, 4)))
            flag = false;

        return flag;
    }
    else
        return false;
}

bool Board::HasLegalEnPassant()
{
    if (enPassant.Id() != NS)
    {
        int file = enPassant.File();
        int rank = turn == WHITE ? 4 : 3;
        if (file - 1 >= 0 && board[rank][file - 1] == (turn == WHITE ? 'P' : 'p'))
            return true;
        if (file + 1 < 8 && board[rank][file + 1] == (turn == WHITE ? 'P' : 'p'))
            return true;
    }

    return false;
}

std::vector<Move> Board::PseudoLegalMoves()
{
    std::vector<Move> res;
    for (int j = 0; j < 8; ++j)
    {
        for (int i = 0; i < 8; ++i)
        {
            if (board[i][j] == (turn == WHITE ? 'P' : 'p'))
            {
                int startRank = turn == WHITE ? 1 : 6;
                int endRank = turn == WHITE ? 7 : 0;

                // One square forward
                int fwdRank = i + (turn == WHITE ? 1 : -1);
                ASSERT(fwdRank >= 0 && fwdRank < 8, "Illegal pawn position!");
                if (board[fwdRank][j] == '.')
                {
                    if (fwdRank != endRank)
                        res.emplace_back(Square(i, j), Square(fwdRank, j));
                    else
                        for (int k = 0; k < 4; ++k)
                            res.emplace_back(Square(i, j), Square(fwdRank, j), PieceTypes[k + 1]);
                }

                // Two squares forward
                int ffwdRank = startRank + (turn == WHITE ? 2 : -2);
                if (i == startRank && board[fwdRank][j] == '.' && board[ffwdRank][j] == '.')
                    res.emplace_back(Square(i, j), Square(ffwdRank, j));

                // Capturing (diagonal)
                // Left
                if (j  > 0 && board[fwdRank][j - 1] != '.' &&
                    (turn == WHITE ? islower(board[fwdRank][j - 1]) : isupper(board[fwdRank][j - 1])))
                {
                    if (fwdRank != endRank)
                        res.emplace_back(Square(i, j), Square(fwdRank, j - 1));
                    else
                        for (int k = 0; k < 4; ++k)
                            res.emplace_back(Square(i, j), Square(fwdRank, j - 1), PieceTypes[k + 1]);
                    
                }
                
                // Right
                if (j < 7 && board[fwdRank][j + 1] != '.' &&
                    (turn == WHITE ? islower(board[fwdRank][j + 1]) : isupper(board[fwdRank][j + 1])))
                {
                    if (fwdRank != endRank)
                        res.emplace_back(Square(i, j), Square(fwdRank, j + 1));
                    else
                        for (int k = 0; k < 4; ++k)
                            res.emplace_back(Square(i, j), Square(fwdRank, j + 1), PieceTypes[k + 1]);
                }
                // En passant
                int epRank = turn == WHITE ? 4 : 3;
                if (i == epRank && HasLegalEnPassant())
                {
                    int epFile = enPassant.File();
                    if (j + 1 == epFile)
                        res.emplace_back(Square(i, j), Square(fwdRank, j + 1));
                    if (j - 1 == epFile)
                        res.emplace_back(Square(i, j), Square(fwdRank, j - 1));
                    
                }
                
            }
            else if (board[i][j] == (turn == WHITE ? 'N' : 'n'))
            {
                for (int k = 0; k < 8; ++k)
                {
                    int mvRank = i + dyN[k];
                    int mvCol = j + dxN[k];
                    if (mvRank >= 0 && mvRank < 8 && mvCol >= 0 && mvCol < 8)
                    {
                        char at = board[mvRank][mvCol];
                        if(at == '.' || (turn == WHITE ? islower(at) : isupper(at)))
                            res.emplace_back(Square(i, j), Square(mvRank, mvCol));
                    }
                }
            }
            else if (board[i][j] == (turn == WHITE ? 'B' : 'b')
                     || board[i][j] == (turn == WHITE ? 'R' : 'r')
                     || board[i][j] == (turn == WHITE ? 'Q' : 'q')
                     || board[i][j] == (turn == WHITE ? 'K' : 'k'))
            {
                char c = toupper(board[i][j]);
                int k = c == 'B'? 1 : 0;
                int stride = c == 'Q' || c == 'K' ? 1 : 2;
                for (; k < 8; k += stride)
                {
                    bool possible = true;
                    int mvRank = i, mvCol = j;
                    while (possible)
                    {
                        mvRank += dy[k];
                        mvCol += dx[k];
                        if (mvRank >= 0 && mvRank < 8 && mvCol >= 0 && mvCol < 8)
                        {
                            char at = board[mvRank][mvCol];
                            if (at == '.')
                            {
                                res.emplace_back(Square(i, j), Square(mvRank, mvCol));
                                possible = c == 'K' ? false : possible;
                            }
                            else if (turn == WHITE ? islower(at) : isupper(at))
                            {
                                res.emplace_back(Square(i, j), Square(mvRank, mvCol));
                                possible = false;
                            }
                            else
                                possible = false;
                        }
                        else
                            possible = false;
                    }
                }
            }
        }

        // Check castling rights
    }


    return res;
}

bool Board::IsAttackedBy(const Color& c, const Square& s)
{
    if (s.Id() != NS)
    {
        int sqRank = s.Rank(), sqFile = s.File();
        int pawnRank = sqRank + (c == WHITE ? -1 : 1);
        // Pawn attacks
        char pawn = c == WHITE ? 'P' : 'p';
        if (pawnRank >= 0 && pawnRank < 8 &&
            ((sqFile > 0 && board[pawnRank][sqFile - 1] == pawn) ||
             (sqFile < 7 && board[pawnRank][sqFile + 1] == pawn)))
            return true;

        // Knight attacks
        char knight = c == WHITE ? 'N' : 'n';
        for (int i = 0; i < 8; ++i)
        {
            int mvRank = sqRank + dyN[i];
            int mvFile = sqFile + dxN[i];
            if (mvRank >= 0 && mvRank < 8 && mvFile >= 0 && mvFile < 8 && board[mvRank][mvFile] == knight)
                return true;
        }

        // Bishop, rook and queen attacks
        char bishop = c == WHITE ? 'B' : 'b';
        char rook = c == WHITE ? 'R' : 'r';
        char queen = c == WHITE ? 'Q' : 'q';
        for (int i = 0; i < 8; ++i)
        {
            bool possible = true;
            int mvRank = sqRank, mvFile = sqFile;
            while (possible)
            {
                mvRank += dy[i];
                mvFile += dx[i];
                if (mvRank >= 0 && mvRank < 8 && mvFile >= 0 && mvFile < 8)
                {
                    if (i % 2 && (board[mvRank][mvFile] == bishop || board[mvRank][mvFile] == queen))
                        return true;
                    else if (!(i % 2) && (board[mvRank][mvFile] == rook || board[mvRank][mvFile] == queen))
                        return true;

                    if (board[mvRank][mvFile] != '.')
                        possible = false;
                }
                else
                    possible = false;
            }
        }

        // King attacks
        char king = c == WHITE ? 'K' : 'k';
        for (int i = 0; i < 8; ++i)
        {
            int mvRank = sqRank + dy[i];
            int mvFile = sqFile + dx[i];
            if (mvRank >= 0 && mvRank < 8 && mvFile >= 0 && mvFile < 8 && board[mvRank][mvFile] == king)
                return true;
        }

        return false;
    }
    else
        return false;
}

bool Board::IsCheck()
{
    return IsAttackedBy(Switch(turn), King(turn));
}

Color Board::ToMove()
{
    return turn;
}

std::ostream& operator<<(std::ostream& buf, const Board& b)
{
    for (int i = 7; i >= 0; --i)
        buf << b.board[i] << '\n';

    // Testing output (will be removed)
    buf << "EP: " << b.enPassant.Name() << "\t";
    buf << "To move: " << (b.turn == WHITE ? "w\t" : "b\t");
    buf << "HMC: " << b.halfMoveClock << "\t";
    buf << "FMN: " << b.fullMoveNumber;

    return buf;        
}
