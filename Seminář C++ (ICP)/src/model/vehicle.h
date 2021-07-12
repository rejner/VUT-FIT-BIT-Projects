/*!
 * @file
 * @brief This file contains class that represents vehicle
 *
 * @author Michal Rein, Ondřej Motyčka
 */
#ifndef VEHICLE_H
#define VEHICLE_H

#include <QtGlobal>
#include <QPoint>
#include "stop.h"
#include "street.h"
#include "time.h"
#include <QPen>


/**
 * @brief This class represents Vehicle class
 */
class Vehicle : public QRect
{
    private:
         /**
         * @brief idLine Number of Line
         * @brief idNumber Special number of Vehicle
         */
        QString idLine, idNumber;
        /**
         * @brief dx Variable used for movement on x axis
         * @brief dy Variable used for movement on y axis
         */
        qreal dx, dy;
        /**
         * @brief lastStop Last stop
         * @brief nextStop Next stop
         * @brief startStop Start stop
         * @brief endStop End stop
         */
        Stop *lastStop, *nextStop, *startStop, *endStop;
        /**
         * @brief currentStopIndex Current stop index
         * @brief timeTableIndex Time table index
         */
        qint8 currentStopIndex = 1, timeTableIndex = 1;
        /**
         * @brief timeout Timeout
         */
        qint8 timeout = 0;
        /**
         * @brief deployTime Deploy time of vehicle
         */
        qint16 deployTime;
        /**
         * @brief route Route of vehicle
         */
        QList<QMap<Street *, Stop *> *> *route;
        /**
         * @brief routeEdit Edited route of vehicle
         */
        QList<QMap<Street *, Stop *> *> *routeEdit = nullptr;
        /**
         * @brief routeDefault Default route of vehicle
         */
        QList<QMap<Street *, Stop *> *> *routeDefault;
        /**
         * @brief timetable Timetable of vehicle
         */
        QList<QMap<QString, qint16> *> *timetable = nullptr;
        /**
         * @brief timeHandle Global timer
         */
        Time* timeHandle;
        /**
         * @brief deployed Says if vehicle has been deployed
         */
        bool deployed = false;


    public:
        qreal x, y;
        bool displayed = false, reverse = false, resetEditRoute = false;
        QGraphicsEllipseItem *vehicleMark;
        QGraphicsTextItem *vehicleText;

        Vehicle(QString *idLine, QString *idNumber, QList<QMap<Street *, Stop *> *> *route, qint16 deploy);

        /* GETTERS */
        qreal getX();
        qreal getY();
        qreal getDx();
        qreal getDy();
        QString getidNumber();
        QString getLineNumber();
        Stop* getlastStop();
        Stop* getnextStop();
        qint16 getTimetableIndex();
        qint16 getCurrentStopIndex();
        QString getFullSignature();
        QList<QMap<Street *, Stop *> *>* getRoute();
        QList<QMap<QString, qint16> *>* getTimetable();

        /* SETTERS */
        void setDx(qreal val);
        void setDy(qreal val);
        void setTimeHandle(Time* timeObj);
        void setRoute(QList<QMap<Street *, Stop *> *> *route, bool isEdit);

        /* CALCULATIONS */
        void calculateTrajectory();
        void waitOnStop();
        void calculateDirection();
        void createTimetable();

        /* CHECKERS */
        bool isDeployed();
        bool checkDeployTime();

};

#endif // VEHICLE_H
