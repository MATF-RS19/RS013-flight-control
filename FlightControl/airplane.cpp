#include "airplane.h"

const double Airplane::fuelCap = 1000;
const double Airplane::fuelUse = 1;
const double Airplane::speed = 2.5;
int Airplane::nOfPlanes = 0;

Airplane::Airplane(QPointF pos, const QPointF target, double fuel)
{
    // First we draw the plane (a circle for now)
    setRect(0,0,10,10);
    setPos(pos);

    // Plane is flying from pos to target, and spawns with some fuel
    setOrigin(pos);
    setTarget(target);
    this->fuel = fuel;
    state = FLYING;
    theta = 0;
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
    qDebug() << "Flight-" + QString::number(flightNo) + " just crashed";
}

State Airplane::getState()
{
    return state;
}

void Airplane::setState(State state)
{
    if(this->state != CRASHED){

        this->state = state;

    }
}

bool Airplane::isIncoming()
{
    return incoming;
}

void Airplane::move(){

    if(state == CRASHED) return;

    // Move the plane forward
    setPos(pos() - direction * speed);
    fuel -= fuelUse;

    if(state == FLYING){
        moveToTarget();
    }else if(state == HOLDING){
        holdingPattern();
    }else if(state == LANDING){
        landAndRefuel();
    }else if(state == REFUELING){
        incoming = !incoming;
        state = FLYING;
        timer->start(50);
    }


}

void Airplane::update(){

    if(state == CRASHED) return;

    // Check if the plane collided with other planes and if so, destroy all planes that collided
    QList<QGraphicsItem*> crashedPlanes = scene()->collidingItems(this);
    if(!crashedPlanes.empty()){
        foreach(QGraphicsItem* item, crashedPlanes){
            Airplane* plane = dynamic_cast<Airplane*>(item);
            if(plane && plane->state != CRASHED){
                plane->setState(CRASHED);
                state = CRASHED;
            }
        }
    }

    // If the fuel runs out, the plane crashes
    if(fuel <= 0){
        state = CRASHED;
        qDebug() << "Flight-" + QString::number(flightNo) + " ran out of fuel";
        return;
    }


}

void Airplane::moveToTarget(){

    // If plane arrived at target, land and refuel
    QPointF d = pos() - target;
    double toTarget = qSqrt(d.x() * d.x() + d.y() * d.y());

    if(toTarget <= 5){
        state = LANDING;
    }

    // Update direction vector of the plane
    if(qAtan2(direction.x(), direction.y()) -
       qAtan2(d.x()/toTarget, d.y()/toTarget) < 0){
        steer(-0.025);
    }else{
        steer(0.025);
    }
    //BUG

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
    state = REFUELING;
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
