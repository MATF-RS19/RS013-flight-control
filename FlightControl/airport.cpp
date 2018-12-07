#include "airport.h"

Airport::Airport()
{
    setRect(0,0,20,20);

    currentPlane = nullptr;
    occupied = false;

    // Call update() every 50 miliseconds
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

    timer->start(50);

}

Airport::~Airport()
{
    delete timer;
}

void Airport::update()
{
//    qDebug() << planes.length();
    for(int i=0; i<planes.length(); i++){

        if(planes[i]->getState() == CRASHED){

            if(currentPlane == planes[i]){
                currentPlane = nullptr;
                occupied = false;
            }

            delete planes[i];
            planes.remove(i);

            qDebug() << planes.length();

        }

    }

    if(currentPlane != nullptr){

        //qDebug() << currentPlane->flightNo;
    }

    if(!planes.empty()){
        schedule();
    }
}

void Airport::schedule()
{

    // Sort the planes by the distance from the airport
    std::sort(planes.begin(), planes.end(),
              [this] (const Airplane* p1, const Airplane* p2) -> bool
    {
        QPointF v1 = p1->pos() - this->pos();
        double d1 = qSqrt(v1.x() * v1.x() + v1.y() * v1.y());
        QPointF v2 = p2->pos() - this->pos();
        double d2 = qSqrt(v2.x() * v2.x() + v2.y() * v2.y());

        return d1 < d2;
    }
              );

    // Some tmp logic, has a bug somewhere
    for(auto plane : planes){
        if(!plane->isIncoming()) continue;

        if(currentPlane == nullptr && plane->getState() != CRASHED){
            currentPlane = plane;
            currentPlane->setState(FLYING);
        }

        if(plane != currentPlane){
            QPointF v = plane->pos() - pos();
            double dist = qSqrt(v.x() * v.x() + v.y() * v.y());

            if(dist < 200)
                plane->setState(HOLDING);
        }
    }

    if(currentPlane != nullptr){
        if(currentPlane->getState() == LANDING or currentPlane->getState() == REFUELING){
            occupied = true;
        }else{
            if(occupied == true){
                currentPlane = nullptr;
                occupied = false;
            }
        }
     }


}

