#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QDebug>
#include <QPointer>
#include <QTime>
#include "airplane.h"
#include "airport.h"

class Controller : public QGraphicsView{

Q_OBJECT

public:
    Controller(int width, int height);


signals:
    void flightInfo(const QString&);
    void landingInfo(const QString&, bool);

public slots:
    void update();
    void spawnPlanes();
    void planeFinished(const QString& s, bool crashed);


private:
    QGraphicsScene* scene;
    QVector<Airport*> airports;

    QPointer<Airplane> focused_plane;
    QPointer<Airport> selected_airport1;
    QPointer<Airport> selected_airport2;


    const static int width;
    const static int height;

    void run(int width, int height);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void wheelEvent(QWheelEvent* event);

    int scaleCounter;
    int planeCounter;

    double originX;
    double originY;

    Airport *findClosestAirport(const QPointF &airplanePos);
};

#endif // CONTROLLER_H
