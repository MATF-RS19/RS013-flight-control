#include "controller.h"

#include <random>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Controller::Controller(int width, int height)
{
    // Create the scene
    scene = new QGraphicsScene(this);
    setSceneRect(0, 0, width, height);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);


    setFixedSize(width, height);
//    fitInView(sceneRect(), Qt::KeepAspectRatio);
    setScene(scene);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    focused_plane = nullptr;

    // Call update() every 50 miliseconds
    static QTimer t;
    connect(&t, SIGNAL(timeout()), this, SLOT(update()));
    t.start(15000);

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

        QPointF tmp = mapToScene(pos);

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
        Airplane* plane = new Airplane(mapToScene(event->pos()), closest->pos(), Airplane::fuelCap);
        closest->planes.push_back(plane);
        scene->addItem(plane);
//        qDebug() <<mapToScene(event->pos());
    } else if(event->button() == Qt::LeftButton) {
        for(Airport* airport : airports) {
            foreach (Airplane* plane, airport->planes) {
    //            qDebug() << plane->boundingRect();
                if(plane->sceneBoundingRect().contains((mapToScene(event->pos())))){

                    if(focused_plane) focused_plane->setState(State::FLYING);

                    focused_plane = plane;
                    focused_plane->setState(State::MANUAL);
                    qDebug() << "Assumed manual control of plane " << focused_plane->flightNo;
                    break;
                }
            }
        }
    } else if(event->button() == Qt::MiddleButton){
        // Store original position.
        originX = event->x();
        originY = event->y();
    }
}

void Controller::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::MidButton){
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
        focused_plane->steer(-0.025);

    }else if(event->key() == Qt::Key_Right){

        if(!focused_plane){
            qDebug() << "No plane is focused";
            return;
        }
        focused_plane->steer(0.025);

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


        // Manual control is intended for dangerous situations
        // If a plane is running low on fuel even though its target is too far it should find nearest airport to land

        auto closest = findClosestAirport(focused_plane->pos());

        // Move this plane from previous target airport to closest airport
        auto targetPos = focused_plane->getTarget();
        auto currentTargetIt = std::find_if(airports.begin(), airports.end(),
                                              [targetPos](Airport* a){return a->pos() == targetPos;});
        Airport* currentTarget = *currentTargetIt;
        currentTarget->planes.erase(std::remove(currentTarget->planes.begin(), currentTarget->planes.end(), focused_plane));

        closest->planes.push_back(focused_plane);

        focused_plane->setTarget(closest->pos());

        if(closest->currentPlane && (closest->currentPlane->getState() == State::LANDING)){
            qDebug() << "Another plane currently landing";
            return;
        }
        closest->currentPlane = focused_plane;
        focused_plane->setState(State::FLYING);
        focused_plane = nullptr;
    }
}

void Controller::wheelEvent(QWheelEvent *event)
{
    if(event->delta() > 0){
        scaleCounter++;
        scale(1.25, 1.25);
    }else{
        if(scaleCounter > 0) {
            scaleCounter--;
            scale(0.8, 0.8);
        }
    }
}

void Controller::update()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<int> randomDiscrete(1, airports.size()-1);

    for(int i = 0; i < airports.size(); i++) {

            int randomIdx = randomDiscrete(gen);
//            qDebug() << randomIdx;
            int targetIdx = (i + randomIdx) % airports.size();
//            qDebug() << targetIdx;
            Airplane* plane = new Airplane(airports[i]->pos(), airports[targetIdx]->pos(), Airplane::fuelCap);
            airports[targetIdx]->planes.push_back(plane);
            scene->addItem(plane);
    }

}
