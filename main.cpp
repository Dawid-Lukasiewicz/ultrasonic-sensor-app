#include "setportwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SetPortWindow w;
    w.move(QApplication::desktop()->screen()->rect().center()
           -w.rect().center());
    w.show();
    return a.exec();
}
