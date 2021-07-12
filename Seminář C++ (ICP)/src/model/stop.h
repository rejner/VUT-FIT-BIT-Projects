/*!
 * @file
 * @brief This file contains class Stop declaration
 *
 * @author Michal Rein, Ondřej Motyčka
 */

#ifndef STOP_H
#define STOP_H

#include "model/coordinate.h"
#include <QRect>
#include <QtGlobal>
#include <QDebug>
#include <QGraphicsRectItem>

// forward declaration of class Street
class Street;

/**
 * @brief The Stop class represents one unique stop on the map.
 */
class Stop : public QRect
{
    private:
        /**
         * @brief id Identifier.
         */
        QString id;
        /**
         * @brief pos Position represented by Coordinate class.
         */
        Coordinate *pos;
        /**
         * @brief str Street object associated with a stop (will be set by a Street object when assigned to it).
         */
        Street *str;
        /**
         * @brief isAuxStop Indicator whether is stop just an auxiliary or not.
         */
        bool isAuxStop;

    public:

        QGraphicsRectItem* stopMarkHandle;

        /*CONSTRUCTOR*/
        Stop(QString *id, Coordinate *c, bool aux);
        ~Stop();

        /*GETTERS*/
        QString* getId();
        Coordinate* getCoordinate();
        bool getIsAux();
        Street* getStreet();

        /*SETTERS*/
        void setStreet(Street *str);
};



#endif // STOP_H


