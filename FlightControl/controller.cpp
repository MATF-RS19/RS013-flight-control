#include "controller.h"


const int Controller::width = 600;
const int Controller::height = 600;

Controller::Controller()
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
        Airplane* air = new Airplane(mapToScene(event->pos()), airport->pos(), 1000);
        scene->addItem(air);
    }
}
