#include "board.h"
#include <algorithm>
#include <cctype>

#include <iostream>

Board::Board()
{
    PrecomputePiecesAttacks();
    ClearBoard();
}

Board::Board(const std::string &longFen)
{
    PrecomputePiecesAttacks();
    ClearBoard();
    SetFEN(longFen);
    Status();
}

Board::Board(const Board &b)
{
    PrecomputePiecesAttacks();
    pawns = b.pawns;
    knights = b.knights;
    bishops = b.bishops;
    rooks = b.rooks;
    queens = b.queens;
    kings = b.kings;
    occupiedCol[0] = b.occupiedCol[0];
    occupiedCol[1] = b.occupiedCol[1];
    occupied = b.occupied;
    enPassant = b.enPassant;
    turn = b.turn;
    castlingRights = b.castlingRights;
    halfMoveClock = b.halfMoveClock;
    fullMoveNumber = b.fullMoveNumber;
    status = b.status;
    lastMove = b.lastMove;
    lastBoard = b.lastBoard;
}

Board::~Board(){}

void Board::ClearBoard()
{
    pawns = knights = bishops = rooks = queens = kings =
        occupiedCol[0] = occupiedCol[1] = occupied = BBEmpty;
    enPassant = Square(NS);
    turn = WHITE;
    castlingRights = 0;
    halfMoveClock = 0;
    fullMoveNumber = 1;
    lastMove = Move();
    lastBoard = NULL;
}

PieceType Board::PieceTypeAt(const Square &s) const
{
    if (s.Id() == NS)
        return NONE;

    BitBoard mask = s.BB();
    if (!(occupied & mask))
        return NONE;
    else if (pawns & mask)
        return PAWN;
    else if (knights & mask)
        return KNIGHT;
    else if (bishops & mask)
        return BISHOP;
    else if (rooks & mask)
        return ROOK;
    else if (queens & mask)
        return QUEEN;
    else if (kings & mask)
        return KING;

    return NONE;
}

Piece Board::PieceAt(const Square &s) const
{
    if (s.Id() == NS)
        return Piece();

    PieceType pt = PieceTypeAt(s);
    if (pt != NONE)
    {
        BitBoard mask = s.BB();
        bool c = !(occupiedCol[WHITE] & mask);
        return Piece(pt, Color(c));
    }

    return Piece();
}

Square Board::King(Color c) const
{
    BitBoard kingSquare = kings & occupiedCol[c];
    if (countOnes(kingSquare) != 1)
        CRASH("Invalid board: No king(s)!");

    return Square(kingSquare);
}

Piece Board::RemovePieceAt(const Square &s)
{
    Piece at = PieceAt(s);
    PieceType pieceType = at.Type();
    if (pieceType == NONE)
        return Piece();

    BitBoard mask = s.BB();
    if (pieceType == PAWN)
        pawns ^= mask;
    else if (pieceType == KNIGHT)
        knights ^= mask;
    else if (pieceType == BISHOP)
        bishops ^= mask;
    else if (pieceType == ROOK)
        rooks ^= mask;
    else if (pieceType == QUEEN)
        queens ^= mask;
    else if (pieceType == KING)
        kings ^= mask;

    occupied ^= mask;
    occupiedCol[at.Side()] ^= mask;

    return at;
}

void Board::SetPieceAt(const Square &s, const Piece &p)
{
    if (s.Id() == NS || p.IsNone())
        return;

    RemovePieceAt(s);

    BitBoard mask = s.BB();
    if (p.Type() == PAWN)
        pawns |= mask;
    else if (p.Type() == KNIGHT)
        knights |= mask;
    else if (p.Type() == BISHOP)
        bishops |= mask;
    else if (p.Type() == ROOK)
        rooks |= mask;
    else if (p.Type() == QUEEN)
        queens |= mask;
    else if (p.Type() == KING)
        kings |= mask;

    occupied ^= mask;
    occupiedCol[p.Side()] ^= mask;
}

std::string Board::FEN() const
{
    std::string fen = "";
    for (int row = 7; row >= 0; --row)
    {
        int empty = 0;
        for (int col = 0; col < 8; ++col)
        {
            Piece at = PieceAt(Square(row, col));
            if (at.IsNone())
            {
                empty++;
            }
            else
            {
                if (empty)
                {
                    fen += empty + '0';
                    empty = 0;
                }

                fen += at.Symbol();
            }
        }

        // Trailing unprinted pawns
        if (empty)
            fen += empty + '0';

        if (row > 0)
            fen += "/";
    }

    fen += turn == WHITE ? " w " : " b ";
    fen += CastlingRights() + " ";
    fen += enPassant.Id() != NS ? enPassant.Name() : "-";
    fen += " " + std::to_string(halfMoveClock) + " " + std::to_string(fullMoveNumber);

    return fen;
}

void Board::SetFEN(const std::string &fenStr)
{
    // Temporary board - in order not to change the actual state in case of exceptions
    Board B;

    // TODO: Default initialization of bitbords
    std::string f = reduce(fenStr);

    // A FEN record contains six fields. The separator between fields is a space.
    size_t ws[5];
    int i = 0;
    do
        ws[i] = f.find(' ', (i ? ws[i - 1] + 1 : 0));
    while (ws[i] != std::string::npos && ++i < 5);

    if (i != 5)
        throw Exception("Invalid FEN: Incorrect number of fields - six are required!");
    if (std::count(f.begin(), f.end(), '/') != 7)
        throw Exception("Invalid FEN: Input must containt seven '/' characters!");

    // The following FEN descriptions are copied from Wikipedia
    // Source: https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
    //
    // Piece placement (from White's perspective).
    // * Each rank is described, starting with rank 8 and ending with rank 1;
    //   within each rank, the contents of each square are described from file "a" through file "h".
    // * Following the Standard Algebraic Notation (SAN), each piece is identified by a single letter
    //   taken from the standard English names (pawn = "P", knight = "N", bishop = "B", rook = "R",
    //   queen = "Q" and king = "K"). White pieces are designated using upper-case letters ("PNBRQK")
    //   while black pieces use lowercase ("pnbrqk").
    // * Empty squares are noted using digits 1 through 8 (the number of empty squares)
    //   and "/" separates ranks.

    size_t it = 0;
    for (int row = 7; row >= 0; --row)
    {
        size_t stop = f.find_first_of("/ ", it + 1);
        int col = 0;
        while (it < stop)
        {
            size_t num = RankNames.find(f[it]);
            size_t let = PieceNames.find(toupper(f[it]));

            if (let != std::string::npos)
                B.SetPieceAt(Square(row, col++), Piece(f[it]));
            else if (num != std::string::npos)
                col += num + 1;
            else
                throw Exception("Invalid FEN: Irregular letter or number in the piece placement!");

            it++;
        }

        if (col != 8)
            throw Exception("Invalid FEN: Not enoguh pieces on a rank!");
        it++;
    }

    // Active color. "w" means White moves next, "b" means Black moves next.

    if (f[ws[0] + 1] != 'w' && f[ws[0] + 1] != 'b')
        throw Exception("Invalid FEN: Side to move must be 'w' or 'b'!");
    B.turn = f[ws[0] + 1] == 'w' ? WHITE : BLACK;

    // Castling availability.
    // If neither side can castle, this is "-". Otherwise, this has one or more letters:
    // * "K" (White can castle kingside),
    // * "Q" (White can castle queenside),
    // * "k" (Black can castle kingside), and/or
    // * "q" (Black can castle queenside).
    // A move that temporarily prevents castling does not negate this notation.

    size_t castlingCnt = ws[2] - ws[1] - 1;
    std::string castle = "KQkq";
    if (f[ws[1] + 1] != '-')
    {
        for (int i = 0; i < castlingCnt; ++i)
        {
            size_t ind = castle.find(f[ws[1] + 1 + i]);
            if (ind == std::string::npos)
                throw Exception("Invalid FEN: Castling supports only 'K', 'Q', 'k', 'q' or '-'!");

            B.castlingRights |= (1 << ind);
        }
    }

    // En passant target square in algebraic notation.
    // If there's no en passant target square, this is "-".
    // If a pawn has just made a two-square move, this is the position "behind" the pawn. This is
    // recorded regardless of whether there is a pawn in position to make an en passant capture.

    if (f[ws[2] + 1] != '-')
        B.enPassant = Square(f.substr(ws[2] + 1, 2));
    else
        B.enPassant = Square(NS);

    // Halfmove clock: This is the number of halfmoves since the last capture or pawn advance.
    // The reason for this field is that the value is used in the fifty-move rule.

    std::string num = f.substr(ws[3] + 1, ws[4] - ws[3] - 1);
    if (!all_of(num.begin(), num.end(), ::isdigit))
        throw Exception("Invalid FEN: Half-move clock must be a number!");
    B.halfMoveClock = stoi(num);

    // Fullmove number: The number of the full move. It starts at 1, and is incremented after Black's move.

    num = f.substr(ws[4] + 1, f.size() - ws[4] - 1);
    if (!all_of(num.begin(), num.end(), ::isdigit))
        throw Exception("Invalid FEN: Full move number must be a number!");
    B.fullMoveNumber = stoi(num);

    // FEN parsing was successful
    *this = B;
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
    int castlingBit = (c == WHITE ? 0 : 2) + (side ? 0 : 1);

    // We assume that the neither the king nor the choosen rook has previously moved.
    //   Had they moved before, the corresponding entry in `castlingRights` would be 0.
    if ((1 << castlingBit) & castlingRights)
    {
        BitBoard kingMask = kings & occupiedCol[c];
        BitBoard rooksMask = rooks & occupiedCol[c];
        int firstRank = c == WHITE ? 0 : 7;
        int rookFile = side ? 7 : 0;

        // The king and the choosen rook are on the player's first rank starting position.
        if (!(kingMask & ToBBSquare(firstRank, 4)) ||
            !(rooksMask & ToBBSquare(firstRank, rookFile)))
            return false;

        // There are no pieces between the king and the rook.
        BitBoard inBetweenPieces = side ? ToBBSquare(firstRank, 5) | ToBBSquare(firstRank, 6) :
            ToBBSquare(firstRank, 1) | ToBBSquare(firstRank, 2) | ToBBSquare(firstRank, 3);
        if (occupied & inBetweenPieces)
            return false;

        // The king does not start/pass/land from/over/on a square that is attack by an enemy piece.
        int k0 = side ? 4 : 2;
        for (int k = k0; k < k0 + 3; ++k)
            if (IsAttackedBy(Switch(c), Square(firstRank, k)))
                return false;

        return true;
    }

    return false;
}

std::vector<Move> Board::LegalMoves() const
{
    std::vector<Move> moves;

    // King moves
    Square king = King(turn);
    // Remove the king from the occupancy to account for moving away from a checking slider
    BitBoard occupancy = occupied & ~king.BB();
    BitBoard dangerSquares = AttackedSquares(occupiedCol[Switch(turn)], occupancy);
    BitBoard kingMoves = BBKingAttacks[king.Id()] & ~dangerSquares & ~occupiedCol[turn];

    BitBoard checkers = AttackersMask(Switch(turn), king, occupied);
    int checks = countOnes(checkers);

    // King moves, if any, are possible in all cases
    while (kingMoves)
        moves.emplace_back(king, Square(ScanForward(kingMoves)));

    if (checks >= 2)
    {
        // When in double check, the only legal moves are king moves
    }
    else if (checks == 1)
    {
        // When in single check, the possibilities are
        //   1) Move the king out of check
        //      - already done

        //   2) Block the checking piece (if it is a slider)
        int seventhRank = turn == WHITE ? 6 : 1;
        // Note that a pawn move from sevent rank has 4 promotion possibilities

        Square checker = Square(checkers);
        int dir = king.Direction(checker);
        if (dir != -1)
        {
            int mvRank = king.Rank() + dy[dir], mvFile = king.File() + dx[dir];
            Square blockingCandidateSquare(mvRank, mvFile);

            // Iterate over empty squares between king and checker
            while (Legal(mvRank, mvFile) && blockingCandidateSquare != checker)
            {
                // Iterate over all pieces to find blockers for the given square
                BitBoard blockingCandidatePieces = occupiedCol[turn] & ~kings;
                while (blockingCandidatePieces)
                {
                    Square pieceSquare = Square(ScanForward(blockingCandidatePieces));
                    if (CanMove(pieceSquare, blockingCandidateSquare))
                    {
                        BitBoard from = pieceSquare.BB();
                        if ((from & pawns) && (from & BBRanks[seventhRank]))
                        {
                            for (int k = 1; k <= 4; ++k)
                                moves.emplace_back(pieceSquare, blockingCandidateSquare,
                                                   PieceTypes[k]);
                        }
                        else
                        {
                            moves.emplace_back(pieceSquare, blockingCandidateSquare);
                        }
                    }
                }

                mvRank += dy[dir];
                mvFile += dx[dir];
                blockingCandidateSquare = Square(mvRank, mvFile);
            }
        }

        //   3) Capture the checking piece
        BitBoard captureCandidatePieces = occupiedCol[turn] & ~kings;
        while (captureCandidatePieces)
        {
            Square pieceSquare = Square(ScanForward(captureCandidatePieces));
            if (CanMove(pieceSquare, checker))
            {
                BitBoard from = pieceSquare.BB();
                if ((from & pawns) && (from & BBRanks[seventhRank]))
                {
                    for (int k = 1; k <= 4; ++k)
                        moves.emplace_back(pieceSquare, checker, PieceTypes[k]);
                }
                else
                {
                    moves.emplace_back(pieceSquare, checker);
                }
            }
        }

        // Check for en passant capture of the pawn checker (not covered above as the en passant
        //   capture square is considered to be the one behind the checker)

        // behindChecker empty, and checker must be in front of e.p. square
        if (PieceTypeAt(checker) == PAWN && checker.Rank() == (turn == WHITE ? 4 : 3))
        {
            Square behindChecker = Square(checker.Rank() + (turn == WHITE ? 1 : -1), checker.File());
            int left = checker.File() - 1, right = checker.File() + 1;
            if (Legal(left))
            {
                Square leftSq = Square(checker.Rank(), left);
                Move leftM = Move(leftSq, behindChecker);
                if (IsEnPassant(leftM) && !IsPinned(leftSq, behindChecker))
                    moves.push_back(leftM);
            }

            if (Legal(right))
            {
                Square rightSq = Square(checker.Rank(), right);
                Move rightM = Move(rightSq, behindChecker);
                if (IsEnPassant(rightM) && !IsPinned(rightSq, behindChecker))
                    moves.push_back(rightM);
                // BUGFIX
                // if (PieceAt(rightSq) == Piece(PAWN, turn) && CanMove(rightSq, behindChecker))
                //     moves.emplace_back(rightSq, behindChecker);
            }
        }
    }
    else
    {
        // Not in check

        // Piece moves (except king)
        BitBoard pieces = occupiedCol[turn] & ~pawns & ~kings;
        while (pieces)
        {
            BitBoard pieceBB = ScanForward(pieces);
            Square s = Square(pieceBB);
            BitBoard targets = AttackedSquares(pieceBB, occupied);
            while (targets)
            {
                Square t = Square(ScanForward(targets));
                Piece at = PieceAt(t);
                bool isFriendlyPiece = !at.IsNone() && at.Side() == turn;
                if (!isFriendlyPiece && !IsPinned(s, t))
                    moves.emplace_back(s, t);
            }
        }

        // Pawn moves
        BitBoard turnPawns = occupiedCol[turn] & pawns;
        int promotionRank = turn == WHITE ? 7 : 0;
        while (turnPawns)
        {
            BitBoard pawnBB = ScanForward(turnPawns),
                oneFwdBB = turn == WHITE ? ShiftUp(pawnBB) : ShiftDown(pawnBB),
                twoFwdBB = turn == WHITE ? ShiftUp(oneFwdBB) : ShiftDown(oneFwdBB),
                leftFwdBB = ShiftLeft(oneFwdBB),
                rightFwdBB = ShiftRight(oneFwdBB);
            Square pawn(pawnBB), oneFwd(oneFwdBB), twoFwd(twoFwdBB),
                leftFwd(leftFwdBB), rightFwd(rightFwdBB);
            // Two squares forward
            if (CanMove(pawn, twoFwd))
                moves.emplace_back(pawn, twoFwd);
            // One square forward
            if (CanMove(pawn, oneFwd))
            {
                if (oneFwd.Rank() != promotionRank)
                    moves.emplace_back(pawn, oneFwd);
                else
                    for (int k = 1; k <= 4; ++k)
                        moves.emplace_back(pawn, oneFwd, PieceTypes[k]);
            }
            // Captures (includes en passant)
            if (CanMove(pawn, leftFwd))
            {
                if (leftFwd.Rank() != promotionRank)
                    moves.emplace_back(pawn, leftFwd);
                else
                    for (int k = 1; k <= 4; ++k)
                        moves.emplace_back(pawn, leftFwd, PieceTypes[k]);
            }

            if (CanMove(pawn, rightFwd))
            {
                if (rightFwd.Rank() != promotionRank)
                    moves.emplace_back(pawn, rightFwd);
                else
                    for (int k = 1; k <= 4; ++k)
                        moves.emplace_back(pawn, rightFwd, PieceTypes[k]);
            }
        }

        // Castling
        int firstRank = turn == WHITE ? 0 : 7;
        if (CanCastle(turn, true))
            moves.emplace_back(Square(firstRank, 4), Square(firstRank, 6));
        if (CanCastle(turn, false))
            moves.emplace_back(Square(firstRank, 4), Square(firstRank, 2));
    }

    return moves;
}

bool Board::IsPinned(const Square &s, const Square &t) const
{
    Piece fromPiece = PieceAt(s);
    Color c = fromPiece.Side();
    Square king = King(c);

    // Check if the piece or pawn is on a half-ray with the king in the center
    int dir = king.Direction(s);
    if (dir == -1)
        return false;

    // Iterate from the king in the direction of the given square
    int mvRank = king.Rank() + dy[dir], mvFile = king.File() + dx[dir];
    std::vector<Piece> pathPieces;
    std::vector<Square> pathSquares;

    while (Legal(mvRank, mvFile))
    {
        Piece tmp = PieceAt(Square(mvRank, mvFile));
        if (!tmp.IsNone())
        {
            pathPieces.push_back(tmp);
            pathSquares.emplace_back(mvRank, mvFile);
        }

        mvRank += dy[dir];
        mvFile += dx[dir];
    }

    // In case of an en passant capture, two pieces are removed from the board
    // Note: this case could be moved to CanMovePseudoLegal(s, t) in the en passant capture code
    bool epCase = 0;
    if (IsEnPassant(Move(s, t)) && pathPieces.size() >= 2)
    {
        // Locate the potential en passant pawns
        int toMovePawnInd = -1, oppPawnInd = -1;

        for (size_t i = 0; i < pathPieces.size(); ++i)
        {
            if (pathPieces[i] == Piece(PAWN, c) && pathSquares[i] == s)
                toMovePawnInd = i;
            else if (pathPieces[i] == Piece(PAWN, Switch(c)) &&
                     pathSquares[i] == Square(t.Rank() + (turn == WHITE ? -1 : 1), t.File()))
                oppPawnInd = i;
        }

        epCase = (toMovePawnInd == 0 && oppPawnInd == 1) ||
            (toMovePawnInd == 1 && oppPawnInd == 0);
    }

    // Is there any piece in between the king and moving piece?
    if (!epCase && (pathPieces.empty() || pathSquares[0] != s))
        return false;

    // Is the pinner a piece of opposite color?
    if (pathPieces.size() < (2 + epCase) || pathPieces[1 + epCase].Side() == c)
        return false;

    // It is legal for a piece or pawn to move forward or backward on the same half-ray
    if (dir == king.Direction(t))
        return false;

    // Possible pinners
    Piece piece = Piece(dir % 2 ? BISHOP : ROOK, Switch(c));
    Piece queen = Piece(QUEEN, Switch(c));

    return (pathPieces[1 + epCase] == queen || pathPieces[1 + epCase] == piece);
}

BitBoard Board::AttackedSquares(const BitBoard &attackers, const BitBoard &occupied) const
{
    BitBoard attackedSquares = BBEmpty;

    // Iterate through attackers and add the squares that they attack
    BitBoard tmp = attackers;
    while (tmp)
    {
        BitBoard attacker = ScanForward(tmp);
        SquareType sq = Square(attacker).Id();
        if (pawns & occupiedCol[0] & attacker)
        {
            attackedSquares |= BBPawnAttacks[0][sq];
        }
        else if (pawns & occupiedCol[1] & attacker)
        {
            attackedSquares |= BBPawnAttacks[1][sq];
        }
        else if (knights & attacker)
        {
            attackedSquares |= BBKnightAttacks[sq];
        }
        else if (kings & attacker)
        {
            attackedSquares |= BBKingAttacks[sq];
        }
        else
        {
            BitBoard filePiecesMask = BBFileAttacks[sq][BBAll] & occupied,
                rankPiecesMask = BBRankAttacks[sq][BBAll] & occupied,
                diagPiecesMask = BBDiagAttacks[sq][BBAll] & occupied;

            if ((rooks | queens) & attacker)
            {
                attackedSquares |= BBFileAttacks[sq][filePiecesMask];
                attackedSquares |= BBRankAttacks[sq][rankPiecesMask];
            }
            if ((bishops | queens) & attacker)
            {
                attackedSquares |= BBDiagAttacks[sq][diagPiecesMask];
            }
        }
    }

    return attackedSquares;
}

BitBoard Board::AttackersMask(const Color &c, const Square &s, const BitBoard &occupied) const
{
    const SquareType sq = s.Id();

    if (sq == NS)
        return BBEmpty;

    BitBoard filePiecesMask = BBFileAttacks[sq][BBAll] & occupied,
        rankPiecesMask = BBRankAttacks[sq][BBAll] & occupied,
        diagPiecesMask = BBDiagAttacks[sq][BBAll] & occupied;

    BitBoard attackers =
        (pawns & BBPawnAttacks[Switch(c)][sq]) |
        (knights & BBKnightAttacks[sq]) |
        (kings & BBKingAttacks[sq]) |
        ((rooks | queens) & BBFileAttacks[sq][filePiecesMask]) |
        ((rooks | queens) & BBRankAttacks[sq][rankPiecesMask]) |
        ((bishops | queens) & BBDiagAttacks[sq][diagPiecesMask]);

    // Only attackers of the given color
    return attackers & occupiedCol[c];
}

int Board::IsAttackedBy(const Color &c, const Square &s) const
{
    const SquareType sq = s.Id();

    if (sq == NS)
        return false;

    BitBoard attackers = AttackersMask(c, s, occupied);
    return countOnes(attackers);
}

int Board::IsCheck() const
{
    return IsAttackedBy(Switch(turn), King(turn));
}

bool Board::IsCheckmate() const
{
    if (!IsCheck())
        return false;

    return LegalMoves().size() == 0;
}

bool Board::IsAttacking(const Square &s, const Square &t) const
{
    Piece fromPiece = PieceAt(s);

    if (s.Id() == NS || t.Id() == NS || fromPiece.IsNone())
        return false;

    if (fromPiece.Type() == PAWN)
    {
        return BBPawnAttacks[fromPiece.Side()][s.Id()] & t.BB();
    }
    if (fromPiece.Type() == KNIGHT)
    {
        return BBKnightAttacks[s.Id()] & t.BB();
    }
    if (fromPiece.Type() == KING)
    {
        return BBKingAttacks[s.Id()] & t.BB();
    }
    if (fromPiece.Type() == BISHOP)
    {
        BitBoard diagPiecesMask = BBDiagAttacks[s.Id()][BBAll] & occupied;

        return BBDiagAttacks[s.Id()][diagPiecesMask] & t.BB();
    }
    if (fromPiece.Type() == ROOK)
    {
        BitBoard filePiecesMask = BBFileAttacks[s.Id()][BBAll] & occupied,
            rankPiecesMask = BBRankAttacks[s.Id()][BBAll] & occupied;

        return (BBFileAttacks[s.Id()][filePiecesMask] | BBRankAttacks[s.Id()][rankPiecesMask])
            & t.BB();
    }
    if (fromPiece.Type() == QUEEN)
    {
        BitBoard filePiecesMask = BBFileAttacks[s.Id()][BBAll] & occupied,
            rankPiecesMask = BBRankAttacks[s.Id()][BBAll] & occupied,
            diagPiecesMask = BBDiagAttacks[s.Id()][BBAll] & occupied;

        return (BBFileAttacks[s.Id()][filePiecesMask] | BBRankAttacks[s.Id()][rankPiecesMask]
                | BBDiagAttacks[s.Id()][diagPiecesMask]) & t.BB();
    }

    return false;
}

bool Board::CanMovePseudoLegal(const Square &s, const Square &t) const
{
    Piece fromPiece = PieceAt(s), toPiece = PieceAt(t);

    // Source square must not be empty and destination square must have a piece of different color
    if (s.Id() == NS || t.Id() == NS || fromPiece.IsNone() ||
        (!toPiece.IsNone() && toPiece.Side() == fromPiece.Side()))
        return false;

    bool ret = false;

    if (fromPiece.Type() == PAWN)
    {
        // Pawns have special movement rules
        int rankDist = s.Rank() - t.Rank(),
            fileDist = abs(s.File() - t.File());

        // Pawns can move forwards (but not to a square with another piece)
        if (toPiece.IsNone())
        {
            // In case of the pawn move for two squares, there cannot be a piece in front of him
            int pathSquareRank = fromPiece.Side() == WHITE ? 2 : 5;
            bool isPathSquareClear = !(occupied & ToBBSquare(pathSquareRank, s.File()));

            if (fromPiece.Side() == WHITE)
                ret |= (rankDist == -1 || (rankDist == -2 && s.Rank() == 1 && isPathSquareClear)) &&
                    fileDist == 0;
            else
                ret |= (rankDist == 1 || (rankDist == 2 && s.Rank() == 6 && isPathSquareClear)) &&
                    fileDist == 0;

            // Pawns can also capture en passant
            ret |= IsEnPassant(Move(s, t));
        }
        // Pawns can capture a piece diagonally
        else
        {
            ret |= IsAttacking(s, t);
        }
    }
    else if (fromPiece.Type() == KING)
    {
        ret = IsAttacking(s, t);

        if (ret)
        {
            // Kings cannot move away from a checking slider
            BitBoard occupancy = occupied & ~s.BB();
            // This also implicitly prevents the king from moving close to the other king
            BitBoard attackers = AttackersMask(Switch(fromPiece.Side()), t, occupancy);
            ret &= attackers == BBEmpty;
        }
    }
    else
    {
        // Non-pawns move the same as they attack
        ret = IsAttacking(s, t);
    }

    return ret;
}

bool Board::CanMove(const Square &s, const Square &t) const
{
    return CanMovePseudoLegal(s, t) && !IsPinned(s, t);
}

bool Board::IsEnPassant(const Move &m) const
{
    if (enPassant.Id() == NS)
        return false;

    // It is assumed that there is a pawn of the opppsite color of `turn`
    //   in front of the `enPassant` square

    // Only pawns that advanced for 3 squares can capture en passant
    int validPawnRank = turn == WHITE ? 4 : 3;
    Square f = m.From(), t = m.To();
    return t == enPassant && abs(f.File() - t.File()) == 1 && f.Rank() == validPawnRank &&
        (f.BB() & pawns & occupiedCol[turn]);
}

bool Board::IsCapture(const Move &m) const
{
    Square f = m.From(), t = m.To();
    return (t.BB() & occupiedCol[Switch(turn)] &&
            f.BB() & occupiedCol[turn]) || IsEnPassant(m);
}

bool Board::IsZeroing(const Move &m) const
{
    Piece at = PieceAt(m.From());
    return IsCapture(m) || at == Piece(PAWN, turn);
}

bool Board::IsCastling(const Move &m) const
{
    // Note that a castling move is pseudolegal iff it is completely legal.
    Square f = m.From(), t = m.To();
    return PieceAt(f) == Piece(KING, turn) && f.Distance(t) > 1;
}

bool Board::IsKSCastling(const Move &m) const
{
    return IsCastling(m) && m.To().File() == 6;
}

bool Board::IsQSCastling(const Move &m) const
{
    return IsCastling(m) && m.To().File() == 2;
}

bool Board::IsPawnDoublePush(const Move &m) const
{
    int initialRank = turn == WHITE ? 1 : 6, targetRank = turn == WHITE ? 3 : 4;
    Square f = m.From(), t = m.To();
    return PieceAt(f) == Piece(PAWN, turn) && f.Rank() == initialRank && t.Rank() == targetRank &&
        f.File() == t.File();
}

void Board::Status()
{
    status = VALID;

    if (occupied == BBEmpty)
        status |= EMPTY;

    if (!(kings & occupiedCol[WHITE]))
        status |= NO_WHITE_KING;

    if (!(kings & occupiedCol[BLACK]))
        status |= NO_BLACK_KING;

    if (countOnes(kings) > 2)
        status |= TOO_MANY_KINGS;

    if (countOnes(pawns & occupiedCol[WHITE]) > 8)
        status |= TOO_MANY_WHITE_PAWNS;

    if (countOnes(pawns & occupiedCol[BLACK]) > 8)
        status |= TOO_MANY_BLACK_PAWNS;

    if (countOnes(occupiedCol[WHITE]) > 16)
        status |= TOO_MANY_WHITE_PIECES;

    if (countOnes(occupiedCol[BLACK]) > 16)
        status |= TOO_MANY_BLACK_PIECES;

    // Pawns cannot be on the backrank
    if (pawns & BBBackRanks)
        status |= PAWNS_ON_BACK_RANK;

    if (enPassant.Id() != NS)
    {
        // The en passant square must on the third or sixth rank
        if (enPassant.Rank() != (turn == WHITE ? 5 : 2))
            status |= INVALID_EP_SQUARE;

        // The last move must have been a double pawn push, so there must
        // be a corresponding pawn on the fourth or fifth rank
        if (PieceAt(Square(turn == WHITE ? 4 : 3, enPassant.File())) == Piece(PAWN, Switch(turn)))
            status |= INVALID_EP_SQUARE;

        // The en passant and the second rank square must be empty
        if ((enPassant.BB() & occupied) ||
            (Square(turn == WHITE ? 6 : 1, enPassant.File()).BB() & occupied))
            status |= INVALID_EP_SQUARE;
    }

    if (castlingRights & ~15)
        status |= INVALID_CASTLING_RIGHTS;

    // Side to move cannot be giving check
    if (IsAttackedBy(turn, King(Switch(turn))))
        status |= OPPOSITE_CHECK;

    if (IsCheck() > 2)
        status |= TOO_MANY_CHECKERS;

    Square wK = King(WHITE), bK = King(BLACK);
    // There can be no adjecent kings
    if (wK.Distance(bK) < 2)
        status |= ADJACENT_KINGS;
}

bool Board::IsValid()
{
    Status();
    return status == VALID;
}

Color Board::ToMove() const
{
    return turn;
}

Board* Board::MakeMove(const Move& m) const
{
    Board* next = new Board(*this);
    next->lastMove = m;
    next->lastBoard = const_cast<Board*>(this);

    // Clear old en passant square
    next->enPassant = Square();

    // Remove the moving piece from the board and set it (or the promoted piece) on the new square
    Square f = m.From(), t = m.To();
    Piece p = next->RemovePieceAt(f);

    if (m.Promotion() != NONE)
    {
         p = Piece(m.Promotion(), turn);
    }

    next->SetPieceAt(t, p);

    if (IsEnPassant(m))
    {
        // Also remove the pawn capture en passant
        int epRank = turn == WHITE ? 4 : 3;
        next->RemovePieceAt(Square(epRank, enPassant.File()));
    }
    else if (IsCastling(m))
    {
        // Also move the castling rook
        int baseRank = turn == WHITE ? 0 : 7;
        int rookFromFile = m.To().File() == 6 ? 7 : 0;
        int rookToFile = m.To().File() == 6 ? 5 : 3;

        Piece at = next->RemovePieceAt(Square(baseRank, rookFromFile));
        next->SetPieceAt(Square(baseRank, rookToFile), at);

        // Castling rights bitmask     - 0 0 0 0 q k Q K
        // Remaining options for White - 0 0 0 0 0 0 1 1 =  3
        // Remaining options for Black - 0 0 0 0 1 1 0 0 = 12
        int turnMask = turn == WHITE ? 12 : 3;
        next->castlingRights &= turnMask;
    }
    else if (IsPawnDoublePush(m))
    {
        int epRank = turn == WHITE ? 2 : 5;
        next->enPassant = Square(epRank, m.From().File());
    }

    // Check losing of castling rights
    int turnShift = turn == WHITE ? 0 : 2;
    int startingRank = turn == WHITE ? 0 : 7;
    Piece atKings = next->PieceAt(Square(startingRank, 4)),
        k = Piece(KING, turn), r = Piece(ROOK, turn);

    // If KS castling rights were available, check if the king or KS rook moved
    if ((next->castlingRights & (1 << turnShift)) &&
        (atKings != k ||  next->PieceAt(Square(startingRank, 7)) != r))
    {
        next->castlingRights ^= 1 << turnShift;
    }

    // If QS castling rights were available, check if the king or QS rook moved
    if ((next->castlingRights & (1 << (1 + turnShift))) &&
        (atKings != k|| next->PieceAt(Square(startingRank, 0)) != r))
    {
        next->castlingRights ^= 1 << (1 + turnShift);
    }

    next->halfMoveClock += 1;
    if (IsZeroing(m))
        next->halfMoveClock = 0;

    next->fullMoveNumber += turn == BLACK;
    next->turn = Switch(turn);
    // TODO: status?

    return next;
}

Board* Board::UnmakeMove() const
{
    Board* ret = lastBoard;
    // Destructing the current board, as it will not be accessible in the future
    delete this;

    return ret;
}

bool Board::operator==(const Board &B) const
{
    return (pawns == B.pawns &&
            knights == B.knights &&
            bishops && B.bishops &&
            rooks == B.rooks &&
            queens == B.queens &&
            kings == B.kings &&
            occupiedCol[0] == B.occupiedCol[0] &&
            occupiedCol[1] == B.occupiedCol[1] &&
            occupied == B.occupied &&
            enPassant == B.enPassant &&
            turn == B.turn &&
            castlingRights == B.castlingRights);
}

std::ostream& operator<<(std::ostream &buf, const Board &b)
{
    for (int row = 7; row >= 0; --row)
    {
        for (int col = 0; col < 8; ++col)
        {
            buf << b.PieceAt(Square(row, col));
        }
        buf << "\n";
    }

    std::cout << "\n";

    // Testing output (will be removed)
    buf << "EP: " << b.enPassant.Name() << "\t";
    buf << "To move: " << (b.turn == WHITE ? "w\t" : "b\t");
    buf << "HMC: " << b.halfMoveClock << "\t";
    buf << "FMN: " << b.fullMoveNumber << "\t";
    buf << "Castling: " <<  b.CastlingRights() << "\n";

    return buf;
}
