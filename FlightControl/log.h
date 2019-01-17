#ifndef LOG_H
#define LOG_H

#include "controller.h"
#include <QFrame>
#include <QLabel>
#include <QTextBrowser>

class Log : public QFrame
{
    Q_OBJECT

public:
    Log(Controller *controller);

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
};

#endif // LOG_H
