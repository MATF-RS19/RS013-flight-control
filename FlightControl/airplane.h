#ifndef AIRPLANE_H
#define AIRPLANE_H

#include <QGraphicsEllipseItem>
#include <QPointF>
#include <QObject>
#include <QtMath>
#include <QDebug>
#include <QTimer>
#include <QGraphicsScene>

class Airplane: public QObject, public QGraphicsEllipseItem{
    Q_OBJECT
public:
    Airplane(QPointF pos, const QPointF target, double fuel);

public slots:
    void move();

private:
    void setTarget(const QPointF target);
    void setOrigin(const QPointF origin);
    void refuel();

    static const double speed;
    static const double fuelCap;
    static const double fuelUse;
    double fuel;

    bool inFlight;

    // Plane is flying from origin to target
    QPointF origin;
    QPointF target;

    QTimer* timer;
};

#endif // MYRECT_H
