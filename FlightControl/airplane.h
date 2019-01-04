#ifndef AIRPLANE_H
#define AIRPLANE_H

#include <QGraphicsEllipseItem>
#include <QPointF>
#include <QObject>
#include <QtMath>
#include <QDebug>
#include <QTimer>
#include <QGraphicsScene>

enum class State {FLYING, HOLDING, LANDING, REFUELING, CRASHED};

class Airplane: public QObject, public QGraphicsEllipseItem{
    Q_OBJECT
public:
    Airplane(QPointF pos, const QPointF target, double fuel);
    ~Airplane();

    State getState();
    void setState(State state);

    double getDistance();

    double getFuel();

    bool isIncoming();

    static double calcFuel(QPointF origin, QPointF target);

    int flightNo;

    static int nOfPlanes;

    static const double speed;
    static const double maxAngle;
    static const double fuelCap;
    static const double fuelUse;

public slots:
    void update();
    void move();


private:
    void moveToTarget();
    void holdingPattern();
    void landAndRefuel();

    // The function changes the direction of the plane by angle theta
    void steer(double theta);

    void setTarget(const QPointF target);
    void setOrigin(const QPointF origin);

    double fuel;

    State state;

    bool incoming;

    // Plane is flying from origin to target
    QPointF origin;
    QPointF target;

    // Direction the plane is moving in
    QPointF direction;

    QTimer *timer;
};

#endif // MYRECT_H
