#include "MicrologicIDE.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MicrologicIDE w;
    w.show();

    return a.exec();
}
