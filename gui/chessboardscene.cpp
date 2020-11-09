#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include "chessboardscene.h"

#include <iostream>

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

