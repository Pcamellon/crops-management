#include "controlcultivos.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ControlCultivos w;
    w.setWindowState(w.windowState() ^ Qt::WindowMaximized);
    w.show();

    return a.exec();
}
