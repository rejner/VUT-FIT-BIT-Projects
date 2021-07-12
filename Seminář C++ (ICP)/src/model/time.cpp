/*!
 * @file
 * @brief This file contains implementation of Time class
 *
 * @author Michal Rein, Ondřej Motyčka
*/

#include "time.h"

/**
 * @brief Time::Time Constructor of Time object.
 */
Time::Time()
{
    this->appTime = new QTime(0, 0, 0, 0);
}

/**
 * @brief Time::getAppTime Get actual QTime object.
 * @return QTime object of simulated time.
 */
QTime *Time::getAppTime()
{
    return this->appTime;
}

/**
 * @brief Time::getAppTimeInSeconds Extract iformation about simulated time and convert it into seconds.
 * @return Actual time in seconds.
 */
qint16 Time::getAppTimeInSeconds(){

    qint16 time = 0;
    time += this->appTime->hour() * 3600;
    time += this->appTime->minute() * 60;
    time += this->appTime->second();
    return time;
}

/**
 * @brief Time::updateTime Update simulated time, 1 second passed.
 */
void Time::updateTime()
{
    *this->appTime = appTime->addSecs(1);
}

/**
 * @brief Time::convertTimeToString Convert actual time to QString human readable format.
 * @param time Actual time in seconds.
 * @return Time string.
 */
QString Time::convertTimeToString(qint16 *time)
{
    qint16 hours = *time/3600;
    qint16 minutes = (*time/60) % 60;
    qint16 secs = *time % 60;
    QString timeString = QString("%1").arg(hours, 2, 10, QChar('0')) + ":" + QString("%1").arg(minutes, 2, 10, QChar('0')) + ":" + QString("%1").arg(secs, 2, 10, QChar('0'));
    return timeString;

}

/**
 * @brief Time::convertStringToTime Convert time string in HH:MM:SS format to seconds.
 * @param timeString Time string.
 * @return Time representing given time string in seconds.
 */
qint16 Time::convertStringToTime(QString *timeString)
{
    qint16 time = 0;
    QStringList parts = timeString->split(":");
    time += parts.value(0).toInt(nullptr, 10) * 3600;
    time += parts.value(1).toInt(nullptr, 10) * 60;
    time += parts.value(2).toInt(nullptr, 10);
    return time;
}

/**
 * @brief Time::getCongestionConstant Read congestion level of given street and return delay constant for correct timetable construction.
 * @param street Street object.
 * @return Congestion delay constant.
 */
int Time::getCongestionConstant(Street *street)
{
    int constant;
    switch (*street->getCongestion()) {
        case 1:
            constant = 2;
            break;
        case 2:
            constant = 4;
            break;
        case 3:
            constant = 10;
            break;
        default:
            constant = 1;
            break;
    }
    return constant;

}

