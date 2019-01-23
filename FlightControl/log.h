#ifndef LOG_H
#define LOG_H

#include "controller.h"
#include <QFrame>
#include <QLabel>
#include <QTextBrowser>
#include <QPushButton>

class Controller;

class Log : public QFrame
{
    Q_OBJECT

public:
    Log(Controller *controller);

    QTextEdit *txtEdit;

signals:
    void isClicked();

public Q_SLOTS:
    void appendLandInfo(const QString& text, bool landed);
    void appendTakeOffInfo(const QString& text);
    void info(const QString& text);
    void checkIfClicked();

private:
    QLabel *takeOff;
    QLabel *land;

    QTextBrowser *takeOffInfo;
    QTextBrowser *landInfo;

    QLabel *lblPlaneInfo;
    QTextBrowser *tbPlaneInfo;

    QLabel *lblAirportName;

    QPushButton *btnSave;
};

#endif // LOG_H
