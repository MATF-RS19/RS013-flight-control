#include <QApplication>
#include "controller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Controller controller(600, 600);

    return a.exec();
}
