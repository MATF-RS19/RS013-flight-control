#include "airport.h"
#include <random>

const double Airport::radarRadius(100);

Airport::Airport(QString name)
    : name(std::move(name))
{
    setRect(0,0,20,20);

    selected = false;

    // Call update() every 50 miliseconds
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(50);
}

Airport::~Airport()
{
    delete timer;
}

QRectF Airport::boundingRect() const
{
    return QRectF(-20, -20, 40, 40);
}

void Airport::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    QPixmap img(":/images/00_airport.png");
    painter->drawPixmap(-15, -15, 30, 30, img);

    QFont font = painter->font();
    font.setPointSize(font.pointSize() * 2);
    painter->setFont(font);

    if(selected)
        painter->setPen(Qt::green);
    painter->drawText(0, -10, name);

    if(selected)
        painter->drawRect(boundingRect());

}

QString Airport::getName()
{
    return name;
}

bool Airport::isSelected()
{
    return selected;
}

void Airport::select()
{
    selected = true;
}

void Airport::deselect()
{
    selected = false;
}

void Airport::update()
{
    planes.erase(std::remove_if(planes.begin(), planes.end(),
                                [](const QPointer<Airplane>& p){return p.isNull();}),
                planes.end());

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
        double optimalFuel = p->fuelUse * p->getDistance() / p->speed;
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
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<int> randomDiscrete(0, planesInRadar.size() - 2);

    auto current = planesInRadar;
    double currentSolution = solutionValue(current);
    auto best = current;
    double bestSolution = currentSolution;
    for(int i = 0; i < 50; i++) {
        int index = randomDiscrete(gen);
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
    QVector<QPointer<Airplane>> planesInRadar;
    std::copy_if(planes.begin(), planes.end(),
                 std::back_inserter(planesInRadar),
                 [](const QPointer<Airplane>& p){
                    return p->getDistance() < radarRadius
                           && p->getState() != State::MANUAL
                           && p->getState() != State::DANGER; });


    if(planesInRadar.empty()) {
        return;
    }

    localSearch(planesInRadar);


    for(const auto& p : planesInRadar) {
        if(p == currentPlane || p->getState() == State::MANUAL || p->getState() == State::DANGER) continue;
        p->setState(State::HOLDING);
    }

    if(!currentPlane || (currentPlane->getDistance() > radarRadius/2)) {
        planesInRadar[0]->setState(State::FLYING);
        currentPlane = planesInRadar[0];
    }
}

