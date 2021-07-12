/*!
 * @file
 * @brief This file contains implementation of class Street
 *
 * @author Michal Rein, Ondřej Motyčka
 */

#include "street.h"

/**
 * @brief Street::createStreet  New Street creator. Check for coordinates validity is performed, Street object is returned on success.
 * @param id Name of the street.
 * @param clevel Congestion level.
 * @param begin Street's begin coordinate.
 * @param end Street's end coordinate.
 * @return New valid Street object.
 */
Street* Street::createStreet(QString *id, qint8 clevel, Coordinate *begin, Coordinate *end){

    Street *str = new Street();

    if ((begin->getX() != end->getX()) && (begin->getY() != end->getY())){
        str = nullptr;
        return str;
    }

    str->id = *id;
    str->congestionLevel = clevel;
    str->begin = begin;
    str->end = end;
    str->setLine(begin->getX(), begin->getY(), end->getX(), end->getY());

    return str;
}

/**
 * @brief Street::follows Check if given street follows this street.
 * @param s Street object to check follow with.
 * @return True or False boolean value, depending on whether streets follow each other or not.
 */
bool Street::follows(Street *s){
    return ((s->getEnd()->Compare(this->end) || s->getEnd()->Compare(this->begin) ||
             (s->getBegin()->Compare(this->begin) || s->getBegin()->Compare(this->end)))) ?
                true : false;
}

/**
 * @brief Street::addStop Add stop to the street's stop list, also check if position of a stop is valid.
 * @param stop Stop object that should be added to the street's stop list.
 * @return Result of operation. If operation cannot be performed, return false.
 */
bool Street::addStop(Stop *stop){

    // if X coordinates of stop and begin of a street are different
    if (stop->getCoordinate()->diffX(this->begin) != 0){

        // Y for both must be the same, otherwise stop isn't located on a street
        if (stop->getCoordinate()->diffY(this->begin) == 0){

            if (((begin->getX() <= stop->getCoordinate()->getX()) &&
                (stop->getCoordinate()->getX() <= end->getX())) ||
                 ((begin->getX() >= stop->getCoordinate()->getX()) && (stop->getCoordinate()->getX() >= end->getX()))){
                this->stopList->append(stop);
                stop->setStreet(this);
                return true;
            }

        }

    // if Y coordinates of stop and begin of a street are different
    } else if (stop->getCoordinate()->diffY(this->begin) != 0){

        // X for both must be the same, otherwise stop isn't located on a street
        if (stop->getCoordinate()->diffX(this->begin) == 0){

            if (((begin->getY() <= stop->getCoordinate()->getY()) &&
                (stop->getCoordinate()->getY() <= end->getY())) ||
                 ((begin->getY() >= stop->getCoordinate()->getY()) && (stop->getCoordinate()->getY() >= end->getY()))){
                this->stopList->append(stop);
                stop->setStreet(this);
                return true;
            }

        }
    }
    else if (stop->getCoordinate()->Compare(this->begin) || stop->getCoordinate()->Compare(this->end)){
        this->stopList->append(stop);
        stop->setStreet(this);
        return true;
    }

    return false;
}

/**
 * @brief Street::getId Get street identifier.
 * @return Street identifier (name).
 */
QString *Street::getId()
{
    return &this->id;
}

/**
 * @brief Street::getBegin Get begin coordinate of a street.
 * @return Begin Coordinate object.
 */
Coordinate *Street::getBegin()
{
    return this->begin;
}

/**
 * @brief Street::getEnd Get end coordinate of a street.
 * @return End Coordinate object.
 */
Coordinate *Street::getEnd()
{
    return this->end;
}

/**
 * @brief Street::getStopList Get List of stops associated with street.
 * @return List of stops.
 */
QList<Stop*> *Street::getStopList()
{
    return this->stopList;
}

/**
 * @brief Street::getCongestion Get level of congestion.
 * @return Level of congestion.
 */
int *Street::getCongestion()
{
    return &this->congestionLevel;
}

/**
 * @brief Street::getLineHandle Get handle of QGraphicsLineItem that represents street's line in the scene.
 * @return QGraphicsLineItem handle.
 */
QGraphicsLineItem *Street::getLineHandle()
{
    return this->lineHandle;
}

/**
 * @brief Street::getTextHandle Get handle of QGraphicsTextItem that represents street's name in the scene.
 * @return  QGraphicsTextItem handle.
 */
QGraphicsTextItem *Street::getTextHandle()
{
    return this->textHandle;
}

/**
 * @brief Street::setCongestionLevel Set congestion level of this Street object.
 * @param level Congestion level.
 * @return Result of operation. Returns false when level is out of possible range.
 */
bool Street::setCongestionLevel(qint8 level)
{
    if (0 <= level && 4 >= level){
        this->congestionLevel = level;
        return true;
    }

    return false;
}

/**
 * @brief Street::setLineHandle Set QGraphicsLineItem handle of this object.
 * @param handle QGraphicsLineItem object created by Scene object.
 */
void Street::setLineHandle(QGraphicsLineItem *handle)
{
    this->lineHandle = handle;
}

/**
 * @brief Street::setTextHandle Set QGraphicsTextItem handle of this object.
 * @param handle QGraphicsTextItem object created by Scene object.
 */
void Street::setTextHandle(QGraphicsTextItem *handle)
{
    this->textHandle = handle;
}


