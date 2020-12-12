#ifndef CHESSBOARDSCENE_H
#define CHESSBOARDSCENE_H

// Note: Switch to forward declarations
#include <QGraphicsScene>

class Board;

class ChessBoardScene : public QGraphicsScene
{
public:
    ChessBoardScene(quint32 sceneSize, QObject *parent = nullptr);

    void UpdateBoard(Board *B);

private:
    // Constants
    const QColor whiteSquare = QColor(240, 217, 181);
    const QColor blackSquare = QColor(181, 136, 99);

    // Piece images
    QPixmap pixmapPieces[12];

    // Graphic items for squares and pieces
    QGraphicsRectItem* squares[64];
    QGraphicsPixmapItem* pieces[64];

    // Other
    quint32 sceneSize, squareSize;

    // Methods
    void InitPieces(const QString& s);

    // Implementing mouse events
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);
    // void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
};

#endif // CHESSBOARDSCENE_H
