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
    t.start(50);

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

        scene->addItem(airports[i]);
    }
    show();
}

static double distanceSquared(QPointF p, QPointF q)
{
    return (p.x() - q.x()) * (p.x() - q.x()) + (p.y() - q.y()) * (p.y() - q.y());
}

void Controller::mousePressEvent(QMouseEvent *event)
{
    // Spawn a new airplane on the clicked location (this is for testing purposes)
    if(event->button() == Qt::RightButton) {
        // This airplane's target is the closest airport
        auto closest = std::min_element(airports.cbegin(), airports.cend(),
                                        [event](Airport* a1, Airport* a2)
                                        {return distanceSquared(a1->pos(), event->pos()) <
                                                distanceSquared(a2->pos(), event->pos());});
        Airplane* plane = new Airplane(mapToScene(event->pos()), (*closest)->pos(), Airplane::fuelCap);
        (*closest)->planes.push_back(plane);
        scene->addItem(plane);
        qDebug() << event->pos();
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
        if(airports[0]->currentPlane && (airports[0]->currentPlane->getState() == State::LANDING
                                         || airports[0]->currentPlane->getState() == State::REFUELING)){
            qDebug() << "Another plane currently landing";
            return;
        }
        airports[0]->currentPlane = focused_plane;
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
    // trebalo bi da je nepotrebno jer je QPointer
    if(focused_plane){
        if(focused_plane->getState() == State::CRASHED){
            focused_plane = nullptr;
        }
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<double> random(0, 1);

    std::uniform_int_distribution<int> randomDiscrete(0, airports.size());

    static int max_planes = 1;

    for(int i = 0; i < airports.size(); i++) {
        if(random(gen) > 0.95 && airports[i]->planes.size() < max_planes){
            double a = random(gen) * 2 * M_PI;
            double r = 500 + random(gen) * 200;
            QPointF pos;
            pos.setX(r * cos(a) + 300);
            pos.setY(r * sin(a) + 300);



            Airplane* plane = new Airplane(pos, airports[i]->pos(), Airplane::fuelCap);
            airports[i]->planes.push_back(plane);
            scene->addItem(plane);
        }
    }

//    samo prolazi...
//    if(random(gen) > 0.998){
//        double a = random(gen) * 2 * M_PI;
//        double r = 600;
//        QPoint pos, tar;
//        pos.setX(r * cos(a) + 300);
//        pos.setY(r * sin(a) + 300);

//        a = random(gen) * 2 * M_PI;
//        tar.setX(r * cos(a) + 300);
//        tar.setY(r * sin(a) + 300);

//        Airplane* plane = new Airplane(pos, tar, Airplane::fuelCap);
//        scene->addItem(plane);
//    }
}
