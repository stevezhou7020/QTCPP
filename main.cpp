#include <QApplication>
#include"cconfiglefile.h"
#include<QtDebug>
#include "parklot.h"
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    parklot w;
    w.show();
   return a.exec();
}
