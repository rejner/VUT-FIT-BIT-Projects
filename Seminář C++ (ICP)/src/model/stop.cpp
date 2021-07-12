/*!
 * @file
 * @brief This file contains implementation of class Stop
 *
 * @author Michal Rein, Ondřej Motyčka
 */

#include "stop.h"

/**
 * @brief Stop::Stop Constructor of a Stop object.
 * @param id Identifier (name) of a stop.
 * @param c Position of a stop.
 * @param aux Identifier whether this stop is an auxiliary object.
 */
Stop::Stop(QString *id, Coordinate *c, bool aux)
{
    this->id = *id;
    this->pos = c;
    this->isAuxStop = aux;
    this->setRect(pos->getX()-7.5, pos->getY()-7.5, 15, 15);
}

/**
 * @brief Stop::getId Get stop identifier (name).
 * @return Stop identifier.
 */
QString* Stop::getId(){
    return &id;
}

/**
 * @brief Stop::getCoordinate Get Coordinate object (position).
 * @return Stop's position on the map.
 */
Coordinate* Stop::getCoordinate(){
    return pos;
}

/**
 * @brief Stop::getIsAux Get auxiliary stop indicator.
 * @return Boolean value whether stop is an auxiliary or not.
 */
bool Stop::getIsAux(){
    return isAuxStop;
}

/**
 * @brief Stop::getStreet Get Street object associated with a stop (stop is located on that street).
 * @return Street object where stop is located.
 */
Street *Stop::getStreet()
{
    return this->str;
}

/**
 * @brief Stop::setStreet Set Street object on which stop is located.
 * @param str Street object.
 */
void Stop::setStreet(Street *str)
{
    this->str = str;
}

