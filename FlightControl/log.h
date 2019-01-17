#ifndef LOG_H
#define LOG_H

#include "controller.h"
#include <QFrame>
#include <QTextBrowser>

class Log : public QFrame
{
    Q_OBJECT

public:
    Log(Controller *controller);

public Q_SLOTS:
    void appendText(const QString& text, bool landed);

private:
    QTextBrowser *takeOffInfo;
    QTextBrowser *landInfo;
};

#endif // LOG_H
