#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QInputDialog>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QPixmap>
#include <QClipboard>
#include <QMessageBox>
#include "board.h"

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
    // Graphics
    Ui::MainWindow* ui;
    QGraphicsScene* scene;
    QGraphicsRectItem* squares[64];
    QGraphicsPixmapItem* pieces[64];
    QPixmap pixmapPieces[12];
    int sqSize;

    // Base
    Board b;

    void InitPieces(const QString& set);
    void RefreshBoard();

private slots:
    void SetFEN();
};

#endif // MAINWINDOW_H
