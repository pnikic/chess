#ifndef GAME_H
#define GAME_H

#include <list>
#include <string>
#include <vector>

class Board;

class Node
{
public:
    Node();
    Node(Board *b);
    Node(Board *b, Node *p);
    ~Node();

    Board *board;
    Node *parent;
    std::list<Node*> children;
};

class Tree
{
public:
    Tree();
    Tree(Node* r);
    ~Tree();

    Node *root;
};

// Interface class between the GUI and the actual (chess) game
class Game
{
public:
    Game();
    ~Game();

    // void SetFen(const std::string &fen);
    void MakeMove(const std::string &uci);
    std::vector<std::string> LegalMoves();

// private:
    Tree *game;
    Node *mainLineEnd;
};

#endif //GAME_H
