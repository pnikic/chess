#ifndef CHESSBOARDSCENE_H
#define CHESSBOARDSCENE_H

// Note: Switch to forward declarations
#include <QGraphicsScene>

class ChessBoardScene : public QGraphicsScene
{
public:
    ChessBoardScene(QObject *parent = nullptr) : QGraphicsScene(parent) {}

private:
    // Implementing mouse events
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);
    // void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
};

#endif // CHESSBOARDSCENE_H
