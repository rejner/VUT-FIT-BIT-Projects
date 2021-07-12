/*!
 * @file
 * @brief This file contains StreetMap class declaration
 *
 * @author Michal Rein, Ondřej Motyčka
*/

#ifndef STREETMAP_H
#define STREETMAP_H

#include "model/coordinate.h"
#include "model/street.h"
#include "model/stop.h"
#include "model/transportline.h"
#include <QDebug>
#include <QDir>
#include "QJsonDocument"
#include "QJsonObject"
#include "QJsonArray"

/**
 * @brief The StreetMap class is a representation of actual map. It holds information about all stops, streets and transport lines.
 * Layout of streets and stops is loaded from JSON file in specific format, including information about transport lines.
 */
class StreetMap
{
    private:
        /**
         * @brief streetList List of Street objects.
         */
        QList<Street *> *streetList = new QList<Street*>;
        /**
         * @brief stopList List of Stop objects.
         */
        QList<Stop *> *stopList = new QList<Stop*>;
        /**
         * @brief transportLineList List of TransportLine objects.
         */
        QList<TransportLine *> *transportLineList = new QList<TransportLine*>;

    public:

        /*CONSTRUCTOR*/
        StreetMap();

        /*MAP CREATOR*/
        void createReferenceMap(QString path);

        /*GETTERS*/
        QList<Street *>* getStreetList();
        QList<Stop *>* getStopList();
        QList<TransportLine *>* getTransportLineList();

        /*SEARCH METHODS*/
        Street* findStreet(QGraphicsLineItem *line);
        Stop* findStop(QGraphicsRectItem *mark);
        Vehicle* findVehicle(QGraphicsEllipseItem *mark);
        TransportLine* findTransportLine(Vehicle *vehicle);
};

#endif // STREETMAP_H
