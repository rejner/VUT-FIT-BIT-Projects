/*!
 * @file
 * @brief This file contains class Coordinate declaration
 *
 * @author Michal Rein, Ondřej Motyčka
 */

#ifndef COORDINATE_H
#define COORDINATE_H

#include <QtGlobal>

/**
 * @brief The Coordinate class is representation of real coordinate containing X and Y values.
 * It is used to describe position of certain entities (stops, beginnings of streets etc...).
 */
class Coordinate
{
    private:
        /**
         * @brief x X value.
         */
        qint16 x;
        /**
         * @brief y Y value.
         */
        qint16 y;

    public:
        static Coordinate* createCoordinate(qint16 x, qint16 y);
        void setCoordinates(qint16 x, qint16 y);
        bool Compare(Coordinate *c);
        qint16 getX();
        qint16 getY();
        qint16 diffX(Coordinate *c);
        qint16 diffY(Coordinate *c);
};


#endif // COORDINATE_H
