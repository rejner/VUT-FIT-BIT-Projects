/*!
 * @file
 * @brief This file contains TransportLine class declaration
 *
 * @author Michal Rein, Ondřej Motyčka
*/

#ifndef TRANSPORTLINE_H
#define TRANSPORTLINE_H

#include <QtGlobal>
#include <QMap>
#include "stop.h"
#include "street.h"
#include "vehicle.h"

/**
 * @brief The TransportLine class represents one unique transport line. It holds information about it's id, stops, streets and vehicles.
 * Instance of this class can be passed to multiple vehicles, which then share the same route, line number but varies
 * in their unique vehicle number id and position based on time of their creation.
 */
class TransportLine
{
    private:
        /**
         * @brief id Unique identifier.
         */
        QString id;
        /**
         * @brief stopList List of Stop objects.
         */
        QList<Stop *> *stopList = new QList<Stop *>;
        /**
         * @brief vehicleList List of Vehicle objects.
         */
        QList<Vehicle *> *vehicleList = new QList<Vehicle *>;

    public:
        static TransportLine* createTransportLine(QString *id);
        static QList<QMap<Street *, Stop *> *> *createRoute(QList<Stop *> *stopLis, QList<Street *> *streetList);

        QList<QMap<Street *, Stop *> *> *route;
        QList<QMap<Street *, Stop *> *> *routeEdit;


        QList<Street *> *streetList = new QList<Street *>;
        bool addStop(Stop *stop);
        bool addVehicle(QString *idLine, QString *idNumber, qint16 deploy);
        QString* getId();
        QList<Vehicle *>* getVehicleList();
        QList<QMap<Street *, Stop *> *> * getRoute();

        void setEditRoute();
        void resetEditRoute();

        QList<Stop *> *getStopList();
        QList<Street *> *getStreetList();


};

#endif // TRANSPORTLINE_H
