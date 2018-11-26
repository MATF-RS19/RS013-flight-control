#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QDebug>
#include "airplane.h"
#include "airport.h"

class Controller : public QGraphicsView
{
public:
    Controller();

    void run();
    void mousePressEvent(QMouseEvent* event);

private:
    QGraphicsScene* scene;
    Airport* airport;
};

#endif // CONTROLLER_H
