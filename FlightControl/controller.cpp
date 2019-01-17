#include "controller.h"

#include <random>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLabel>
#include <QHBoxLayout>

Controller::Controller(int width, int height)
{
    // Create the scene
    scene = new QGraphicsScene(this);
    setSceneRect(0, 0, width, height);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);


    setFixedSize(width, height);
//    fitInView(sceneRect(), Qt::KeepAspectRatio);
    setScene(scene);
    setMouseTracking(true);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    focused_plane = nullptr;
    selected_airport1 = nullptr;
    selected_airport2 = nullptr;

    // Call update() every 50 miliseconds
    static QTimer t;
    connect(&t, SIGNAL(timeout()), this, SLOT(update()));
    t.start(50);

    // Call spawnPlanes() every 15 seconds
    static QTimer t1;
    connect(&t1, SIGNAL(timeout()), this, SLOT(spawnPlanes()));
    t1.start(15000);

    scaleCounter = 0;


    run(width, height);
}

void Controller::run(int width, int height)
{
    QFile file(":/data/airports.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString val = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
//    qDebug() << doc;
    QJsonObject jsonObj = doc.object();

    QJsonValue map = jsonObj.value("map");
    QPixmap background(map.toString());
    setSceneRect(0, 0, background.width(), background.height());
//    background = background.scaled(width, height, Qt::IgnoreAspectRatio);
    setCacheMode(QGraphicsView::CacheBackground);
    setTransformationAnchor(QGraphicsView::NoAnchor);
    double scaleX = static_cast<double>(width) / background.width();
    double scaleY = static_cast<double>(height) / background.height();
    scale(scaleX, scaleY);
    setBackgroundBrush(background);


    QJsonValue value = jsonObj.value("airports");
//    qDebug() << value.toArray()[0].toObject()["name"].toString();
    QJsonArray array = value.toArray();


    for(int i = 0; i < array.size(); i++) {
        // Create an airport and add it to scene
        QJsonObject airport = array[i].toObject();
        airports.push_back(new Airport(airport["name"].toString()));
        QJsonArray position = airport["position"].toArray();
        QPoint pos(position[0].toInt(), position[1].toInt());
        airports[i]->setPos(mapToScene(pos));

//        QPointF tmp = mapToScene(pos);
        airports[i]->setFlag(QGraphicsItem::ItemIsSelectable, true);
        scene->addItem(airports[i]);
//        scene->addEllipse(tmp.x()-airports[i]->radarRadius/2, tmp.y()-airports[i]->radarRadius/2, airports[i]->radarRadius,airports[i]->radarRadius);
    }
    show();
}

static double distanceSquared(QPointF p, QPointF q)
{
    return (p.x() - q.x()) * (p.x() - q.x()) + (p.y() - q.y()) * (p.y() - q.y());
}

Airport* Controller::findClosestAirport(const QPointF& airplanePos)
{
    // This airplane's target is the closest airport
    auto closest = std::min_element(airports.cbegin(), airports.cend(),
                                    [airplanePos](Airport* a1, Airport* a2)
                                    {return distanceSquared(a1->pos(), airplanePos) <
                                            distanceSquared(a2->pos(), airplanePos);});

//    qDebug() << (*closest)->name;
    return *closest;
}

void Controller::mousePressEvent(QMouseEvent *event)
{
    // Spawn a new airplane on the clicked location (this is for testing purposes)
    if(event->button() == Qt::RightButton) {
        auto closest = findClosestAirport(mapToScene(event->pos()));
        Airplane* plane = new Airplane(mapToScene(event->pos()), closest->pos());
        connect(plane, SIGNAL(finished(QString,bool)),
                this, SLOT(planeFinished(QString,bool)));
        closest->planes.push_back(plane);
        scene->addItem(plane);
        QString s = "Flight-" + QString::number(plane->flightNo) + " : nowhere "
                + " --> " + closest->getName();
        emit flightInfo(s);
//        qDebug() <<mapToScene(event->pos());

    } else if(event->button() == Qt::LeftButton) {

        for(Airport* airport : airports) {
            // Check if this airport is clicked
            if(airport->sceneBoundingRect().contains((mapToScene(event->pos())))){
                if(!selected_airport1){
                    qDebug() << "Selected airport: " << airport->getName();
                    selected_airport1 = airport;
                    selected_airport1->setSelected(true);
                }else if(selected_airport1 != airport && !selected_airport2){
                    qDebug() << "Selected airport: " << airport->getName();
                    selected_airport2 = airport;
                    selected_airport2->setSelected(true);
                }
                return;
            }


            // Check if airplane is clicked
            foreach (Airplane* plane, airport->planes) {
    //            qDebug() << plane->boundingRect();
                if(plane->sceneBoundingRect().contains((mapToScene(event->pos())))){

                    if(focused_plane) focused_plane->setState(State::FLYING);

                    focused_plane = plane;

                    focused_plane->setState(State::MANUAL);
                    qDebug() << "Manual control of flight " << focused_plane->flightNo;
                    return;
                }
            }
        }

        if(focused_plane){
            focused_plane->setState(State::FLYING);
            focused_plane = nullptr;
        }

        if(selected_airport1){
            selected_airport1->setSelected(false);
            selected_airport1 = nullptr;
        }
        if(selected_airport2){
            selected_airport2->setSelected(false);
            selected_airport2 = nullptr;
        }


    } else if(event->button() == Qt::MiddleButton && !focused_plane){
        // Store original position.
        originX = event->x();
        originY = event->y();
    }
}

void Controller::mouseMoveEvent(QMouseEvent *event)
{

    emit airplaneInfo(QString());
    for(auto& item: scene->items()) {
        if(item->sceneBoundingRect().contains(mapToScene(event->pos()))) {
            Airplane* plane = dynamic_cast<Airplane*>(item);
            if(plane) {
                QString s = "Flight-" + QString::number(plane->flightNo);
                emit airplaneInfo(s);
            }
        }
    }

    if((event->buttons() & Qt::MidButton) && !focused_plane){
        QPointF oldp = mapToScene(originX, originY);
        QPointF newp = mapToScene(event->pos());
        QPointF translation = newp - oldp;

        translate(translation.x(), translation.y());

        originX = event->x();
        originY = event->y();
    }
}

void Controller::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Left){

        if(!focused_plane){
            qDebug() << "No plane is focused";
            return;
        }
        focused_plane->steer(-focused_plane->maxAngle);

    }else if(event->key() == Qt::Key_Right){

        if(!focused_plane){
            qDebug() << "No plane is focused";
            return;
        }
        focused_plane->steer(focused_plane->maxAngle);

    }else if(event->key() == Qt::Key_Up){

        if(!focused_plane){
            qDebug() << "No plane is focused";
            return;
        }
        if(focused_plane->getState() != State::CRASHED)
            focused_plane->setState(State::FLYING);

        focused_plane = nullptr;

    }else if(event->key() == Qt::Key_Down){

        if(!focused_plane){
            qDebug() << "No plane is focused";
            return;
        }

        // land on selected airport or automaticaly find a nearest airport to land
        auto closest = selected_airport1 ? (Airport*)selected_airport1 : findClosestAirport(focused_plane->pos());
        // Move this plane from previous target airport to closest airport
        auto targetPos = focused_plane->getTarget();
        auto currentTargetIt = std::find_if(airports.begin(), airports.end(),
                                              [targetPos](Airport* a){return a->pos() == targetPos;});
        Airport* currentTarget = *currentTargetIt;
        currentTarget->planes.erase(std::remove(currentTarget->planes.begin(), currentTarget->planes.end(), focused_plane));

        closest->planes.push_back(focused_plane);

        focused_plane->setTarget(closest->pos());

        closest->currentPlane = focused_plane;
        focused_plane->setState(State::FLYING);
        focused_plane = nullptr;

    }else if(event->key() == Qt::Key_1){
        if(selected_airport1 && selected_airport2){
            Airplane* plane = new Airplane(selected_airport1->pos(), selected_airport2->pos(), 1);
            QString s = "Flight-" + QString::number(plane->flightNo) + " : " +
                    selected_airport1->getName() + " --> " + selected_airport2->getName();
            emit flightInfo(s);
            selected_airport2->planes.push_back(plane);
            scene->addItem(plane);
        }
    }else if(event->key() == Qt::Key_2){
        if(selected_airport1 && selected_airport2){
            Airplane* plane = new Airplane(selected_airport1->pos(), selected_airport2->pos(), 3);
            QString s = "Flight-" + QString::number(plane->flightNo) + " : " +
                    selected_airport1->getName() + " --> " + selected_airport2->getName();
            emit flightInfo(s);
            selected_airport2->planes.push_back(plane);
            scene->addItem(plane);
        }
    }else if(event->key() == Qt::Key_3){
        if(selected_airport1 && selected_airport2){
            Airplane* plane = new Airplane(selected_airport1->pos(), selected_airport2->pos(), 2);
            QString s = "Flight-" + QString::number(plane->flightNo) + " : " +
                    selected_airport1->getName() + " --> " + selected_airport2->getName();
            emit flightInfo(s);
            selected_airport2->planes.push_back(plane);
            scene->addItem(plane);
        }
    }
}

void Controller::wheelEvent(QWheelEvent *event)
{

    QPointF oldp = mapToScene(event->pos());

    if(event->delta() > 0){
        scaleCounter++;
        scale(1.25, 1.25);
    }else{
        if(scaleCounter > 0) {
            scaleCounter--;
            scale(0.8, 0.8);
        }
    }

    QPointF newp = mapToScene(event->pos());

    QPointF translation = newp - oldp;

    translate(translation.x(), translation.y());
}

void Controller::update()
{

    if(focused_plane){
        originX = viewport()->size().width()/2;
        originY = viewport()->size().height()/2;

        QPointF oldp(mapToScene(originX, originY));
        QPointF newp(focused_plane->pos().x(), focused_plane->pos().y());
        QPointF translation = oldp - newp;

        translate(translation.x(), translation.y());
    }
}

void Controller::spawnPlanes()
{

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<int> randomDiscrete(1, airports.size()-1);

    int num = randomDiscrete(gen);

    for(int i = 0; i < num; i++) {

            int randomIdx = randomDiscrete(gen);
//            qDebug() << randomIdx;
            int targetIdx = (i + randomIdx) % airports.size();
//            qDebug() << targetIdx;

            Airplane* plane = new Airplane(airports[i]->pos(), airports[targetIdx]->pos());
            connect(plane, SIGNAL(finished(QString,bool)),
                    this, SLOT(planeFinished(QString,bool)));
            QString s = "Flight-" + QString::number(plane->flightNo) + " : " + airports[i]->getName()
                    + " --> " + airports[targetIdx]->getName();
            emit flightInfo(s);
            airports[targetIdx]->planes.push_back(plane);
            scene->addItem(plane);
    }

    std::random_shuffle(airports.begin(), airports.end());

}

void Controller::planeFinished(const QString &s, bool crashed)
{
    emit landingInfo(s, crashed);
}
