#include "airplane.h"
#include <QPainter>
#include <QStyleOption>
#include <cmath>
#include <QRectF>

const double Airplane::fuelUse = 1;
const double Airplane::fuelCap = 2000;
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

double Airplane::calculateAngle()
{
    QLineF newDir(mapFromScene(direction), mapFromScene(0, 0));
    double angle = std::acos(newDir.dx() / newDir.length());
    if(newDir.dy() < 0)
        angle = 2 * M_PI - angle;
    angle = qRadiansToDegrees(angle);
    angle += 90;
    angle = normalizeAngle(angle);
    if(angle > 180)
        angle -= 360;
    return angle;
}

Airplane::Airplane(QPointF pos, const QPointF target, double fuel)
{
    // First we draw the plane (a circle for now)

    setPos(pos);

    wastedFuel = 0;

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

    currentAngle = calculateAngle();
    setRotation(currentAngle);

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
    return QRectF(-36 - adjust, -44 - adjust,
                  72 + adjust, 120 + adjust);
}

QPainterPath Airplane::shape() const
{
    QPainterPath path;
    qreal adjust = -0.5;
    QRectF rect(-9 - adjust, -11 - adjust,
                  18 + adjust, 30 + adjust);

    path.addRect(rect);
    return path;
}

void Airplane::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QPixmap img(":/images/01_airplane.png");
    painter->drawPixmap(-20, -20, 40, 50, img);

    painter->drawRect(-15, 30, 30, 5);
    if(state == State::DANGER) painter->setBrush(Qt::red);
    else if (state == State::FLYING) painter->setBrush(Qt::green);
    else if (state == State::HOLDING) painter->setBrush(Qt::yellow);
    else if (state == State::MANUAL) painter->setBrush(Qt::blue);
    else painter->setBrush(Qt::black);
    painter->drawEllipse(20, 30, 10, 10);

    double fuelRatio = fuel / fuelCap;
    int r = 255 * (1 - fuelRatio);
    int g = 255 * fuelRatio;
    int b = 0;
    painter->setBrush(QColor(r, g, b));
    painter->drawRect(-15, 30, 30 * fuelRatio, 5);

    painter->setRenderHint(QPainter::Antialiasing);

//    painter->drawRect(boundingRect());

//    painter->drawPolygon(QPolygonF(
//    { QPointF(0, 0),
//      QPointF(-60, 0),
//      QPointF(-60, -100),
//      QPointF(60, -100),
//      QPointF(60, 0)}
//    ));

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

double Airplane::getWastedFuel()
{
    return wastedFuel;
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
    setRotation(rotation() + currentAngle);
    setScale(0.75);
//    setPos(pos() - speed * direction);
    fuel -= fuelUse;

    if(state == State::MANUAL || state == State::DANGER){
        currentAngle = 0;
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
        timer->start(50);
        state = State::FLYING;
    }


}

void Airplane::update()
{
    if(state == State::CRASHED) {
        return;
    }

    // Steer if there are planes dangerously close
    bool inDanger = false;

    const auto dangerPlanes = scene()->items(
                QPolygonF(
                    { mapToScene(0, 0),
                      mapToScene(-50, 0),
                      mapToScene(-50, -80),
                      mapToScene(50, -80),
                      mapToScene(50, 0)}
                ), Qt::IntersectsItemShape);

    for (auto item: dangerPlanes) {

        if(state == State::LANDING || state == State::MANUAL || state == State::REFUELING) break;

        if(item == this) continue;
        Airplane* plane = dynamic_cast<Airplane*>(item);

        if(plane){
            inDanger = true;
//            qDebug() << "in danger =" << inDanger;

            QLineF lineToPlane(QPointF(0, 0), mapFromItem(item, 0, 0));

            qreal angleToPlane = acos(lineToPlane.dx() / lineToPlane.length());
            if(lineToPlane.dy() < 0) angleToPlane = M_PI * 2 - angleToPlane;

            angleToPlane = normalizeAngle((/*180 -  */qRadiansToDegrees(angleToPlane)) + 180/2);

//            qDebug() << angleToPlane;

            angleToPlane = qDegreesToRadians(angleToPlane);
//            qDebug() << "angle to plane =" << angleToPlane;


            if(angleToPlane >= M_PI) {
                // Rotate left
               steer(angleToPlane > maxAngle ? maxAngle : angleToPlane);
//               plane->steer(angleToPlane > maxAngle ? -maxAngle : -angleToPlane);
//                qDebug() << "current angle =" << currentAngle;
            } else if(angleToPlane <= M_PI) {
                // Rotate right
                steer(angleToPlane > maxAngle ? -maxAngle : -angleToPlane);
//                plane->steer(angleToPlane > maxAngle ? maxAngle : angleToPlane);
//                qDebug() << "current angle =" << currentAngle;
            }
        }
    }
    if(inDanger) {
//        qDebug() << "DANGER" << this->flightNo;
        state = State::DANGER;
    } else {
        if(state == State::DANGER) state = State::FLYING;
    }

    // Check if the plane collided with other planes and if so, destroy all planes that collided
    QList<QGraphicsItem*> crashedPlanes = scene()->collidingItems(this, Qt::IntersectsItemShape);
    foreach(QGraphicsItem* item, crashedPlanes){

        if(state == State::REFUELING) break;

        Airplane* plane = dynamic_cast<Airplane*>(item);

        if(plane && plane->state != State::CRASHED && plane->state != State::REFUELING){
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
    wastedFuel += fuelUse;
    steer(-0.0125);
//    steer(0.025);
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

    // Angle between previous and current direction vector
    currentAngle = calculateAngle();
}

void Airplane::setTarget(const QPointF target)
{
    this->target = target;
}

void Airplane::setOrigin(const QPointF origin)
{
    this->origin = origin;
}
