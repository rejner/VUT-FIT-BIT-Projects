/*!
 * @file
 * @brief This file contains implementation of class StreetMap
 *
 * @author Michal Rein, Ondřej Motyčka
 */
#include "streetmap.h"

/**
 * @brief StreetMap::StreetMap Object constructor.
 */
StreetMap::StreetMap()
{
    qDebug() << "Street map has been created.";
}

/**
 * @brief StreetMap::createReferenceMap Parse JSON file with reference map and fill lists with objects.
 */
void StreetMap::createReferenceMap(QString path)
{
    QString val;
    QFile file;
    file.setFileName(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();

    QJsonDocument document = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject object_json = document.object();
    QJsonArray street_array = object_json["streets"].toArray();
    QJsonArray lines_array  = object_json["lines"].toArray();

    QString street_id, stop_id, line_id;
    QJsonArray begin, end, stops, stops_coordinates, route;
    qint16 begin_x, begin_y, end_x, end_y, congestion_level, stop_x, stop_y;

    /* STREETS PART */
    //Reading an array of streets
    foreach (const QJsonValue & street_val, street_array) {
        QJsonObject temp_object = street_val.toObject();

        street_id = temp_object["streetName"].toString();
        congestion_level = temp_object["congestion"].toInt();

        begin = temp_object["begin"].toArray();
        begin_x = begin[0].toInt();
        begin_y = begin[1].toInt();

        end = temp_object["end"].toArray();
        end_x = end[0].toInt();
        end_y = end[1].toInt();

        Street* str = Street::createStreet(&street_id, congestion_level, Coordinate::createCoordinate(begin_x, begin_y), Coordinate::createCoordinate(end_x,end_y));

        stops = temp_object["stops"].toArray();
        if(!stops.isEmpty()){

            /* STOPS PART */
            foreach(const QJsonValue & stop_val, stops){
                QJsonObject stop_temp_obj = stop_val.toObject();

                stop_id = stop_temp_obj["stopName"].toString();

                stops_coordinates = stop_temp_obj["coordinates"].toArray();
                stop_x = stops_coordinates[0].toInt();
                stop_y = stops_coordinates[1].toInt();

                Stop *stop = new Stop(&stop_id, Coordinate::createCoordinate(stop_x, stop_y), false);

                this->stopList->append(stop);
                str->addStop(stop);
            }
        }
        this->streetList->append(str);
    }

    /* TRANSPORT LINE PART */
    foreach(const QJsonValue & lines_val, lines_array){
        QJsonObject temp_object = lines_val.toObject();

        line_id = temp_object["lineNumber"].toString();
        TransportLine *Line = TransportLine::createTransportLine(&line_id);

        route = temp_object["route"].toArray();
        foreach(const QJsonValue & route_val, route){
            QJsonObject route_obj = route_val.toObject();
            QJsonArray stops_array = route_obj["stops"].toArray();

            if(!stops_array.isEmpty()){
                for(auto stops_iter: stops_array){
                    QString stop_id = stops_iter.toString();
                    for(auto stoplist_iter: *this->getStopList()){
                        if(stoplist_iter->getId() == stop_id){
                            Line->addStop(stoplist_iter);
                        }
                    }
                }
            } else {
                for(auto street_iter: *this->getStreetList()){
                    if(street_iter->getId() == route_obj["streetName"].toString()){
                        QString aux = "AuxStop";
                        qint16 auxX = (street_iter->getBegin()->getX() + street_iter->getEnd()->getX())/2;
                        qint16 auxY = (street_iter->getBegin()->getY() + street_iter->getEnd()->getY())/2;
                        Stop *auxStop = new Stop(&aux, Coordinate::createCoordinate(auxX, auxY), true);
                        auxStop->setStreet(street_iter);
                        Line->addStop(auxStop);
                    }
                }
            }
        }

        // create route
        Line->route = TransportLine::createRoute(Line->getStopList(), Line->getStreetList());
        this->getTransportLineList()->append(Line);

        /* VEHICLES PART */
        // extract all vehicles associated with current transport line
        QJsonArray vehicles_array = temp_object["vehicles"].toArray();
        for(auto vehicles_iter: vehicles_array){
            QJsonObject vehicle_obj = vehicles_iter.toObject();
            qint16 vehicle_number = vehicle_obj["id"].toInt();
            QString vehicle_id = QString::number(vehicle_number);
            QString vehicle_deploy = vehicle_obj["deploy"].toString();
            qint16 time_deploy = Time::convertStringToTime(&vehicle_deploy);
            QString linka = *Line->getId();

            Line->addVehicle(&linka, &vehicle_id,time_deploy);
        }
    }
}

/**
 * @brief StreetMap::getStreetList Get list of Street objects.
 * @return List of Street objects.
 */
QList<Street *> *StreetMap::getStreetList()
{
    return this->streetList;
}

/**
 * @brief StreetMap::getStopList Get list of Stop objects.
 * @return List of Stop objects.
 */
QList<Stop *> *StreetMap::getStopList()
{
    return this->stopList;
}

/**
 * @brief StreetMap::getTransportLineList Get list of TransportLine objects.
 * @return List of TransportLine objects.
 */
QList<TransportLine *> *StreetMap::getTransportLineList()
{
    return this->transportLineList;
}

/**
 * @brief StreetMap::findStreet Find Street object associated with given line QGraphicsLineItem item.
 * @param line QGraphicsLineItem handle of a Street that we are looking for.
 * @return Found Street object on success, otherwise nullptr when not found.
 */
Street *StreetMap::findStreet(QGraphicsLineItem *line)
{
    for (auto street : *this->streetList){
        if (street->getLineHandle() == line){
            return street;
        }
    }

    return nullptr;
}

/**
 * @brief StreetMap::findStop Find Stop object associated with given rectangle QGraphicsRectItem item.
 * @param mark QGraphicsRectItem handle of a Stop that we are looking for.
 * @return Found Stop object on success, otherwise nullptr when not found.
 */
Stop *StreetMap::findStop(QGraphicsRectItem *mark)
{
    for (auto stop : *this->stopList){
        if (stop->stopMarkHandle == mark){
            return stop;
        }
    }

    return nullptr;
}

/**
 * @brief StreetMap::findVehicle Find Vehicle object associated with given ellipse QGraphcisEllipseItem item.
 * @param mark QGraphicsEllipseItem handle of a Vehicle that we are looking for.
 * @return Found Vehicle object on success, otherwise nullptr when not found.
 */
Vehicle *StreetMap::findVehicle(QGraphicsEllipseItem *mark)
{
    for (auto transportLine : *this->transportLineList){
        for (auto vehicle : *transportLine->getVehicleList()){
            if (vehicle->vehicleMark == mark){
                return vehicle;
            }
        }
    }

    return nullptr;
}

/**
 * @brief StreetMap::findTransportLine Find TransportLine object associated with given Vehicle object.
 * @param veh Vehicle object.
 * @return TransportLine object of which given Vehicle object is part of.
 */
TransportLine *StreetMap::findTransportLine(Vehicle *veh)
{
    for (auto transportLine : *this->transportLineList){
        for (auto vehicle : *transportLine->getVehicleList()){
            if (vehicle == veh){
                return transportLine;
            }
        }
    }

    return nullptr;
}
