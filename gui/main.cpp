#include "mainwindow.h"
#include <QApplication>

#include "base.h"
#include "board.h"
#include "move.h"
#include "piece.h"
#include "setsqr.h"
#include "square.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
