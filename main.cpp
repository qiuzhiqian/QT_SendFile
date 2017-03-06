#include "sendfile.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SendFile w;
    w.show();

    return a.exec();
}
