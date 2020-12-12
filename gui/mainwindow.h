#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QColor;
class QGraphicsScene;
class QMainWindow;
class QPixmap;

namespace Ui {
    class MainWindow;
}

class ChessBoardScene;
class Board;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    // Graphics
    Ui::MainWindow* ui;
    ChessBoardScene* scene;

    // Base
    Board *B;

    // Methods
    void InitPieces(const QString& set);
    void RefreshBoard();

private slots:
    void SetFEN();
};

#endif // MAINWINDOW_H
