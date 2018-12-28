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
    Controller(int width, int height);

    void run();
    void mousePressEvent(QMouseEvent* event);

private:
    QGraphicsScene* scene;
    Airport* airport;
    std::vector<Airplane*> planes;

    const static int width;
    const static int height;
};

#endif // CONTROLLER_H
