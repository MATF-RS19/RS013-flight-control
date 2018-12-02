#ifndef AIRPORT_H
#define AIRPORT_H

#include "airplane.h"
#include <algorithm>

class Airport : public QObject, public QGraphicsEllipseItem{

Q_OBJECT

public:
    Airport();
    ~Airport();

    QVector<Airplane*> planes;
    Airplane* currentPlane;

public slots:
    void update();


private:
    void schedule();

    bool occupied;

    QTimer* timer;
};

#endif // AIRPORT_H
