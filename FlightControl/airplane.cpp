#include "airplane.h"
#include <QPainter>
#include <QStyleOption>
#include <cmath>
#include <QRectF>

const double Airplane::fuelUse = 1;
const double Airplane::fuelCap = 1000;
const double Airplane::speed = 2.5;
const double Airplane::maxAngle = 0.05;
int Airplane::nOfPlanes = 0;

static double normalizeAngle(double angle)
{
    while (angle < 0) {
        angle += 360;
    }

    while (angle > 360) {
        angle -= 360;
    }

    return angle;
}

Airplane::Airplane(QPointF pos, const QPointF target, double fuel)
{
    // First we draw the plane (a circle for now)

    setPos(pos);


    // Plane is flying from pos to target, and spawns with some fuel
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

    QLineF dir(pos, target);
    currentAngle = std::acos(dir.dx() / dir.length());
    if(dir.dy() < 0)
        currentAngle = 2 * M_PI - currentAngle;
    currentAngle = qRadiansToDegrees(currentAngle);
    currentAngle += 90;
    currentAngle = normalizeAngle(currentAngle);
    if(currentAngle > 180)
        currentAngle -= 360;
    setRotation(currentAngle);
    //qDebug() << currentAngle;

    // Call update() every 50 miliseconds
    static QTimer timer;
    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    timer.start(50);

    // Move the plane ever 50 miliseconds
    this->timer = new QTimer();
    connect(this->timer, SIGNAL(timeout()), this, SLOT(move()));
    this->timer->start(50);


//    qDebug() << "Spawned a plane at " << pos;

}

Airplane::~Airplane()
{
    delete timer;
//    qDebug() << "Flight-" + QString::number(flightNo) + " just State::CRASHED";
}

QRectF Airplane::boundingRect() const
{
    qreal adjust = -0.5;
    return QRectF(-18 - adjust, -22 - adjust,
                  36 + adjust, 60 + adjust);
}

// Funkcija koja vraca preciznije granice misa - radi detekcije kolizije
QPainterPath Airplane::shape() const
{
    QPainterPath path;
    path.addRect(-10, -20, 20, 40);
    return path;
}

void Airplane::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    // Crtamo telo
    painter->setBrush(QColor());
    painter->drawEllipse(-10, -20, 20, 40);


//    if (QStyleOptionGraphicsItem::
//            levelOfDetailFromTransform(
//                painter->worldTransform()
//            ) >= .75) {

        // Crtamo oci
        painter->setBrush(Qt::white);
        painter->drawEllipse(-10, -17, 8, 8);
        painter->drawEllipse(2, -17, 8, 8);

        // Crtamo nos
        painter->setBrush(Qt::black);
        painter->drawEllipse(QRectF(-2, -22, 4, 4));


        // Crtamo usi
        painter->setBrush(scene()->collidingItems(this).isEmpty()
                ? Qt::darkYellow : Qt::red);
        painter->drawEllipse(-17, -12, 16, 16);
        painter->drawEllipse(1, -12, 16, 16);

        // Crtamo rep
        QPainterPath path(QPointF(0, 20));
        path.cubicTo(-5, 22, -5, 22, 0, 25);
        path.cubicTo(5, 27, 5, 32, 0, 30);
        path.cubicTo(-5, 32, -5, 42, 0, 35);
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(path);
//    }
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

double Airplane::calcFuel(QPointF o, QPointF t)
{
    QPointF d = o - t;
    return qSqrt(d.x() * d.x() + d.y() * d.y()) / Airplane::speed;
}

void Airplane::move(){

    if(state == State::CRASHED) return;

    // Move the plane forward
    setPos(mapToParent(0, -speed));
    setRotation(-currentAngle);
    setScale(0.75);
//    setPos(pos() - speed * direction);
    fuel -= fuelUse;

    if(state == State::MANUAL){
        return;
    }else if(state == State::FLYING){
        moveToTarget();
    }else if(state == State::HOLDING){
        holdingPattern();
    }else if(state == State::LANDING){
        landAndRefuel();
    }else if(state == State::REFUELING){
        if(!incoming){
            deleteLater();
        }
        incoming = !incoming;
        state = State::FLYING;
        timer->start(50);
    }


}

void Airplane::update()
{
    if(state == State::CRASHED) {
        qDebug() << "prvi kresd";
        qDebug() << flightNo;
//        deleteLater();
//        return;
    }

    // TODO: Steer if there are planes dangerously close

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
        qDebug() << flightNo << "crashed";
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
    qDebug() << "Flight-" + QString::number(flightNo) + " arrived to target";
    QPointF t = target;
    setTarget(origin);
    setOrigin(t);
    state = State::REFUELING;
    this->fuel = fuelCap;
}

void Airplane::steer(double theta)
{
    double x = qCos(theta) * direction.x() - qSin(theta) * direction.y();
    double y = qSin(theta) * direction.x() + qCos(theta) * direction.y();

    direction.setX(x);
    direction.setY(y);

    QLineF oldDir(mapToScene(0, -100), mapToScene(0, 0));
    QLineF newDir(QPointF(0, 0), direction * 100);

    // Angle between previous and current direction vector
    double angle = oldDir.angleTo(newDir);

//    qDebug() << "prvi";
//    qDebug() << angle;
    angle = normalizeAngle(angle);
//    if(angle > 180) {
//        angle = angle - 360;
//    }
//    qDebug() << "drugi";
//    qDebug() << angle;

//    if(angle > 359.9 || angle < 0.1) {
//        angle = 0;
//    }
    currentAngle += qSin(qDegreesToRadians(angle)) * 10;
    currentAngle = normalizeAngle(currentAngle);
    if(currentAngle > 180)
        currentAngle -= 360;
//    qDebug() << rotation();
//    qDebug() << currentAngle;
}

void Airplane::setTarget(const QPointF target)
{
    this->target = target;
}

void Airplane::setOrigin(const QPointF origin)
{
    this->origin = origin;
}
