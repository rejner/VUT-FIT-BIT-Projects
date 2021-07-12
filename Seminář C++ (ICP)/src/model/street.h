/*!
 * @file
 * @brief This file contains class Street declaration
 *
 * @author Michal Rein, Ondřej Motyčka
*/

#ifndef STREET_H
#define STREET_H


#include <QtGlobal>
#include <QList>
#include <QLine>
#include <QDebug>
#include "model/coordinate.h"
#include "model/stop.h"
#include <QGraphicsLineItem>

/**
 * @brief The Street class represents one unique street in the map simulated environment. It holds information about its id, level of
 * congestion and stops that are present on the street.
 */
class Street : public QLine
{
    private:
        /**
         * @brief id Unique identifier.
         */
        QString id;
        /**
         * @brief begin Begin coordinate.
         * @brief end End coordinate.
         */
        Coordinate *begin, *end;
        /**
         * @brief stopList List of stops that are present on the street.
         */
        QList<Stop *> *stopList = new QList<Stop *>;
        /**
         * @brief congestionLevel Level of congestion.
         */
        int congestionLevel = 0;
        /**
         * @brief lineHandle Handle for QGraphicsLineItem that represents street's line in the scene.
         */
        QGraphicsLineItem* lineHandle;
        /**
         * @brief textHandle Handle for QGraphicsTextItem that represents street's name in the scene.
         */
        QGraphicsTextItem* textHandle;

    public:

        /*CONSTRUCTOR*/
        static Street* createStreet(QString *id, qint8 clevel, Coordinate *begin, Coordinate *end);

        /*UTILS*/
        bool follows(Street *s);
        bool addStop(Stop *stop);

        /*GETTERS*/
        QString* getId();
        Coordinate* getBegin();
        Coordinate* getEnd();
        QList<Stop*>* getStopList();
        int* getCongestion();
        QGraphicsLineItem* getLineHandle();
        QGraphicsTextItem* getTextHandle();

        /*SETTERS*/
        bool setCongestionLevel(qint8 level);
        void setLineHandle(QGraphicsLineItem* handle);
        void setTextHandle(QGraphicsTextItem* handle);


};

#endif // STREET_H


