#include "controller.h"

#include <random>

Controller::Controller(int width, int height)
{
    // Create the scene
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0,0,width, height);
    setScene(scene);
    fitInView(0,0, width, height, Qt::KeepAspectRatio);
    setFixedSize(width, height);

    focused_plane = nullptr;

    // Call update() every 50 miliseconds
    static QTimer t;
    connect(&t, SIGNAL(timeout()), this, SLOT(update()));
    t.start(50);
}

void Controller::run()
{
    // Create an airport and add it to scene
    airport = new Airport();
    airport->setPos(300,300);

    scene->addItem(airport);

    show();
}

void Controller::mousePressEvent(QMouseEvent *event)
{
    // Spawn a new airplane on the clicked location (this is for testing purposes)
    if(event->button() == Qt::RightButton){
        Airplane* plane = new Airplane(mapToScene(event->pos()), airport->pos(), Airplane::fuelCap);
        airport->planes.push_back(plane);
        scene->addItem(plane);
    }else if(event->button() == Qt::LeftButton){
        foreach (Airplane* plane, airport->planes) {
            qDebug() << plane->boundingRect();
            if(plane->sceneBoundingRect().contains((mapToScene(event->pos())))){

                if(focused_plane) focused_plane->setState(State::FLYING);

                focused_plane = plane;
                focused_plane->setState(State::MANUAL);
                qDebug() << "Assumed manual control of plane " << focused_plane->flightNo;
                break;
            }
        }
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
        if(airport->currentPlane && (airport->currentPlane->getState() == State::LANDING || airport->currentPlane->getState() == State::REFUELING)){
            qDebug() << "Another plane currently landing";
            return;
        }
        airport->currentPlane = focused_plane;
        focused_plane->setState(State::FLYING);
        focused_plane = nullptr;
    }
}

void Controller::update()
{
    if(focused_plane){
        if(focused_plane->getState() == State::CRASHED){
            focused_plane = nullptr;
        }
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<double> random(0, 1);

    static int max_planes = 10;

    if(random(gen) > 0.95 && airport->planes.size() < max_planes){
        double a = random(gen) * 2 * M_PI;
        double r = 500 + random(gen) * 200;
        QPoint pos;
        pos.setX(r * cos(a) + 300);
        pos.setY(r * sin(a) + 300);

        Airplane* plane = new Airplane(pos, airport->pos(), Airplane::fuelCap);
        airport->planes.push_back(plane);
        scene->addItem(plane);
    }


    if(random(gen) > 0.998){
        double a = random(gen) * 2 * M_PI;
        double r = 600;
        QPoint pos, tar;
        pos.setX(r * cos(a) + 300);
        pos.setY(r * sin(a) + 300);

        a = random(gen) * 2 * M_PI;
        tar.setX(r * cos(a) + 300);
        tar.setY(r * sin(a) + 300);

        Airplane* plane = new Airplane(pos, tar, Airplane::fuelCap);
        scene->addItem(plane);
    }
}
