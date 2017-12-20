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
    status = Valid;
}

Board::Board(const std::string& longFen)
{
    castlingRights = 0; // 0 0 0 0 0 0 0 0
    enPassant = Square(NS);
    fullMoveNumber = 1;
    halfMoveClock = 0;
    SetFEN(longFen);
    status = Valid;
    Status();
}

Board::Board(const Board& b)
{
    board = b.board;
    castlingRights = b.castlingRights;
    turn = b.turn;
    enPassant = b.enPassant;
    fullMoveNumber = b.fullMoveNumber;
    halfMoveClock = b.halfMoveClock;
    status = b.status;
}

void Board::ClearBoard()
{
    board = EmptyBoard;
    castlingRights = 0;
    turn = WHITE;
    enPassant = Square(NS);
    fullMoveNumber = 1;
    halfMoveClock = 0;
    Status();
}

SetSquares Board::Pieces(const Piece& p) const
{
    SetSquares ss;
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            if (board[i][j] == p.Symbol())
                ss.Add(Square(i, j));
    
    return ss;
}

Piece Board::PieceAt(const Square& s) const
{
    if (s.Id() == NS)
        return Piece();

    char at = board[s.Rank()][s.File()];
    return at != '.' ? Piece(at) : Piece();
}

PieceType Board::PieceTypeAt(const Square& s) const
{
    if (s.Id() == NS)
        return NONE;
    
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

Square Board::King(Color c) const
{
    char king = c ? 'k' : 'K';
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            if (board[i][j] == king)
                return Square(i, j);

    CRASH("Invalid board: No king(s)!");
    return Square();
}

Piece Board::RemovePieceAt(const Square& s)
{
    if (s.Id() == NS)
        return Piece();

    char at = board[s.Rank()][s.File()];
    board[s.Rank()][s.File()] = '.';
    
    if (at != '.')
        return Piece(at);
    else
        return Piece();
}

void Board::SetPieceAt(const Square& s, const Piece& p)
{
    if (s.Id() == NS)
        return;
    
    char sym = p.Symbol();
    board[s.Rank()][s.File()] = sym != ' ' ? sym : '.';    
}

std::string Board::FEN() const
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
    // In case fenStr represents an invalid FEN, we will not update *this.
    BoardArray boardB;
    Square enPassantB;
    Color turnB;
    uint8_t castlingRightsB;
    int halfMoveClockB;
    std::string f = trim(fenStr);
    f = reduce(f);

    boardB = EmptyBoard;
    size_t ws[5];
    int i = 0;
    do
        ws[i] = f.find(' ', (i ? ws[i - 1] + 1 : 0));
    while (ws[i] != std::string::npos && ++i < 5);

    if (ws[0] == std::string::npos)
        throw Exception("Invalid FEN: Missing whitespace after piece placement!");
    if (std::count(f.begin(), f.end(), '/') != 7)
        throw Exception("Invalid FEN: Input must containt seven '/' characters!");
    
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
                boardB[7 - i][to8++] = f[it];
            else if (num != std::string::npos)
                to8 += num + 1;
            else
                throw Exception("Invalid FEN: Irregular letter or number in the piece placement!");

            it++;
        }

        if (to8 != 8)
            throw Exception("Invalid FEN: Not enoguh pieces on a rank!");
        it++;
    }

    if (f[ws[0] + 1] != 'w' && f[ws[0] + 1] != 'b')
        throw Exception("Invalid FEN: Side to move must be 'w' or 'b'!");
    turnB = f[ws[0] + 1] == 'w' ? WHITE : BLACK;

    if (ws[1] != std::string::npos)
    {
        size_t nr;
        if (ws[2] != std::string::npos)
            nr = ws[2] - ws[1] - 1;
        else
            nr = f.size() - ws[1] - 1;
        
        std::string castle = "KQkq";
        castlingRightsB = 0;
        if (f[ws[1] + 1] != '-')
        {
            for (int i = 0; i < nr; ++i)
            {
                size_t ind = castle.find(f[ws[1] + 1 + i]);
                if (ind == std::string::npos)
                    throw Exception("Invalid FEN: Castling supports only 'K', 'Q', 'k', 'q' or '-'!");
                castlingRightsB |= (1 << ind);
            }
        }
    }
    else
    {
        board = boardB; turn = turnB;
        return;
    }

    if (ws[2] != std::string::npos)
    {
        if (f[ws[2] + 1] != '-')
            enPassantB = Square(f.substr(ws[2] + 1, 2));
        else
            enPassantB = Square(NS);
    }
    else
    {
        board = boardB; turn = turnB;
        castlingRights = castlingRightsB;
        return;
    }

    if (ws[3] != std::string::npos)
    {
        std::string num = f.substr(ws[3] + 1, ws[4] - ws[3] - 1);
        if (!all_of(num.begin(), num.end(), ::isdigit))
            throw Exception("Invalid FEN: Half-move clock must be a number!");
        halfMoveClockB = stoi(num);
    }
    else
    {
        board = boardB; turn = turnB;
        castlingRights = castlingRightsB;
        enPassant = enPassantB;
        return;
    }

    if (ws[4] != std::string::npos)
    {
        std::string num = f.substr(ws[4] + 1, f.size() - ws[4] - 1);
        if (!all_of(num.begin(), num.end(), ::isdigit))
            throw Exception("Invalid FEN: Full move number must be a number!");
        board = boardB; turn = turnB;
        castlingRights = castlingRightsB;
        enPassant = enPassantB;
        halfMoveClock = halfMoveClockB;
        fullMoveNumber = stoi(num);
    }
    else
    {
        board = boardB; turn = turnB;
        castlingRights = castlingRightsB;
        enPassant = enPassantB;
        halfMoveClock = halfMoveClockB;
    }

}

std::string Board::CastlingRights() const
{
    std::string castle = "KQkq", res = "";
    for (int i = 0; i < 4; ++i)
        if (castlingRights & (1 << i))
            res += castle[i];

    return res.size() ? res : "-";
}

bool Board::CanCastle(Color c, bool side) const
{
    int firstRank = c == WHITE ? 0 : 7;
    char rook = c == WHITE ? 'R' : 'r';
    int i = (c == WHITE ? 0 : 2) + (side ? 0 : 1);
    int rookFile = side ? 7 : 0;

    // We assume that the neither the king nor the choosen rook has previously moved.
    // If they had moved before, the corresponding entry castlingRights would be 0.
    if ((1 << i) & castlingRights)
    {
        Square kingSq = King(c);
        // The king and the choosen rook are on the player's first rank.
        if (kingSq.Rank() != firstRank || kingSq.File() != 4 || board[firstRank][rookFile] != rook)
            return false;
        
        // There are no pieces between the king and the rook.
        if ((side && (board[firstRank][5] != '.' || board[firstRank][6] != '.')) ||
            (!side && (board[firstRank][1] != '.' || board[firstRank][2] != '.' || board[firstRank][3] != '.')))
            return false;
                    
        // The king does not start/pass/land from/over/on a square that is attack by an enemy piece.
        int k_0 = side ? 4 : 2;
        for (int k = k_0; k < k_0 + 3; ++k)
            if (IsAttackedBy(Switch(c), Square(firstRank, k)))
                return false;
        
        return true;
    }
    else
        return false;
}

bool Board::HasLegalEnPassant() const
{
    if (enPassant.Id() == NS)
        return false;
    
    int file = enPassant.File();
    int rank = turn == WHITE ? 4 : 3;

    if (file - 1 >= 0 && board[rank][file - 1] == (turn == WHITE ? 'P' : 'p'))
        return true;
    if (file + 1 < 8 && board[rank][file + 1] == (turn == WHITE ? 'P' : 'p'))
        return true;

    return false;
}

std::vector<Move> Board::PseudoLegalMoves() const
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
                ASSERT(Legal(fwdRank), "Illegal pawn position!");
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
                    if (Legal(mvRank, mvCol))
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
                        if (Legal(mvRank, mvCol))
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
    }

    // Castling
    int firstRank = turn == WHITE ? 0 : 7;
    if (CanCastle(turn, true))
        res.emplace_back(Square(firstRank, 4), Square(firstRank, 6));
    if (CanCastle(turn, false))
        res.emplace_back(Square(firstRank, 4), Square(firstRank, 2));

    return res;
}

bool Board::IsPinned(const Color& c, const Square& s) const
{
    char at = board[s.Rank()][s.File()];
    if (at == '.' || (c == WHITE ? islower(at) : isupper(at)))
        return false;
    
    Square king = King(c);
    if (king.Id() == NS)
        return false;

    int dir = king.Direction(s);
    if (dir == -1)
        return false;

    char piece = dir % 2 ? (c == BLACK ? 'B' : 'b') : (c == BLACK ? 'R' : 'r');
    char queen = c == BLACK ? 'Q' : 'q';
    int mvRank = king.Rank() + dy[dir], mvFile = king.File() + dx[dir];

    std::string path;
    while (Legal(mvFile, mvRank))
    {
        path += board[mvRank][mvFile];
        mvFile += dx[dir];
        mvRank += dy[dir];
    }

    char pinned = path.find_first_not_of('.');
    char pinner = path.find_first_not_of('.', pinned + 1);

    if (pinner != std::string::npos && pinned != std::string::npos &&
        (path[pinner] == queen || path[pinner] == piece))
        return true;

    return false;
}

int Board::IsAttackedBy(const Color& c, const Square& s) const
{
    if (s.Id() == NS)
        return false;

    int att = 0;
    int sqRank = s.Rank(), sqFile = s.File();
    int pawnRank = sqRank + (c == WHITE ? -1 : 1);
    // Pawn attacks
    char pawn = c == WHITE ? 'P' : 'p';
    if (pawnRank >= 0 && pawnRank < 8 &&
        ((sqFile > 0 && board[pawnRank][sqFile - 1] == pawn) ||
         (sqFile < 7 && board[pawnRank][sqFile + 1] == pawn)))
        ++att;

    // Knight attacks
    char knight = c == WHITE ? 'N' : 'n';
    for (int i = 0; i < 8; ++i)
    {
        int mvRank = sqRank + dyN[i];
        int mvFile = sqFile + dxN[i];
        if (Legal(mvRank, mvFile) && board[mvRank][mvFile] == knight)
            ++att;
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
            if (Legal(mvRank, mvFile))
            {
                if (i % 2 && (board[mvRank][mvFile] == bishop || board[mvRank][mvFile] == queen))
                    ++att;
                else if (!(i % 2) && (board[mvRank][mvFile] == rook || board[mvRank][mvFile] == queen))
                    ++att;

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
        if (Legal(mvRank, mvFile) && board[mvRank][mvFile] == king)
            ++att;
    }

    return att;
}

bool Board::IsCheck() const
{
    return IsAttackedBy(Switch(turn), King(turn));
}

bool Board::IsAttacking(const Square& s, const Square& t, const Piece &p /*= Piece()*/) const
{
    char at = (p.IsNone() ? board[s.Rank()][s.File()] : p.Symbol()), atUpper = toupper(at);
    if (s.Id() == NS || t.Id() == NS || at == '.')
        return false;

    int rankDist = s.Rank() - t.Rank();
    int fileDist = abs(s.File() - t.File());
    if (at == 'P')
        return rankDist == -1 && fileDist == 1;
    if (at == 'p')
        return rankDist == 1 && fileDist == 1;
    
    rankDist = abs(rankDist);
    if (atUpper == 'N')
        return (rankDist == 2 && fileDist == 1) || (rankDist == 1 && fileDist == 2);
    
    if (atUpper == 'K')
        return s.Distance(t) == 1;

    int dir = s.Direction(t);
    if (dir == -1 || (atUpper == 'B' && !(dir % 2)) || (atUpper == 'R' && (dir % 2)))
        return false;
    
    int mvRank = s.Rank() + dy[dir], mvFile = s.File() + dx[dir];
    while (Legal(mvFile, mvRank) && board[mvRank][mvFile] == '.' && Square(mvRank, mvFile) != t)
        mvRank += dy[dir], mvFile += dx[dir];

    return Square(mvRank, mvFile) == t;
}

bool Board::CanMove(const Square& s, const Square& t, const Piece& p /*= Piece()*/) const
{
    char at = (p.IsNone() ? board[s.Rank()][s.File()] : p.Symbol());
    if (s.Id() == NS || t.Id() == NS || at == '.')
        return false;

    int rankDist = s.Rank() - t.Rank();
    int fileDist = abs(s.File() - t.File());
    if (at == 'P')
        return (rankDist == -1 || (rankDist == -2 && s.Rank() == 1)) && fileDist == 0;
    if (at == 'p')
        return (rankDist == 1 || (rankDist == 2 && s.Rank() == 6)) && fileDist == 0;

    return IsAttacking(s, t, p);
}

bool Board::IsEnPassant(const Move& m) const
{
    Square f = m.From(), t = m.To();
    return board[f.Rank()][f.File()] == (turn == WHITE ? 'P' : 'p') &&
        board[f.Rank()][t.File()] == (turn == WHITE ? 'p' : 'P') &&
        t == enPassant && abs(f.File() - t.File()) == 1;
}

bool Board::IsCapture(const Move& m) const
{
    Square f = m.From(), t = m.To();
    char fPiece = board[f.Rank()][f.File()], tPiece = board[t.Rank()][t.File()];
    return ((turn == WHITE ? isupper(fPiece) : islower(fPiece)) &&
            (turn == WHITE ? islower(tPiece) : isupper(tPiece))) ||
        IsEnPassant(m);
}

bool Board::IsZeroing(const Move& m) const
{
    Square f = m.From();
    char fPiece = board[f.Rank()][f.File()];
    return IsCapture(m) || fPiece == (turn == WHITE ? 'P' : 'p');
}

bool Board::IsCastling(const Move& m) const
{
    // Note that a castling move is pseudolegal iff it is completely legal.
    Square f = m.From(), t = m.To();
    char fPiece = board[f.Rank()][f.File()];
    return fPiece == (turn == WHITE ? 'K' : 'k') && f.Distance(t) > 1;
}

bool Board::IsKSCastling(const Move& m) const
{
    return IsCastling(m) && m.To().File() == 6;
}

bool Board::IsQSCastling(const Move &m) const
{
    return IsCastling(m) && m.To().File() == 2;
}

std::string Board::SAN(const Move& m) const
{
    std::string SAN;
    Square f = m.From(), t = m.To();
    char fPiece = board[f.Rank()][f.File()];
    
    if (toupper(fPiece) == 'P')
    {
        if (IsCapture(m))
            SAN = std::string(1, f.Name()[0]) + "x" + t.Name();
        else
            SAN = t.Name();

        if (m.Promotion() != NONE)
            SAN += "=" + std::string(1, PieceNames[m.Promotion()]);
    }
    else if (IsKSCastling(m))
        SAN = "O-O";
    else if (IsQSCastling(m))
        SAN = "O-O-O";
    else
    {
        // To resolve move ambiguities (e.g. Nge2, Bgd5, Qa2d5 etc.)
        std::string fP = std::string(1, toupper(fPiece));
        std::string x = IsCapture(m) ? "x" : "";
        std::vector<Square> A;
        for (int j = 0; j < 8; ++j)
        {
            for (int i = 0; i < 8; ++i)
            {
                Square sq = Square(i, j);
                if (board[i][j] == fPiece && sq != f && !IsPinned(turn, sq) && IsAttacking(sq, t))
                    A.push_back(sq);
            }
        }

        if (!A.size())
            SAN = fP + x + t.Name();
        else if (find_if(A.begin(), A.end(), [&](Square sq){return sq.File() == f.File();}) == A.end())
            SAN = fP + std::string(1, f.Name()[0]) + x + t.Name();
        else if (find_if(A.begin(), A.end(), [&](Square sq){return sq.Rank() == f.Rank();}) == A.end())
            SAN = fP + std::string(1, f.Name()[1]) + x + t.Name();
        else
            SAN = fP + f.Name() + x + t.Name();
    }

    Square oppKing = King(Switch(turn));
    char oKing = turn == WHITE ? 'k' : 'K';
    if (m.Promotion() != NONE)
        fPiece = turn == WHITE ? PieceNames[m.Promotion()] : tolower(PieceNames[m.Promotion()]);

    // After the move is played, the opposite side might end up in check (e.g. discovery)
    Board after(*this);
    after.SetPieceAt(t, Piece(fPiece));
    after.RemovePieceAt(f);
    int nrAttackers = after.IsAttackedBy(turn, oppKing);

    if (nrAttackers)
    {
        // Check for checkmate
        bool flag = true;
        if (nrAttackers == 1)
        {
            // Find direction and square of the attacker
            int dir; Square Att;
            for (int i = 0; i < 8; ++i)
                for (int j = 0; j < 8; ++j)
                    if (after.IsAttacking(Square(i, j), oppKing))
                        dir = oppKing.Direction(Square(i,j)), Att = Square(i, j);

            if (dir != -1)
            {
                // Interposition (only unpinned pieces or pawns can interpose)
                int mvRank = oppKing.Rank() + dy[dir], mvFile = oppKing.File() + dx[dir];
                while (flag && Legal(mvRank, mvFile) && Square(mvRank, mvFile) != Att)
                {
                    char king = turn == BLACK ? 'K' : 'k';
                    for (int i = 0; i < 8 && flag; ++i)
                    {
                        for (int j = 0; j < 8 && flag; ++j)
                        {
                            char at = board[i][j];
                            if ((turn == BLACK ? isupper(at) : islower(at)) && at != king &&
                                after.CanMove(Square(i, j), Square(mvRank, mvFile)) &&
                                !after.IsPinned(Switch(turn), Square(i, j)))
                                flag = false;
                        }
                    }

                    mvRank += dy[dir]; mvFile += dx[dir];
                }
            }

            // Can the checking piece be captured?
            for (int i = 0; i < 8 && flag; ++i)
            {
                for (int j = 0; j < 8 && flag; ++j)
                {
                    char at = board[i][j];
                    if (at != oKing && (turn == WHITE ? islower(at) : isupper(at)) &&
                        after.IsAttacking(Square(i, j), Att) && !after.IsPinned(Switch(turn), Square(i, j)))
                        flag = false;
                }
            }
        
            // The king can only capture an undefended piece.
            if (flag && after.IsAttacking(oppKing, Att) && !after.IsAttackedBy(turn, Att))
                flag = false;
        }
        
        // Can the king escape?
        for (int i = 0; i < 8 && flag; ++i)
        {
            int eRank = oppKing.Rank() + dy[i], eFile = oppKing.File() + dx[i];
            if (!Legal(eRank, eFile))
                continue;
            
            Square e(eRank, eFile);
            if (board[eRank][eFile] != '.' || e == t)
                continue;

            // How would the board then look like?
            Board tmp(after);
            tmp.SetPieceAt(e, Piece(oKing));
            tmp.RemovePieceAt(oppKing);

            if (!tmp.IsAttackedBy(turn, e))
                flag = false;
        }
        SAN += flag ? "#" : "+";
    }

    return SAN;
}

void Board::Status()
{
    if (board == EmptyBoard)
        status |= Empty;

    int nrKW = 0, nrKB = 0;
    int nrPW = 0, nrPB = 0;
    int nrW = 0, nrB = 0;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (PieceNames.find(toupper(board[i][j])) != std::string::npos)
            {
                if (isupper(board[i][j]))
                    ++nrW;
                else
                    ++nrB;

                switch (board[i][j])
                {
                case 'K':
                    ++nrKW; break;
                case 'k':
                    ++nrKB; break;
                case 'P':
                    ++nrPW; break;
                case 'p':
                    ++nrPB; break;
                }
            }
        }
    }

    if (nrKW == 0)
        status |= NoWhiteKing;
    if (nrKB == 0)
        status |= NoBlackKing;
    if (nrKW > 1 || nrKB > 1)
        status |= TooManyKings;

    // There can be no more than 16 pieces of any color.
    if (nrW > 16)
        status |= TooManyWhitePieces;
    if (nrB > 16)
        status |= TooManyBlackPieces;
    // There can be no more than 8 pawns of any color.
    if (nrPW > 8)
        status |= TooManyWhitePawns;
    if (nrPB > 8)
        status |= TooManyBlackPawns;

    // Pawns cannot be on the backrank.
    if (any_of(board[0].begin(), board[0].end(), [](char x){return toupper(x) == 'P';}) ||
        any_of(board[7].begin(), board[7].end(), [](char x){return toupper(x) == 'P';}))
        status |= PawnsOnBackRank;

    if (enPassant.Id() != NS)
    {
        // The en passant square must on the third or sixth rank.
        if (enPassant.Rank() != (turn == WHITE ? 5 : 2))
            status |= InvalidEpSquare;

        // The last move must have been a double pawn push, so there must
        // be a corresponding pawn on the fourth or fifth rank.
        if (board[turn == WHITE ? 4 : 3][enPassant.File()] != (turn == WHITE ? 'p' : 'P'))
            status |= InvalidEpSquare;

        // The en passant and the second rank square must be empty.
        if (board[enPassant.Rank()][enPassant.File()] != '.' ||
            board[turn == WHITE ? 6 : 1][enPassant.File()] != '.')
            status |= InvalidEpSquare;
    }

    // Side to move cannot be giving check.
    if (IsAttackedBy(turn, King(Switch(turn))))
        status |= OppositeCheck;

    Square wK = King(WHITE), bK = King(BLACK);
    // There can be no adjecent kings.
    if (wK.Distance(bK) < 2)
        status |= AdjacentKings;    
}

bool Board::IsValid() const
{
    return status == Valid;
}

Color Board::ToMove() const
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
    buf << "FMN: " << b.fullMoveNumber << "\t";
    buf << "Castling: " <<  b.CastlingRights() << "\n";

    return buf;        
}
