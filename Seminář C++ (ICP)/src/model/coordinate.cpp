/*!
 * @file
 * @brief This file contains implementation of class Coordinate
 *
 * @author Michal Rein, Ondřej Motyčka
 */

#include "coordinate.h"

/**
 * @brief Coordinate::createCoordinate Creator of Coordinate class. Check for valid coordinate is performed (can't have negative values).
 * @param x X value.
 * @param y Y value.
 * @return Valid Coordinate object.
 */
Coordinate* Coordinate::createCoordinate(qint16 x, qint16 y)
{
    Coordinate *c = new Coordinate();
    if (x >= 0 && y >= 0){
        c->setCoordinates(x, y);
        return c;
    }
    return nullptr;
}

/**
 * @brief Coordinate::setCoordinates Set X and Y values to Coordinate object.
 * @param x X value.
 * @param y Y value.
 */
void Coordinate::setCoordinates(qint16 x, qint16 y){
    this->x = x;
    this->y = y;
}

/**
 * @brief Coordinate::getX Get X value of Coordinate object.
 * @return X value.
 */
qint16 Coordinate::getX(){
    return this->x;
}

/**
 * @brief Coordinate::getY Get Y value of Coordinate object.
 * @return Y value.
 */
qint16 Coordinate::getY(){
    return this->y;
}

/**
 * @brief Coordinate::diffX Calculate X value difference between this and given Coordinate objects.
 * @param c Coordinate to perform difference operation with.
 * @return Calculated difference of X values of two Coordinate objects.
 */
qint16 Coordinate::diffX(Coordinate *c){
    return this->x - c->getX();
}

/**
 * @brief Coordinate::diffY Calculate Y value difference between current and given Coordinate objects.
 * @param c Coordinate to perform difference operation with.
 * @return Calculated difference of Y values of two Coordinate objects.
 */
qint16 Coordinate::diffY(Coordinate *c){
    return this->y - c->getY();
}

/**
 * @brief Coordinate::Compare Compare this and given Coordinate objects for X and Y values.
 * @param c Coordinate to perform comparison operation with.
 * @return Boolean value true when both X and Y values match, false othervise.
 */
bool Coordinate::Compare(Coordinate *c){
    return this->x == c->getX() && this->y == c->getY();
}
