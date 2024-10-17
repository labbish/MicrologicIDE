#include "MicrologicIDE.h"
#include <direct.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    exepath = std::string(_getcwd(NULL, 0));
    QApplication a(argc, argv);
    MicrologicIDE w(0,argc>1?argv[1]:"");
    w.show();

    return a.exec();
}
