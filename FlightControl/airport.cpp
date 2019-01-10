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
    auto maxFuelWastedPlane = std::max_element(planes.cbegin(),
                                               planes.cend(),
                                               [](const QPointer<Airplane>& p1, const QPointer<Airplane>& p2)
                                                   {return p1->getWastedFuel() < p2->getWastedFuel();});
    double maxFuelWasted = (*maxFuelWastedPlane)->getWastedFuel();
    double totalFuel = 0;

    for(const auto& p : planes) {
        // plane moving straight to airport
        double optimalFuel = p->getDistance() / Airplane::speed;
        totalFuel += optimalFuel;
        // plane is going to run out of fuel
        if(totalFuel > p->getFuel()) {
            return 1000000000;
        }
        double wastedFuel = p->getWastedFuel() + totalFuel - optimalFuel;
        if(wastedFuel > maxFuelWasted) {
            maxFuelWasted = wastedFuel;
        }
    }

    return maxFuelWasted;
}

// Minimize the maximum fuel wasted
void Airport::localSearch(QVector<QPointer<Airplane>>& planesInRadar)
{
    if(planesInRadar.size() < 2) return;
    // Sort the planes by the fuel wasted - time already spent waiting
    std::sort(planesInRadar.begin(), planesInRadar.end(),
              [] (const QPointer<Airplane> p1, const QPointer<Airplane> p2)
                    {
                        return p1->getWastedFuel() > p2->getWastedFuel();
                    }
              );

    auto current = planesInRadar;
    double currentSolution = solutionValue(current);
    auto best = current;
    double bestSolution = currentSolution;
    for(int i = 0; i < 50; i++) {
        int index = std::rand() % (planesInRadar.size() - 1);
        auto tmp = current;
        std::swap(tmp[index], tmp[index + 1]);
        double tmpSolution = solutionValue(tmp);
        if(tmpSolution < currentSolution) {
            current = tmp;
            currentSolution = tmpSolution;
        }
        if(tmpSolution < bestSolution) {
            best = tmp;
            bestSolution = tmpSolution;
        }
    }


    planesInRadar = best;
}

void Airport::schedule()
{
    QVector<QPointer<Airplane>> incomingPlanesInRadar;
    std::copy_if(planes.begin(), planes.end(),
                 std::back_inserter(incomingPlanesInRadar),
                 [this](const QPointer<Airplane>& p){
                    return p->isIncoming()
                           && p->getDistance() < radarRadius
                           && p->getState() != State::MANUAL
                           && p->getState() != State::DANGER; });


    if(incomingPlanesInRadar.empty()) {
        return;
    }

    localSearch(incomingPlanesInRadar);


    for(const auto& p : incomingPlanesInRadar) {
        if(p == currentPlane) continue;
        if(p->getState() != State::DANGER)
            p->setState(State::HOLDING);
    }
    if(!currentPlane) {
        incomingPlanesInRadar[0]->setState(State::FLYING);
        currentPlane = incomingPlanesInRadar[0];
    }
}

