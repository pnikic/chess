#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "board.h"
#include "chessboardscene.h"

#include <QMainWindow>

class QColor;
class QGraphicsScene;
class QMainWindow;
class QPixmap;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    // Implementing mouse events
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e);

    // Constants
    const QColor whiteSquare = QColor(240, 217, 181);
    const QColor blackSquare = QColor(181, 136, 99);

    // Graphics
    Ui::MainWindow* ui;
    ChessBoardScene* scene;
    QGraphicsRectItem* squares[64];
    QGraphicsPixmapItem* pieces[64];
    QPixmap pixmapPieces[12];
    qint32 squareSize;

    // Base
    Board b;

    void InitPieces(const QString& set);
    void RefreshBoard();

private slots:
    void SetFEN();
};

#endif // MAINWINDOW_H
