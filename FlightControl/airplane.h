#ifndef AIRPLANE_H
#define AIRPLANE_H

#include <QGraphicsItem>
#include <QPointF>
#include <QObject>
#include <QtMath>
#include <QDebug>
#include <QTimer>
#include <QGraphicsScene>

enum class State {FLYING, HOLDING, CRASHED, MANUAL, DANGER};

struct AirplaneData{
    QString image;
    double fuelCap;
    double fuelUse;
    double speed;
    double maxAngle;
};

class Airplane: public QObject, public QGraphicsItem {
    Q_OBJECT
public:

    Airplane(QPointF pos, const QPointF target, int type=-1);

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

    QPointF getTarget();
    void setTarget(const QPointF target);

    double calcFuel(QPointF origin, QPointF target);

    int flightNo;

    static int nOfPlanes;

    const int type;
    const QString image;
    const double speed;
    const double maxAngle;
    const double fuelCap;
    const double fuelUse;

    static QVector<AirplaneData> data;

    // The function changes the direction of the plane by angle theta
    void steer(double theta);

public slots:
    void update();
    void move();

signals:
    void finished(const QString&, bool);

private:

    void moveToTarget();
    void holdingPattern();
    void land();

    double calculateAngle();

    void setOrigin(const QPointF origin);

    double fuel;

    double wastedFuel;

    State state;

    bool steerLeft;
    bool stillDangerous;

    // Plane is flying from origin to target
    QPointF origin;
    QPointF target;

    // Direction the plane is moving in
    QPointF direction;

    double currentAngle;
};

#endif // MYRECT_H
