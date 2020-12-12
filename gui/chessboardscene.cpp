// Base
#include "../base/board.h"

// Gui
#include "chessboardscene.h"

// Qt
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QTransform>

// Debug
#include <iostream>

ChessBoardScene::ChessBoardScene(quint32 sceneSize, QObject *parent) :
    sceneSize(sceneSize),
    QGraphicsScene(parent)
{
    squareSize = sceneSize / 8;
    QBrush whiteBrush(whiteSquare), blackBrush(blackSquare);

    // Add 64 graphic items for squares to the scene
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            squares[i * 8 + j] = addRect(i * squareSize, j * squareSize, squareSize, squareSize,
                                         Qt::NoPen, (i + j) % 2 ? blackBrush : whiteBrush);


    // Initialize 64 graphic items for pieces
    for (int i = 0; i < 64; ++i)
        pieces[i] = new QGraphicsPixmapItem;

    InitPieces("merida");
}

void ChessBoardScene::InitPieces(const QString& s)
{
    // Initialize piece images

    QString nam[] = {"_pawn", "_knight", "_bishop", "_rook", "_queen", "_king"};
    QString col[] = {"white", "black"};

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 6; ++j)
            pixmapPieces[i * 6 + j] = QPixmap("../gui/pieces/" + s + "/" + col[i] + nam[j] + ".png").
                scaled(QSize(squareSize, squareSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void ChessBoardScene::UpdateBoard(Board *B)
{
    for (QGraphicsPixmapItem* gi : pieces)
        gi->setVisible(false);

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            Piece at = B->PieceAt(Square(i, j));
            if (!at.IsNone())
            {
                pieces[(7 - i) * 8 + j] = addPixmap(pixmapPieces[at.Type() + (at.Side() ? 6 : 0)]);
                pieces[(7 - i) * 8 + j]->setOffset(squareSize * j, squareSize * (7 - i));
                pieces[(7 - i) * 8 + j]->setVisible(true);
            }
        }
    }
}

void ChessBoardScene::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        QGraphicsItem *item = itemAt(e->scenePos(), QTransform());
        if (item != nullptr)
        {
            item->setOpacity(.5);
        }
    }
}

void ChessBoardScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        QGraphicsItem *item = itemAt(e->buttonDownScenePos(Qt::LeftButton), QTransform());
        if (item != nullptr)
        {
            item->setOpacity(1);
        }

        item = itemAt(e->lastScenePos(), QTransform());
        if (item != nullptr)
        {
            // Do something with destination square
        }
    }
}

