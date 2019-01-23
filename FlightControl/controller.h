#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QDebug>
#include <QPointer>
#include <QTime>
#include <QLabel>
#include <QHBoxLayout>
#include "airplane.h"
#include "airport.h"
#include "log.h"


class Log;

class Controller : public QGraphicsView{

Q_OBJECT

public:
    Controller(int width, int height);


signals:
    void flightInfo(const QString&);
    void landingInfo(const QString&, bool);
    void airplaneInfo(const QString&);

public slots:
    void update();
    void spawnPlanes();
    void planeFinished(const QString& s, bool crashed);
    void save();


private:

    Log *log;

    QGraphicsScene* scene;
    QVector<Airport*> airports;

    QPointer<Airplane> focusedPlane;
    QPointer<Airport> selectedAirport1;
    QPointer<Airport> selectedAirport2;


    const static int width;
    const static int height;

    void run(int width, int height);

    void saveAirportData(QString file);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void wheelEvent(QWheelEvent* event);

    int scaleCounter;
    int planeCounter;
    bool buildAirport;

    double originX;
    double originY;

    Airport *findClosestAirport(const QPointF &airplanePos);

    QVBoxLayout *layout;
    QLabel *lbl;
};

#endif // CONTROLLER_H
