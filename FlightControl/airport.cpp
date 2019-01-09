#include "airport.h"

Airport::Airport()
{
    setRect(0,0,20,20);

    radarRadius = 400;
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
//    qDebug() << "pre";
//    qDebug() << planes.length();

    planes.erase(std::remove_if(planes.begin(), planes.end(),
                                [](const QPointer<Airplane>& p){return p.isNull();}),
                planes.end());

//    qDebug() << "posle";
//    qDebug() << planes.length();
    if(currentPlane && !currentPlane->isIncoming()) {
        currentPlane = nullptr;
    }
    if(!planes.empty()){
        schedule();
    }
}
double Airport::solutionValue(const QVector<QPointer<Airplane>>& planes)
{
    double totalSum = 0;
    double guess = 0;

    for(const auto& p : planes) {
        // 50 - timer...
        guess += p->getDistance() / Airplane::speed + 1 / 50;
        if(p->getFuel() - guess * Airplane::fuelUse < 0) return 100000000;
        // 100 - minimum...
        double real = p->getFuel() - 1000;
        if(guess > real) {
            totalSum += guess - real;
        }
    }

    return totalSum;
}

void Airport::localSearch(QVector<QPointer<Airplane>>& planesInRadar)
{
    if(planesInRadar.size() < 2) return;
    // Sort the planes by the distance from the airport
    std::sort(planesInRadar.begin(), planesInRadar.end(),
              [this] (const QPointer<Airplane> p1, const QPointer<Airplane> p2) -> bool
    {
        QPointF v1 = p1->pos() - this->pos();
        double d1 = qSqrt(v1.x() * v1.x() + v1.y() * v1.y());
        QPointF v2 = p2->pos() - this->pos();
        double d2 = qSqrt(v2.x() * v2.x() + v2.y() * v2.y());

        return d1 < d2;
    }
              );

    auto best = planesInRadar;
    double bestSolution = solutionValue(planesInRadar);
    double currentSolution;
    for(int i = 0; i < 100; i++) {
        int index = std::rand() % (planesInRadar.size() - 1);
        auto current = planesInRadar;
        std::swap(current[index], current[index + 1]);
        currentSolution = solutionValue(current);
        if(currentSolution < bestSolution) {
            best = current;
            bestSolution = currentSolution;
        }
    }

//    qDebug() << bestSolution;
    planesInRadar = best;
}

void Airport::schedule()
{
    QVector<QPointer<Airplane>> incomingPlanesInRadar;
    std::copy_if(planes.begin(), planes.end(),
                 std::back_inserter(incomingPlanesInRadar),
                 [this](const QPointer<Airplane>& p){return p->isIncoming() && p->getDistance() < radarRadius && p->getState() != State::MANUAL;});


    if(incomingPlanesInRadar.empty()) {
        return;
    }

    localSearch(incomingPlanesInRadar);


    // Sort the planes by the distance from the airport
//    std::sort(incomingPlanesInRadar.begin(), incomingPlanesInRadar.end(),
//              [this] (const Airplane* p1, const Airplane* p2) -> bool
//    {
//        QPointF v1 = p1->pos() - this->pos();
//        double d1 = qSqrt(v1.x() * v1.x() + v1.y() * v1.y());
//        QPointF v2 = p2->pos() - this->pos();
//        double d2 = qSqrt(v2.x() * v2.x() + v2.y() * v2.y());

//        return d1 < d2;
//    }
//              );

    for(const auto& p : incomingPlanesInRadar) {
        if( p == currentPlane) continue;
        p->setState(State::HOLDING);
    }
    if(!currentPlane) {
        incomingPlanesInRadar[0]->setState(State::FLYING);
        currentPlane = incomingPlanesInRadar[0];
    }
}

