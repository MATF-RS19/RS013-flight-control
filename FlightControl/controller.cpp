#include "controller.h"
#include <QPointer>

Controller::Controller(int width, int height)
{
    // Create the scene
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0,0,width, height);
    setScene(scene);
    fitInView(0,0, width, height, Qt::KeepAspectRatio);
    setFixedSize(width, height);
}

void Controller::run()
{
    // Create an airport and add it to scene
    airport = new Airport();
    airport->setPos(300,300);

    scene->addItem(airport);

    show();
}

void Controller::mousePressEvent(QMouseEvent *event)
{
    // Spawn a new airplane on the clicked location (this is for testing purposes)
    if(event->button() == Qt::LeftButton){
        Airplane* plane = new Airplane(mapToScene(event->pos()), airport->pos(), 1000);
        airport->planes.push_back(plane);
        scene->addItem(plane);
    }
}
