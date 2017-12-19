#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    sqSize = 50;
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            squares[i * 8 + j] = scene->addRect(i * sqSize, j * sqSize, sqSize, sqSize,
                                 QPen(Qt::black), (i + j) % 2 ? QBrush(Qt::darkGreen) : QBrush(Qt::lightGray));

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
            pixmapPieces[i * 6 + j] = QPixmap("../gui/pieces/" + s + "/" + col[i] + nam[j] + ".png").scaled(QSize(sqSize, sqSize));
}

void MainWindow::SetFEN()
{
    QString text = QApplication::clipboard()->text();
    b.SetFEN(text.toStdString());
    RefreshBoard();
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
                pieces[(7 - i) * 8 + j]->setOffset(sqSize * j, sqSize * (7 - i));
                pieces[(7 - i) * 8 + j]->setVisible(true);
            }
        }
}
