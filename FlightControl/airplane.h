#ifndef AIRPLANE_H
#define AIRPLANE_H

#include <QGraphicsItem>
#include <QPointF>
#include <QObject>
#include <QtMath>
#include <QDebug>
#include <QTimer>
#include <QGraphicsScene>

enum class State {FLYING, HOLDING, LANDING, REFUELING, CRASHED, MANUAL, DANGER};

class Airplane: public QObject, public QGraphicsItem{
    Q_OBJECT
public:
    Airplane(QPointF pos, const QPointF target, double fuel);
    ~Airplane();

    QRectF boundingRect() const override;

    QPainterPath shape() const override;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    State getState();
    void setState(State state);

    double getDistance();

    double getFuel();

    double getWastedFuel();

    bool isIncoming();

    static double calcFuel(QPointF origin, QPointF target);

    int flightNo;

    static int nOfPlanes;

    static const double speed;
    static const double maxAngle;
    static const double fuelCap;
    static const double fuelUse;


    // The function changes the direction of the plane by angle theta
    void steer(double theta);

public slots:
    void update();
    void move();


private:
    void moveToTarget();
    void holdingPattern();
    void landAndRefuel();

    double calculateAngle();

    void setTarget(const QPointF target);
    void setOrigin(const QPointF origin);

    double fuel;

    double wastedFuel;

    State state;

    bool incoming;

    bool steerLeft;
    bool stillDangerous;

    // Plane is flying from origin to target
    QPointF origin;
    QPointF target;

    // Direction the plane is moving in
    QPointF direction;

    double currentAngle;

    QTimer *timer;
};

#endif // MYRECT_H
