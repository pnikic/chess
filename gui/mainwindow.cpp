// Base
#include "../base/board.h"

// Gui
#include "chessboardscene.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

// Qt
#include <QClipboard>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Set up the user interface
    ui->setupUi(this);
    int chessboardViewWidth = ui->chessboardView->width();
    scene = new ChessBoardScene(chessboardViewWidth, this);
    ui->chessboardView->setScene(scene);

    // Draw the starting position on the scene
    B = new Board(StartingFEN);
    scene->UpdateBoard(B);

    connect(ui->actionLoad_FEN, SIGNAL (triggered()), this, SLOT (SetFEN()));
}

MainWindow::~MainWindow()
{
    delete B;
    delete ui;
}

void MainWindow::SetFEN()
{
    // Set FEN from clipboard

    QString text = QApplication::clipboard()->text();
    try
    {
        B->SetFEN(text.toStdString());
        scene->UpdateBoard(B);
    }
    catch (std::exception& e)
    {
        QMessageBox msg;
        msg.setText(e.what());
        msg.exec();
    }
}
