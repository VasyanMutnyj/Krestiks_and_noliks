#include <QApplication>
#include "game.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
  //  MainWindow w;
  //  w.show();
    Game v;
    v.show();
    return a.exec();
}
