#include "airplane.h"

const double Airplane::fuelCap = 1000;
const double Airplane::fuelUse = 1;
const double Airplane::speed = 2.5;
const double Airplane::maxAngle = 0.05;
int Airplane::nOfPlanes = 0;

Airplane::Airplane(QPointF pos, const QPointF target, double fuel)
{
    // First we draw the plane (a circle for now)
    setRect(0,0,10,10);
    setPos(pos);

    // Plane is State::FLYING from pos to target, and spawns with some fuel
    setOrigin(pos);
    setTarget(target);
    this->fuel = fuel;
    state = State::FLYING;
    incoming = true;

    flightNo = nOfPlanes++;

    // Calculate direction vector of the plane
    direction = pos - target;
    double toTarget = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
    direction /= toTarget;

    // Call update() every 50 miliseconds
    static QTimer timer;
    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    timer.start(50);

    // Move the plane ever 50 miliseconds
    this->timer = new QTimer();
    connect(this->timer, SIGNAL(timeout()), this, SLOT(move()));
    this->timer->start(50);

}

Airplane::~Airplane()
{
    delete timer;
    qDebug() << "Flight-" + QString::number(flightNo) + " just State::CRASHED";
}

State Airplane::getState()
{
    return state;
}

void Airplane::setState(State state)
{
    if(this->state != State::CRASHED){

        this->state = state;

    }
}

double Airplane::getDistance()
{
    QPointF d = pos() - target;
    return qSqrt(d.x() * d.x() + d.y() * d.y());
}

double Airplane::getFuel()
{
    return fuel;
}

bool Airplane::isIncoming()
{
    return incoming;
}

void Airplane::move(){

    if(state == State::CRASHED) return;

    // Move the plane forward
    setPos(pos() - direction * speed);
    fuel -= fuelUse;

    if(state == State::FLYING){
        moveToTarget();
    }else if(state == State::HOLDING){
        holdingPattern();
    }else if(state == State::LANDING){
        landAndRefuel();
    }else if(state == State::REFUELING){
        incoming = !incoming;
        state = State::FLYING;
        timer->start(50);
    }


}

void Airplane::update(){

    if(state == State::CRASHED) {
        qDebug() << "prvi kresd";
        qDebug() << flightNo;
//        deleteLater();
//        return;
    }
    // Check if the plane collided with other planes and if so, destroy all planes that collided
    QList<QGraphicsItem*> crashedPlanes = scene()->collidingItems(this);
    foreach(QGraphicsItem* item, crashedPlanes){
        Airplane* plane = dynamic_cast<Airplane*>(item);
        if(plane && plane->state != State::CRASHED){
            plane->setState(State::CRASHED);
            state = State::CRASHED;
            plane->deleteLater();
        }
    }


    // If the fuel runs out, the plane crashes
    if(fuel <= 0){
        state = State::CRASHED;
        qDebug() << "Flight-" + QString::number(flightNo) + " ran out of fuel";
//        return;
    }

    if(state == State::CRASHED) {
        qDebug() << "drugi kresd";
        qDebug() << flightNo;
        deleteLater();
    }
}

void Airplane::moveToTarget(){

    // If plane arrived at target, land and refuel
    QPointF d = pos() - target;
    double toTarget = qSqrt(d.x() * d.x() + d.y() * d.y());

    if(toTarget <= 5) {
//        qDebug() << "aaa";
        state = State::LANDING;
    }

    // Determine if the plane should steer left or right
    double a = direction.y();
    double b = -direction.x();
    double c = direction.x() * y() - direction.y() * x();
    double f = a * target.x() + b * target.y() + c;

    double dot = d.x() * direction.x() + d.y() * direction.y();
    double d_norm = toTarget;
    double dir_norm = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());

    double pam = dot / (d_norm * dir_norm);
    if(pam > 1) pam = 1;
    if(pam < -1) pam = -1;
    double angle = acos(pam);
    if(angle > maxAngle) angle = maxAngle;
    if(f > 0) {
        steer(angle);
    } else {
        steer(-angle);
    }

}


void Airplane::holdingPattern(){

    steer(0.025);
}

void Airplane::landAndRefuel(){

    timer->start(1000);
    this->fuel = fuelCap;
    qDebug() << "Flight-" + QString::number(flightNo) + " arrived to target";
    QPointF t = target;
    setTarget(origin);
    setOrigin(t);
    state = State::REFUELING;
}


void Airplane::steer(double theta)
{
    double x = qCos(theta) * direction.x() - qSin(theta) * direction.y();
    double y = qSin(theta) * direction.x() + qCos(theta) * direction.y();

    direction.setX(x);
    direction.setY(y);

}


void Airplane::setTarget(const QPointF target)
{
    this->target = target;
}

void Airplane::setOrigin(const QPointF origin)
{
    this->origin = origin;
}
