#include <QApplication>
#include "controller.h"
#include "log.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Controller controller(600, 600);

    Log log(&controller);
    log.show();

    return a.exec();
}
