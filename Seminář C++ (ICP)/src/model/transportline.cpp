/*!
 * @file
 * @brief This file contains implementation of TransportLine class
 *
 * @author Michal Rein, Ondřej Motyčka
*/

#include "transportline.h"

/**
 * @brief TransportLine::createTransportLine Create new transport line containing only it's id. It's lists are being filled externally by other methods.
 * @param id Transport line number.
 * @return New TransportLine object.
 */
TransportLine* TransportLine::createTransportLine(QString *id)
{
    TransportLine *trLine = new TransportLine();
    trLine->id = *id;
    return trLine;
}

/**
 * @brief TransportLine::addStop Add stop object into list of stops.
 * @param stop Stop object which should be added.
 * @return Status of successful addition of a stop to the stop list.
 */
bool TransportLine::addStop(Stop *stop)
{
    if (this->stopList->empty()){
        this->stopList->append(stop);
        this->streetList->append(stop->getStreet());
        return true;
    }

    // if names of streets differ, follow must be checked
    if (this->streetList->back()->getId() != stop->getStreet()->getId()){

        // if lastly inserted street follows street of the next stop
        if (this->streetList->back()->follows(stop->getStreet())){
            this->streetList->append(stop->getStreet());
            this->stopList->append(stop);

        } else {
            return false;
        }

    // stop is on the same street -> just add it to the stop list
    } else {
        this->stopList->append(stop);
    }

    return true;
}

/**
 * @brief TransportLine::addVehicle Create new Vehicle object and add it into list of Vehicle objects.
 * @param lineId Transport line id.
 * @param vehicleNumber Vehicle number id.
 * @param deploy Deploy time in seconds.
 * @return True value on successfull creation.
 */
bool TransportLine::addVehicle(QString *lineId, QString *vehicleNumber, qint16 deploy)
{
    qDebug() << "Creating new vehicle " << lineId << " (" << vehicleNumber << "). ";
    Vehicle *vehicle = new Vehicle(lineId, vehicleNumber, this->route, deploy);
    this->vehicleList->append(vehicle);
    return true;
}

/**
 * @brief TransportLine::createRoute Take streets and stop from lists and create a new list of QMap<Street, Stop> entries representing route.
 * @param stopList List of Stop objects.
 * @param streetList List of Street objects.
 * @return List of QMap entries representing route.
 */
QList<QMap<Street *, Stop *> *> *TransportLine::createRoute(QList<Stop *> *stopList, QList<Street *> *streetList)
{
    QList<QMap<Street *, Stop *> *> *routeList = new QList<QMap<Street *, Stop *> *>;

    for (auto str : *streetList){

        bool atLeastOne = false;
        for (auto stop : *stopList){
            if (str->getId()->data() == stop->getStreet()->getId()->data()){
                QMap<Street*, Stop*> *item = new QMap<Street *, Stop *>;
                item->insert(str, stop);
                routeList->append(item);
                atLeastOne = true;
            }
        }
        if (!atLeastOne){
            QMap<Street*, Stop*> *item = new QMap<Street *, Stop *>;

            // create new auxiliary stop
            QString aux = "AuxStop";
            qint16 auxX = (str->getBegin()->getX() + str->getEnd()->getX())/2;
            qint16 auxY = (str->getBegin()->getY() + str->getEnd()->getY())/2;
            Stop *auxStop = new Stop(&aux, Coordinate::createCoordinate(auxX, auxY), true);
            auxStop->setStreet(str);

            item->insert(str, auxStop);
            routeList->append(item);
        }
    }

    return routeList;
}

/**
 * @brief TransportLine::getVehicleList Get list of Vehicle objects.
 * @return List of Vehicle objects.
 */
QList<Vehicle *> *TransportLine::getVehicleList()
{
    return this->vehicleList;
}

/**
 * @brief TransportLine::getRoute Get route.
 * @return Route.
 */
QList<QMap<Street *, Stop *> *> *TransportLine::getRoute()
{
    return route;
}

/**
 * @brief TransportLine::setEditRoute Set new route to all Vehicle objects.
 */
void TransportLine::setEditRoute()
{
    for (auto vehicle : *vehicleList){
        vehicle->setRoute(routeEdit, true);
    }
}

/**
 * @brief TransportLine::resetEditRoute Reset route of all Vehicle object to default state.
 */
void TransportLine::resetEditRoute()
{
    for (auto vehicle : *vehicleList){
        vehicle->resetEditRoute = true;
    }
}

/**
 * @brief TransportLine::getStopList Get list of Stop objects.
 * @return List of Stop objects.
 */
QList<Stop *> *TransportLine::getStopList()
{
    return this->stopList;
}

/**
 * @brief TransportLine::getStreetList Get list of Street objects.
 * @return List of Street objects.
 */
QList<Street *> *TransportLine::getStreetList()
{
    return this->streetList;
}

/**
 * @brief TransportLine::getId Get transport line identifier.
 * @return Identifier.
 */
QString *TransportLine::getId()
{
    return &this->id;
}


