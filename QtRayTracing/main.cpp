#include "QtRayTracing.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QtRayTracing w;
    w.show();

    return a.exec();
}
