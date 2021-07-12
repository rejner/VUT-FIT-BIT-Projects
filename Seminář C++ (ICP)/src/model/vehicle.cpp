/*!
 * @file
 * @brief This file contains implementation of vehicle
 *
 * @author Michal Rein, Ondřej Motyčka
 */

#include "vehicle.h"

/**
 * @brief Constructor of class Vehicle
 * @param idLine ID of line
 * @param idNumber ID of vehicle
 * @param route Route
 * @param deploy Deploy time
 */
Vehicle::Vehicle(QString *idLine, QString *idNumber, QList<QMap<Street *, Stop *> *> *route, qint16 deploy)
{
    this->idLine = *idLine;
    this->idNumber = *idNumber;
    this->route = route;
    this->routeDefault = route;
    this->deployTime = deploy;

    this->lastStop = this->route->front()->begin().value();
    this->nextStop = this->route->value(1)->begin().value();
    this->startStop = this->route->begin().i->t()->begin().value();
    this->endStop = this->route->back()->begin().value();

    this->x = startStop->getCoordinate()->getX();
    this->y = startStop->getCoordinate()->getY();

    this->setRect(this->x-15, this->y-15, 30, 30);

    calculateDirection();
}

/**
 * @brief Method that returns full vehicle signature
 * @return Vehicle signature
 */
QString Vehicle::getFullSignature()
{
    QString signature;
    signature.append(this->idLine);
    signature.append(" (");
    signature.append(this->idNumber);
    signature.append(")");
    return signature;
}

/**
 * @brief Returns Dx
 * @return Dx
 */
qreal Vehicle::getDx()
{
    return this->dx;
}

/**
 * @brief Returns Dy
 * @return Dy
 */
qreal Vehicle::getDy()
{
    return this->dy;
}


/**
 * @brief Sets Dx
 * @param val Value of axis X
 */
void Vehicle::setDx(qreal val)
{
    if (val == 0){
        dx = val;
        return;
    }

    qreal modifiedValue = val;
    int congestionLevel = 0;

    Stop *tmpStop1;
    Street *tmpStr1, *tmpStr2;

    // get legit congestion level
    // if vehicle is not passing in reversal order
    if (!reverse){

        tmpStop1 = route->value(currentStopIndex-1)->begin().value();


        tmpStr1 = route->value(currentStopIndex-1)->begin().value()->getStreet();
        tmpStr2 = route->value(currentStopIndex)->begin().value()->getStreet();

        // if both stops are on different streets
        if (!(tmpStr1 == tmpStr2)){

            // and vehicle is not on stop's coordinates yet
            if (!(tmpStop1->getCoordinate()->getX() == x && tmpStop1->getCoordinate()->getY() == y)){

                // vehicle is on the border of streets, congestion level is picked from the next stop's street
                congestionLevel = *tmpStr2->getCongestion();

            } else {
                congestionLevel = *tmpStr1->getCongestion();
            }

        // stops are on the same street, congestion level is the same for both options
        } else {
            congestionLevel = *tmpStr1->getCongestion();
        }

    // vehicle is passing in reversal order
    } else {

        tmpStop1 = route->value(currentStopIndex+1)->begin().value();


        tmpStr1 = route->value(currentStopIndex+1)->begin().value()->getStreet();
        tmpStr2 = route->value(currentStopIndex)->begin().value()->getStreet();

        // the same as in not reversal order
        if (!(tmpStr1 == tmpStr2)){

            if (!(tmpStop1->getCoordinate()->getX() == x && tmpStop1->getCoordinate()->getY() == y)){
                congestionLevel = *tmpStr2->getCongestion();
            } else {
                congestionLevel = *tmpStr1->getCongestion();
            }

        } else {
            congestionLevel = *tmpStr1->getCongestion();
        }
    }

    switch(congestionLevel){
        case 0:
            modifiedValue = 1; // normal speed
            break;
        case 1:
            modifiedValue = 0.5; // 2x slower
            break;
        case 2:
            modifiedValue = 0.25; // 4x slower
            break;
        case 3:
            modifiedValue = 0.20; // 5x slower (NOT WORKING)
            break;
    }

    // if value was negative, modified value have to be also negative
    if (val < 0)
        modifiedValue *= -1;

    dx = modifiedValue;
}


/**
 * @brief Sets Dy
 * @param val Value of axis y
 */
void Vehicle::setDy(qreal val)
{
    if (val == 0){
        dy = val;
        return;
    }

    qreal modifiedValue = val;
    int congestionLevel = 0;

    Stop *tmpStop1;
    Street *tmpStr1, *tmpStr2;

    // get legit congestion level
    // if vehicle is not passing in reversal order
    if (!reverse){

        tmpStop1 = route->value(currentStopIndex-1)->begin().value();

        tmpStr1 = route->value(currentStopIndex-1)->begin().value()->getStreet();
        tmpStr2 = route->value(currentStopIndex)->begin().value()->getStreet();

        // if both stops are on different streets
        if (!(tmpStr1 == tmpStr2)){

            // and vehicle is not on stop's coordinates yet
            if (!(tmpStop1->getCoordinate()->getX() == x && tmpStop1->getCoordinate()->getY() == y)){

                // vehicle is on the border of streets, congestion level is picked from the next stop's street
                congestionLevel = *tmpStr2->getCongestion();

            } else {
                congestionLevel = *tmpStr1->getCongestion();
            }

        // stops are on the same street, congestion level is the same for both options
        } else {
            congestionLevel = *tmpStr1->getCongestion();
        }

    // vehicle is passing in reversal order
    } else {

        tmpStop1 = route->value(currentStopIndex+1)->begin().value();


        tmpStr1 = route->value(currentStopIndex+1)->begin().value()->getStreet();
        tmpStr2 = route->value(currentStopIndex)->begin().value()->getStreet();

        // the same as in not reversal order
        if (!(tmpStr1 == tmpStr2)){

            if (!(tmpStop1->getCoordinate()->getX() == x && tmpStop1->getCoordinate()->getY() == y)){
                congestionLevel = *tmpStr2->getCongestion();
            } else {
                congestionLevel = *tmpStr1->getCongestion();
            }

        } else {
            congestionLevel = *tmpStr1->getCongestion();
        }
    }

    switch(congestionLevel){
        case 0:
            modifiedValue = 1; // normal speed
            break;
        case 1:
            modifiedValue = 0.5; // 2x slower
            break;
        case 2:
            modifiedValue = 0.25; // 4x slower
            break;
        case 3:
            modifiedValue = 0.20; // 5x slower (NOT WORKING)
            break;
    }

    // if value was negative, modified value have to be also negative
    if (val < 0)
        modifiedValue *= -1;

    dy = modifiedValue;
}

/**
 * @brief Getter for route
 * @return Route
 */
QList<QMap<Street *, Stop *> *>* Vehicle::getRoute()
{
    return this->route;
}

/**
 * @brief Setter for route
 * @param route Route
 * @param isEdit Editing switch
 */
void Vehicle::setRoute(QList<QMap<Street *, Stop *> *> *route, bool isEdit)
{
    isEdit ? this->routeEdit = route : this->route = route;
}


/**
 * @brief Getter for ID
 * @return id number
 */
QString Vehicle::getidNumber()
{
    return this->idNumber;
}


/**
 * @brief Getter for line number
 * @return Line number
 */
QString Vehicle::getLineNumber()
{
    return this->idLine;
}


/**
 * @brief Getter for x
 * @return x
 */
qreal Vehicle::getX()
{
    return this->x;
}

/**
 * @brief Getter for y
 * @return y
 */
qreal Vehicle::getY()
{
    return this->y;
}

/**
 * @brief Getter for last stop
 * @return Last stop
 */
Stop *Vehicle::getlastStop()
{
    return this->lastStop;
}

/**
 * @brief Getter for next stop
 * @return Next stop
 */
Stop *Vehicle::getnextStop()
{
    return this->nextStop;
}

/**
 * @brief Getter for timetable index
 * @return Timetable index
 */
qint16 Vehicle::getTimetableIndex()
{
    return this->timeTableIndex;
}

/**
 * @brief Getter for current stop index
 * @return Current stop index
 */
qint16 Vehicle::getCurrentStopIndex()
{
    return this->currentStopIndex;
}

/**
 * @brief Getter for timetable
 * @return Timetable
 */
QList<QMap<QString, qint16> *> *Vehicle::getTimetable()
{
    return this->timetable;
}

/**
 * @brief Setter for time handle
 * @param timeObj Global timer
 */
void Vehicle::setTimeHandle(Time *timeObj)
{
    this->timeHandle = timeObj;
}

/**
 * @brief Calculates trajectory of vehicle
 */
void Vehicle::calculateTrajectory()
{
    // if timetable has not been initialized yet, create it
    if (timetable == nullptr){
        createTimetable();
        qDebug() << "Timetable of vehicle " << getFullSignature() << ".";
        for (auto checkpoint : *timetable){
            qDebug() << checkpoint->begin().key() << " " << checkpoint->begin().value();
        }
    }

    if (this->x == this->nextStop->getCoordinate()->getX()){
        if (this->y == this->nextStop->getCoordinate()->getY()){
            waitOnStop();
        } else {

            if (this->x == route->value(this->currentStopIndex)->begin().key()->getBegin()->getX() &&
                    this->y == route->value(this->currentStopIndex)->begin().key()->getBegin()->getY()){
                calculateDirection();

            } else if ( this->x == route->value(this->currentStopIndex)->begin().key()->getEnd()->getX() &&
                        this->y == route->value(this->currentStopIndex)->begin().key()->getEnd()->getY()){
                calculateDirection();
            }

            this->x = this->x + this->dx;
            this->y = this->y + this->dy;

            if (displayed){
                //this->vehicleMark->setPen(QPen(Qt::darkBlue));
                this->vehicleMark->setRect(this->x-20, this->y-20, 40, 40);

            } else {
                //this->vehicleMark->setPen(QPen(Qt::darkRed));
                this->vehicleMark->setRect(this->x-15, this->y-15, 30, 30);
            }

            if (this->getLineNumber().toInt() < 10){
                 this->vehicleText->setPos(this->x-8.5, this->y-13);
            } else if (this->getLineNumber().toInt() < 100){
                this->vehicleText->setPos(this->x-12.5, this->y-13);
            } else if (this->getLineNumber().toInt() < 1000){
                this->vehicleText->setPos(this->x-16.5, this->y-13);
            }

        }

    } else if (x == route->value(currentStopIndex)->begin().key()->getBegin()->getX() &&
               y == route->value(currentStopIndex)->begin().key()->getBegin()->getY()){

        calculateDirection();
        this->x = this->x + this->dx;
        this->y = this->y + this->dy;

        if (displayed){
            //this->vehicleMark->setPen(QPen(Qt::darkBlue));
            this->vehicleMark->setRect(this->x-20, this->y-20, 40, 40);
        } else {
            //this->vehicleMark->setPen(QPen(Qt::darkRed));
            this->vehicleMark->setRect(this->x-15, this->y-15, 30, 30);
        }

        if (this->getLineNumber().toInt() < 10){
             this->vehicleText->setPos(this->x-8.5, this->y-13);
        } else if (this->getLineNumber().toInt() < 100){
            this->vehicleText->setPos(this->x-12.5, this->y-13);
        } else if (this->getLineNumber().toInt() < 1000){
            this->vehicleText->setPos(this->x-16.5, this->y-13);
        }

    }  else if (x == route->value(currentStopIndex)->begin().key()->getEnd()->getX() &&
                y == route->value(currentStopIndex)->begin().key()->getEnd()->getY()){

        calculateDirection();
        this->x = this->x + this->dx;
        this->y = this->y + this->dy;

        if (displayed){
            //this->vehicleMark->setPen(QPen(Qt::darkBlue));
            this->vehicleMark->setRect(this->x-20, this->y-20, 40, 40);
        } else {
            //this->vehicleMark->setPen(QPen(Qt::darkRed));
            this->vehicleMark->setRect(this->x-15, this->y-15, 30, 30);
        }

        if (this->getLineNumber().toInt() < 10){
             this->vehicleText->setPos(this->x-8.5, this->y-13);
        } else if (this->getLineNumber().toInt() < 100){
            this->vehicleText->setPos(this->x-12.5, this->y-13);
        } else if (this->getLineNumber().toInt() < 1000){
            this->vehicleText->setPos(this->x-16.5, this->y-13);
        }

    } else {

        this->x = this->x + this->dx;
        this->y = this->y + this->dy;

        if (displayed){
            //this->vehicleMark->setPen(QPen(Qt::darkBlue));
            this->vehicleMark->setRect(this->x-20, this->y-20, 40, 40);
        } else {
            //this->vehicleMark->setPen(QPen(Qt::darkRed));
            this->vehicleMark->setRect(this->x-15, this->y-15, 30, 30);
        }

        if (this->getLineNumber().toInt() < 10){
             this->vehicleText->setPos(this->x-8.5, this->y-13);
        } else if (this->getLineNumber().toInt() < 100){
            this->vehicleText->setPos(this->x-12.5, this->y-13);
        } else if (this->getLineNumber().toInt() < 1000){
            this->vehicleText->setPos(this->x-16.5, this->y-13);
        }
    }

}

/**
 * @brief Simulates waiting on stop
 */
void Vehicle::waitOnStop()
{

    if (timeout != 0){
        timeout++;
        if (timeout == 60){
            nextStop = route->value(currentStopIndex)->begin().value();
            calculateDirection();
            timeout = 0;
        }
        return;
    }

    qDebug() << "Vehicle " << getFullSignature() << " reached stop coordinates.";

    // reset x and y for better precision
    x = nextStop->getCoordinate()->getX();
    y = nextStop->getCoordinate()->getY();

    // vehicle has reached an end destination
    if (nextStop == endStop){
        reverse = !reverse;

        // a new route has been set
        if (routeEdit != nullptr){

            route = routeEdit;
            currentStopIndex = 0;
            lastStop = route->front()->begin().value();
            nextStop = route->front()->begin().value();
            startStop = route->begin().i->t()->begin().value();
            endStop = route->back()->begin().value();

            x = startStop->getCoordinate()->getX();
            y = startStop->getCoordinate()->getY();

            /*if (reverse){
                // set new stop index
                currentStopIndex = route->size()-1;
                lastStop = route->value(currentStopIndex)->begin().value();
                nextStop = route->value(currentStopIndex)->begin().value();
                x = lastStop->getCoordinate()->getX();
                y = lastStop->getCoordinate()->getY();
            }*/
            reverse = false;
            routeEdit = nullptr;

        } else if (resetEditRoute){

            route = routeDefault;

            lastStop = route->front()->begin().value();
            nextStop = route->value(1)->begin().value();
            startStop = route->begin().i->t()->begin().value();
            endStop = route->back()->begin().value();

            x = startStop->getCoordinate()->getX();
            y = startStop->getCoordinate()->getY();

            if (reverse){

                // set new stop index
                currentStopIndex = route->size() - 1;
                lastStop = route->value(currentStopIndex)->begin().value();
                nextStop = route->value(currentStopIndex)->begin().value();

                x = lastStop->getCoordinate()->getX();
                y = lastStop->getCoordinate()->getY();

            }

            resetEditRoute = false;

        } else {

            Stop *tmp = startStop;
            startStop = endStop;
            endStop = tmp;

            qDebug() << "Vehilce " << getFullSignature() << " reached end of the route...";

        }

        // recalculate time table
        timeTableIndex = 1;
        createTimetable();

    } else {

        if (!route->value(currentStopIndex)->begin().value()->getIsAux()){
            qDebug() << "Vehicle " << getFullSignature() << " waiting on a stop...";
            timeTableIndex++;
        } else {
            qDebug() << "Vehicle " << getFullSignature() << " just passed an auxiliary stop...";
        }
    }

    if (!route->value(currentStopIndex)->begin().value()->getIsAux()){

        if (!reverse){
            currentStopIndex++;
        } else {
            currentStopIndex--;
        }
        timeout = 1;
        lastStop = nextStop;

    } else {

        if (!reverse){
            currentStopIndex++;
        } else {
            currentStopIndex --;
        }
        nextStop = route->value(currentStopIndex)->begin().value();
        calculateDirection();
    }
}

/**
 * @brief Calculate direction of vehicle
 */
void Vehicle::calculateDirection()
{
    if (nextStop->getCoordinate()->getX() - x != 0 && nextStop->getCoordinate()->getY() - y != 0){

        qDebug() << "Calculating direction for vehicle " << getFullSignature() << ", next stop is on different street...";

        if (route->value(currentStopIndex)->begin().key()->getBegin()->getX() - x == 0){
            if (route->value(currentStopIndex)->begin().key()->getBegin()->getY() - y < 0){
                setDy(-1);

            } else {
                setDy(1);
            }
            setDx(0);


        } else if (route->value(currentStopIndex)->begin().key()->getEnd()->getX() - x == 0){
            if (route->value(currentStopIndex)->begin().key()->getEnd()->getY() - y < 0){
                setDy(-1);

            } else {
                setDy(1);
            }
            setDx(0);

        } else if (route->value(currentStopIndex)->begin().key()->getBegin()->getY() - y == 0){
            if (route->value(currentStopIndex)->begin().key()->getBegin()->getX() - x < 0){
                setDx(-1);

            } else {
                setDx(1);
            }
            setDy(0);

        } else if (route->value(currentStopIndex)->begin().key()->getEnd()->getY() - y == 0){
            if (route->value(currentStopIndex)->begin().key()->getEnd()->getX() - x < 0){
                setDx(-1);

            } else {
                setDx(1);
            }
            setDy(0);

        }

    } else if (nextStop->getCoordinate()->getX() - x != 0){
        if (nextStop->getCoordinate()->getX() - x <= 0){
            setDx(-1);
        } else {
            setDx(1);
        }
        setDy(0);

    } else if (nextStop->getCoordinate()->getY() - y != 0){
        if (nextStop->getCoordinate()->getY() - y >= 0){
            setDy(1);
        } else {
            setDy(-1);
        }
        setDx(0);
    }
}

/**
 * @brief Creates timetable
 */
void Vehicle::createTimetable()
{
    if (timetable == nullptr)
        timetable = new QList<QMap<QString, qint16> *>;

    // clear time table
    timetable->clear();

    Stop *startPoint = nullptr;
    Stop *nextPoint = nullptr;
    qint16 lastEstTime = timeHandle->getAppTime()->second() + timeHandle->getAppTime()->minute()*60 + timeHandle->getAppTime()->hour()*3600;
    qint16 estTime = 0;
    // from start -> end
    if (!reverse){

        startPoint = route->value(0)->begin().value();
        for (auto path : *route){

            if (nextPoint != nullptr && lastEstTime != 1 && !nextPoint->getIsAux()){
                estTime += 60; // add wait on stop delay (60 secs)
            }

            nextPoint = path->begin().value();

            int tmpEstTime = 0;
            if (startPoint->getStreet()->getId() == nextPoint->getStreet()->getId()){ // stops are on the same street
                tmpEstTime += startPoint->getCoordinate()->diffX(nextPoint->getCoordinate()) + startPoint->getCoordinate()->diffY(nextPoint->getCoordinate());
                if (tmpEstTime < 0){
                    tmpEstTime *= -1;
                }
                estTime += tmpEstTime * Time::getCongestionConstant(startPoint->getStreet());
            } else { // stops are on neighboring streets

                // if begin of current street neighbours with other street:
                if ((startPoint->getStreet()->getBegin()->Compare(nextPoint->getStreet()->getBegin())) || (startPoint->getStreet()->getBegin()->Compare(nextPoint->getStreet()->getEnd()))){
                    int distToBeginOfCurStr = startPoint->getCoordinate()->diffX(startPoint->getStreet()->getBegin()) + startPoint->getCoordinate()->diffY(startPoint->getStreet()->getBegin());
                    int distFromBeginToNextStop = startPoint->getStreet()->getBegin()->diffX(nextPoint->getCoordinate()) + startPoint->getStreet()->getBegin()->diffY(nextPoint->getCoordinate());

                    if (distToBeginOfCurStr < 0) distToBeginOfCurStr *= -1;
                    if (distFromBeginToNextStop < 0) distFromBeginToNextStop *= -1;

                    distToBeginOfCurStr *= Time::getCongestionConstant(startPoint->getStreet());
                    distFromBeginToNextStop *= Time::getCongestionConstant(nextPoint->getStreet());

                    estTime += distToBeginOfCurStr + distFromBeginToNextStop;

                // end of current street neighbours with other street:
                } else {
                    int distToEndOfCurStr = startPoint->getCoordinate()->diffX(startPoint->getStreet()->getEnd()) + startPoint->getCoordinate()->diffY(startPoint->getStreet()->getEnd());
                    int distFromEndToNextStop = startPoint->getStreet()->getEnd()->diffX(nextPoint->getCoordinate()) + startPoint->getStreet()->getEnd()->diffY(nextPoint->getCoordinate());

                    if (distToEndOfCurStr < 0) distToEndOfCurStr *= -1;
                    if (distFromEndToNextStop < 0) distFromEndToNextStop *= -1;

                    distToEndOfCurStr *= Time::getCongestionConstant(startPoint->getStreet());
                    distFromEndToNextStop *= Time::getCongestionConstant(nextPoint->getStreet());

                    estTime += distToEndOfCurStr + distFromEndToNextStop;
                }
            }

            // if nextPoint is and auxiliary stop, just save value and calculate further
            if (nextPoint->getIsAux()){
                startPoint = nextPoint;
                continue;
            }

            estTime += lastEstTime; // add last estimated time

            QMap<QString, qint16> *item = new QMap<QString, qint16>;
            item->insert(Time::convertTimeToString(&estTime), estTime);
            timetable->append(item);

            //timetable.add(new AbstractMap.SimpleImmutableEntry<String, Integer>(Time.convertTimeToString(estTime), estTime));

            startPoint = nextPoint;
            lastEstTime = estTime;
            estTime = 0;
        }

    // end -> start (vehicle is travelling in reverse order)
    } else {
        startPoint = route->value(route->size() - 1)->begin().value();

        for (int i = route->size() - 1; i >= 0; i--){
            if (nextPoint != nullptr && lastEstTime != 1 && !nextPoint->getIsAux()){
                estTime += 60; // add wait on stop delay (60 secs)
            }

            nextPoint = route->value(i)->begin().value();

            int tmpEstTime = 0;
            if (startPoint->getStreet()->getId() == nextPoint->getStreet()->getId()){ // stops are on the same street
                tmpEstTime += startPoint->getCoordinate()->diffX(nextPoint->getCoordinate()) + startPoint->getCoordinate()->diffY(nextPoint->getCoordinate());
                if (tmpEstTime < 0){
                    tmpEstTime *= -1;
                }
                estTime += tmpEstTime * Time::getCongestionConstant(startPoint->getStreet());
            } else { // stops are on neighboring streets

                // if begin of current street neighbours with other street:
                if (startPoint->getStreet()->getBegin()->Compare(nextPoint->getStreet()->getBegin()) || startPoint->getStreet()->getBegin()->Compare(nextPoint->getStreet()->getEnd())){
                    int distToBeginOfCurStr = startPoint->getCoordinate()->diffX(startPoint->getStreet()->getBegin()) + startPoint->getCoordinate()->diffY(startPoint->getStreet()->getBegin());
                    int distFromBeginToNextStop = startPoint->getStreet()->getBegin()->diffX(nextPoint->getCoordinate()) + startPoint->getStreet()->getBegin()->diffY(nextPoint->getCoordinate());

                    if (distToBeginOfCurStr < 0) distToBeginOfCurStr *= -1;
                    if (distFromBeginToNextStop < 0) distFromBeginToNextStop *= -1;

                    distToBeginOfCurStr *= Time::getCongestionConstant(startPoint->getStreet());
                    distFromBeginToNextStop *= Time::getCongestionConstant(nextPoint->getStreet());

                    estTime += distToBeginOfCurStr + distFromBeginToNextStop;

                // end of current street neighbours with other street:
                } else {
                    int distToEndOfCurStr = startPoint->getCoordinate()->diffX(startPoint->getStreet()->getEnd()) + startPoint->getCoordinate()->diffY(startPoint->getStreet()->getEnd());
                    int distFromEndToNextStop = startPoint->getStreet()->getEnd()->diffX(nextPoint->getCoordinate()) + startPoint->getStreet()->getEnd()->diffY(nextPoint->getCoordinate());

                    if (distToEndOfCurStr < 0) distToEndOfCurStr *= -1;
                    if (distFromEndToNextStop < 0) distFromEndToNextStop *= -1;

                    distToEndOfCurStr *= Time::getCongestionConstant(startPoint->getStreet());
                    distFromEndToNextStop *= Time::getCongestionConstant(nextPoint->getStreet());

                    estTime += distToEndOfCurStr + distFromEndToNextStop;
                }
            }

            // if nextPoint is and auxiliary stop, just save value and calculate further
            if (nextPoint->getIsAux()){
                startPoint = nextPoint;
                continue;
            }

            estTime += lastEstTime; // add last estimated time

            //timetable->add(new AbstractMap.SimpleImmutableEntry<String, Integer>(Time.convertTimeToString(estTime), estTime));

            QMap<QString, qint16> *item = new QMap<QString, qint16>;
            item->insert(Time::convertTimeToString(&estTime), estTime);
            timetable->append(item);

            startPoint = nextPoint;
            lastEstTime = estTime;
            estTime = 0;
        }
    }
}

/**
 * @brief Bool value for deployment
 * @return bool
 */
bool Vehicle::isDeployed()
{
    return this->deployed;
}

/**
 * @brief Method for releasing vehicle
 * @return bool
 */
bool Vehicle::checkDeployTime()
{
    if (deployTime == timeHandle->getAppTimeInSeconds()){
        deployed = true;
        vehicleMark->setVisible(true);
        vehicleText->setVisible(true);
        return true;
    } else {
        return false;
    }
}
