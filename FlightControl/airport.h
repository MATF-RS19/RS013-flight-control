#ifndef AIRPORT_H
#define AIRPORT_H

#include "airplane.h"
#include <algorithm>
#include <QPointer>

class Airport : public QObject, public QGraphicsEllipseItem{

Q_OBJECT

public:
    Airport();
    ~Airport();

    QVector<QPointer<Airplane>> planes;
    QPointer<Airplane> currentPlane;

public slots:
    void update();

private:
    double radarRadius;

    void schedule();

    bool occupied;

    QTimer* timer;

    double solutionValue(const QVector<QPointer<Airplane>> &planes);

    void localSearch(QVector<QPointer<Airplane>>& planesInRadar);
};

#endif // AIRPORT_H
