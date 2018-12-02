#include <QApplication>
#include "controller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Controller* controller = new Controller(600, 600);
    controller->run();

    return a.exec();
}
