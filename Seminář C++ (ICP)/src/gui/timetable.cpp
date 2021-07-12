/*!
 * @file
 * @brief This file contains implementation of timetable
 *
 * @author Michal Rein, Ondřej Motyčka
 */

#include "timetable.h"
#include "ui_timetable.h"

/**
 * @brief Constructor of class timetable \n
 * Setups new ui
 * @param parent Parent widget
 */
Timetable::Timetable(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Timetable)
{
    ui->setupUi(this);
}


/**
 * @brief Creates timetable window for vehicle
 * @param vehicle Selected vehicle
 */
void Timetable::vehicleTimetable(Vehicle *vehicle)
{
    int counter = 0;

    // set timetable index
    int timetableIndex = 0;
    if (vehicle->reverse){
        timetableIndex = vehicle->getTimetable()->size() - 1;
    }

    auto route = vehicle->getRoute();

    for(auto route_iterator: *route){
        if(route_iterator->first()->getIsAux()){
            continue;
        }
        auto line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        QLabel *label_counter = nullptr;
        if (!vehicle->reverse){
            label_counter = new QLabel("----- " + QString::number(counter) + " -----");
        } else {
            label_counter = new QLabel("----- " + QString::number(timetableIndex) + " -----");
        }

        auto label_1 = new QLabel("Street: " + *route_iterator->firstKey()->getId(),this);
        auto label_2 = new QLabel("Stop: " + *route_iterator->first()->getId(),this);

        //Pridat cas
        auto label_3 = new QLabel("Estimated arrival: " + vehicle->getTimetable()->value(timetableIndex)->begin().key(), this);

        ui->verticalLayout->addWidget(label_counter);
        ui->verticalLayout->addWidget(label_1);
        ui->verticalLayout->addWidget(label_2);
        ui->verticalLayout->addWidget(label_3);
        ui->verticalLayout->addWidget(line);
        counter++;

        vehicle->reverse ? timetableIndex-- : timetableIndex++;
    }

    ui->verticalLayout->addStretch(0);
}

/**
 * @brief Creates timetable window for stop
 * @param stop Active stop
 * @param map Active map
 */
void Timetable::stopTimetable(Stop *stop, StreetMap *map)
{
    for (auto transportLine : *map->getTransportLineList()){
        for (auto stopIteration : *transportLine->getStopList()){
            if (stopIteration == stop){

                auto line = new QFrame;
                line->setFrameShape(QFrame::HLine);
                line->setFrameShadow(QFrame::Sunken);

                QLabel *labelLineId = new QLabel("----- " + *transportLine->getId() + " -----");
                ui->verticalLayout->addWidget(labelLineId);

                int thisStopIndex = 0;
                for (auto vehicle : *transportLine->getVehicleList()){

                    if (vehicle->isDeployed()){
                        for (auto path : *vehicle->getRoute()){
                            if (path->begin().value() == stop){
                                QLabel *labelVehicleArrival = new QLabel(vehicle->getTimetable()->value(thisStopIndex)->begin().key() + " (" + vehicle->getidNumber() + ")");
                                ui->verticalLayout->addWidget(labelVehicleArrival);
                                thisStopIndex = 0;
                                break;
                            } else {
                                if (!path->begin().value()->getIsAux())
                                    thisStopIndex++;
                            }
                        }
                    }

                }
                ui->verticalLayout->addWidget(line);
            }
        }
    }

    ui->verticalLayout->addStretch(0);
}

/**
 * @brief Destructor of class Timetable
 */
Timetable::~Timetable()
{
    delete ui;
}
