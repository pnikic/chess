#include "board.h"
#include "game.h"

Node::Node() :
    board(NULL),
    parent(NULL)
{}

Node::Node(Board *b) :
    board(b),
    parent(NULL)
{}

Node::Node(Board *b, Node *p) :
    board(b),
    parent(p)
{}

Node::~Node()
{
    for (Node *c : children)
        delete c;

    delete board;
}

Tree::Tree() :
    root(NULL)
{}

Tree::Tree(Node* r) :
    root(r)
{}

Tree::~Tree()
{
    delete root;
}

Game::Game()
{
    Board *start = new Board(StartingFEN);
    mainLineEnd = new Node(start);
    game = new Tree(mainLineEnd);
}

Game::~Game()
{
    delete game;
}

void Game::MakeMove(const std::string &uci)
{
    Board *next = mainLineEnd->board->MakeMove(Move(uci));
    Node *node = new Node(next, mainLineEnd);
    mainLineEnd->children.push_back(node);
    mainLineEnd = node;
}

std::vector<std::string> Game::LegalMoves()
{
    std::vector<Move> moves = mainLineEnd->board->LegalMoves();
    std::vector<std::string> uci_moves(moves.size());

    for (size_t i = 0; i < moves.size(); ++i)
        uci_moves[i] = moves[i].UCI();

    return uci_moves;
}
