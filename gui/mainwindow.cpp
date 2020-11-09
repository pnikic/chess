#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QClipboard>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QGraphicsSceneMouseEvent>
#include <QTransform>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new ChessBoardScene(this);
    ui->graphicsView->setScene(scene);

    int chessboardWidth= ui->graphicsView->width();
    squareSize = chessboardWidth / 8;
    QBrush whiteBrush(whiteSquare), blackBrush(blackSquare);

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            squares[i * 8 + j] = scene->addRect(i * squareSize, j * squareSize, squareSize, squareSize,
                                                Qt::NoPen, (i + j) % 2 ? QBrush(blackSquare) : QBrush(whiteSquare));
        }
    }

    for (int i = 0; i < 64; ++i)
        pieces[i] = new QGraphicsPixmapItem;

    InitPieces("merida");

    connect(ui->actionLoad_FEN, SIGNAL (triggered()), this, SLOT (SetFEN()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitPieces(const QString& s)
{
    QString nam[] = {"_pawn", "_knight", "_bishop", "_rook", "_queen", "_king"};
    QString col[] = {"white", "black"};

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 6; ++j)
            pixmapPieces[i * 6 + j] = QPixmap("../gui/pieces/" + s + "/" + col[i] + nam[j] + ".png").
                scaled(QSize(squareSize, squareSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void MainWindow::SetFEN()
{
    QString text = QApplication::clipboard()->text();
    try
    {
        b.SetFEN(text.toStdString());
        RefreshBoard();
    }
    catch (std::exception& e)
    {
        QMessageBox msg;
        msg.setText(e.what());
        msg.exec();
    }
}

void MainWindow::RefreshBoard()
{
    for (QGraphicsPixmapItem* gi : pieces)
        gi->setVisible(false);

    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
        {
            Piece at = b.PieceAt(Square(i, j));
            if (!at.IsNone())
            {
                pieces[(7 - i) * 8 + j] = scene->addPixmap(pixmapPieces[at.Type() + (at.Side() ? 6 : 0)]);
                pieces[(7 - i) * 8 + j]->setOffset(squareSize * j, squareSize * (7 - i));
                pieces[(7 - i) * 8 + j]->setVisible(true);
            }
        }
}
