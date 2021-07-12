/*!
 * @file
 * @brief This file contains implementation of myscene
 *
 * @author Michal Rein, Ondřej Motyčka
 */

#include "myscene.h"
#include <QDebug>
#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>


/**
 * @brief Constructor of class MyScene \n
 * Creates a new canvas
 * @param parent Graphics View
 * @param map Map
 * @param mainWindow Main Window
 */
MyScene::MyScene(QObject *parent, StreetMap *map, MainWindow *mainWindow) : QGraphicsScene(parent)
{
    this->mapHandle = map;
    this->mainWindowHandle = mainWindow;
}

/**
 * @brief Handles mouse press events
 * @param event Event
 */
void MyScene::mousePressEvent(QGraphicsSceneMouseEvent *event){

    //qDebug() << event->pos() << event->scenePos() << event->screenPos();

    // if app is in an edit mode
    if (mainWindowHandle->editRouteMode){

        // on left click
        if (event->button() == Qt::LeftButton){

            // iterate through items on the click position
            for (auto *item : items(event->scenePos())){

                // object is a stop mark
                if (auto stopMark = dynamic_cast<QGraphicsRectItem*>(item); stopMark != nullptr){

                    if (auto stop = mapHandle->findStop(stopMark); stop != nullptr){

                        // if mark is not painted red already (so it wasn't added yet)
                        if (!(stop->stopMarkHandle->brush().color().red() == 220)){
                            mainWindowHandle->editRouteStops->append(stop);
                            stop->stopMarkHandle->setBrush(QBrush(QColor(220,20,60)));
                            qDebug() << "Stop " << *stop->getId() << " has been added to the edit list.";
                        }
                    }

                // object is a street's line
                } else if (auto streetLine = dynamic_cast<QGraphicsLineItem*>(item); streetLine != nullptr){

                    if (auto street = mapHandle->findStreet(streetLine); street != nullptr){

                        // if street's line object is not painted red already (so it wasn't added yet)
                        if (!(street->getLineHandle()->pen().color().red() == 220)){
                            mainWindowHandle->editRouteStreets->append(street);
                            street->getLineHandle()->setPen(QPen({QColor(220,20,60)}, 8));
                            qDebug() << "Street " << *street->getId() << " has been added to the edit list.";
                        }

                    }

                }
            }
        }

        if (event->button() == Qt::RightButton){

            for (auto stop : *mainWindowHandle->editRouteStops){
                stop->stopMarkHandle->setBrush(QBrush(QColor(11, 181, 25)));
            }

            for (auto street : *mainWindowHandle->editRouteStreets){
                street->getLineHandle()->setPen(QPen({Qt::black}, 5));
            }

            // clear edit lists (edit canceled)
            mainWindowHandle->editRouteStops->clear();
            mainWindowHandle->editRouteStreets->clear();
            qDebug() << "Route edit canceled... Route lists have been cleared.";
        }

    // app in not in route edit mode
    } else {

        if (event->button() == Qt::LeftButton){

            // get array of item pointers that are present within click area
            // for each item that are present within click area (items() will return all items, items(scenePos()) only specific items)
            for (auto* item : items(event->scenePos())){

                // try to dynamic cast item pointer into LineItem
                // if type cast was successfull, we clicked on a line object
                if (auto line = dynamic_cast<QGraphicsLineItem*>(item); line != nullptr){

                    // try to match street object
                    if (auto street = mapHandle->findStreet(line); street != nullptr){
                        mainWindowHandle->setStreetStatus(street);
                        return;
                    }

                } else if (auto stopMark = dynamic_cast<QGraphicsRectItem*>(item); stopMark != nullptr){

                    // try to match stop object
                    if (auto stop = mapHandle->findStop(stopMark); stop != nullptr){
                        mainWindowHandle->setStopStatus(stop);
                        return;
                    }

                } else if (auto vehicleMark = dynamic_cast<QGraphicsEllipseItem*>(item); vehicleMark != nullptr){

                    // try to match vehicle object
                    if (auto vehicle = mapHandle->findVehicle(vehicleMark); vehicle != nullptr){
                        mainWindowHandle->setLineStatus(vehicle);
                        return;
                    }

                }else if (auto text = dynamic_cast<QGraphicsTextItem*>(item); text != nullptr){
                    qDebug() << "Text";
                }
            }
        }

        if (event->button() == Qt::RightButton){
            qDebug() << "Right click pressed...";
            mainWindowHandle->setStreetStatus(nullptr);
            mainWindowHandle->setStopStatus(nullptr);
            mainWindowHandle->setLineStatus(nullptr);
        }
    }

    QGraphicsScene::mousePressEvent(event);
}
