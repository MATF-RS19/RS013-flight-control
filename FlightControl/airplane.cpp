#include "airplane.h"

const double Airplane::fuelCap = 1000;
const double Airplane::fuelUse = 1;
const double Airplane::speed = 5;

Airplane::Airplane(QPointF pos, const QPointF target, double fuel)
{
    // First we draw the plane (a circle for now)
    setRect(0,0,10,10);
    setPos(pos);

    // Plane is traveling from pos to target, and spawns with some fuel
    setOrigin(pos);
    setTarget(target);
    this->fuel = fuel;
    this->inFlight = true;

    // Call move() every 50 miliseconds
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(move()));

    timer->start(50);
}


void Airplane::refuel()
{

    this->fuel = fuelCap;
    //qDebug() << "Refueled";
}

void Airplane::move()
{
    // Calculate direction vector of the plane
    QPointF d = pos() - target;
    double toTarget = qSqrt(d.x() * d.x() + d.y() * d.y());

    // If plane arrived at target, take some time to refuel than go back to origin
    if(toTarget <= 10){
        timer->start(1000);
        inFlight = false;
        refuel();
        //qDebug() << "Plane arrived at target";
        QPointF t = target;
        setTarget(origin);
        setOrigin(t);
        return;
    }
    // If the plane is not in flight, start the flight
    if(!inFlight){
        timer->start(50);
        inFlight = true;
    }

    // Move the plane toward the target
    d /= qSqrt(d.x() * d.x() + d.y() * d.y());
    setPos(pos() - d * speed);

    // If the fuel runs out, the plane crashes
    fuel -= fuelUse;
    if(fuel <= 0){
        qDebug() << "The plane crashed due to lack of fuel";
        delete this;
    }


}

void Airplane::setTarget(const QPointF target)
{
    this->target = target;
}

void Airplane::setOrigin(const QPointF origin)
{
    this->origin = origin;
}
