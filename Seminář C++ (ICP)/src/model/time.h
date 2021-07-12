/*!
 * @file
 * @brief This file contains Time class declaration
 *
 * @author Michal Rein, Ondřej Motyčka
*/

#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <QString>
#include <QTime>
#include "model/street.h"

/**
 * @brief The Time class holds information about simulated application time.
 * It also provides useful methods for time format conversion of simulated time and constructing time tables.
 */
class Time
{
private:

    /**
     * @brief appTime QTime object that holds information about simulated time.
     */
    QTime *appTime;

public:
    Time();

    /* GETTERS */
    QTime* getAppTime();
    qint16 getAppTimeInSeconds();

    /* SETTERS */
    void updateTime();

    /* UTILS */
    static QString convertTimeToString(qint16 *time);
    static qint16 convertStringToTime(QString *timeString);
    static int getCongestionConstant(Street *street);

};

#endif // TIME_UTILS_H
