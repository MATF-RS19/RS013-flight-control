#ifndef AIRPORT_H
#define AIRPORT_H

#include "airplane.h"
#include <algorithm>
#include <QPointer>
#include <QPainter>

class Airport : public QObject, public QGraphicsEllipseItem {

Q_OBJECT

public:
    Airport(QString name);
    ~Airport();

    QRectF boundingRect() const override;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    QVector<QPointer<Airplane>> planes;
    QPointer<Airplane> currentPlane;

public slots:
    void update();

private:
    QString name;

    double radarRadius;

    void schedule();

    bool occupied;

    QTimer* timer;

    double solutionValue(const QVector<QPointer<Airplane>> &planes);

    void localSearch(QVector<QPointer<Airplane>>& planesInRadar);
};

#endif // AIRPORT_H
