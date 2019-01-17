#include "log.h"

#include <QLabel>
#include <QHBoxLayout>

Log::Log(Controller *controller)
    : QFrame()
{
    setFixedSize(600, 400);
    move(800, 100);
    setWindowTitle("Log");

    auto* layout = new QHBoxLayout();
    setLayout(layout);

    //TODO: naslovi za ova dva
    takeOffInfo = new QTextBrowser(this);
    landInfo = new QTextBrowser(this);

    takeOffInfo->setWindowTitle("take off");


    layout->addWidget(takeOffInfo);
    layout->addWidget(landInfo);


    connect(controller, SIGNAL(flightInfo(QString)),
            takeOffInfo, SLOT(append(QString)));

    connect(controller, SIGNAL(landingInfo(const QString&, bool)),
            this, SLOT(appendText(const QString&, bool)));
}

void Log::appendText(const QString &text, bool crashed)
{
    if(crashed) {
        landInfo->setTextColor(Qt::red);
    } else {
        landInfo->setTextColor(Qt::green);
    }
    landInfo->append(text);
}
