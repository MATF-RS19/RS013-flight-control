#include "controller.h"


Controller::Controller()
{
    // Create the scene
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0,0,1000,1000);
    setScene(scene);

    setFixedSize(1000,1000);
}

void Controller::run()
{
    // Create an airport and add it to scene
    airport = new Airport();
    airport->setRect(0,0,20,20);
    airport->setPos(500,500);

    scene->addItem(airport);

    show();
}

void Controller::mousePressEvent(QMouseEvent *event)
{
    // Spawn a new airplane on the clicked location (this is for testing purposes)
    if(event->button() == Qt::LeftButton){
        Airplane* air = new Airplane(event->pos(), airport->pos(), 1000);
        scene->addItem(air);
    }
}
