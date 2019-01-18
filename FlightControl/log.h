#ifndef LOG_H
#define LOG_H

#include "controller.h"
#include <QFrame>
#include <QLabel>
#include <QTextBrowser>

class Controller;

class Log : public QFrame
{
    Q_OBJECT

public:
    Log(Controller *controller);

    QTextEdit *txtEdit;

public Q_SLOTS:
    void appendText(const QString& text, bool landed);
    void info(const QString& text);

private:
    QLabel *takeOff;
    QLabel *land;

    QTextBrowser *takeOffInfo;
    QTextBrowser *landInfo;

    QLabel *lblPlaneInfo;
    QTextBrowser *tbPlaneInfo;

    QLabel *lblAirportName;
};

#endif // LOG_H
