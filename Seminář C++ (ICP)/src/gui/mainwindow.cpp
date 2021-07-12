/*!
 * @file
 * @brief This file contains implementation of mainwindow
 *
 * @author Michal Rein, Ondřej Motyčka
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "help_window.h"
#include "intro.h"

#include <QLabel>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QDebug>
#include <QComboBox>
#include <QMessageBox>
#include "gui/myscene.h"



/**
 * @brief Constructor of Main Window \n
 * Constructor of class MainWindow creates default working space. It setups the Ui aka the main window. The actuall map is then created.
 * Aswell it creates a new timer that is used throughout the program. The time variable is bound to method update_time which is then called every tick of the timer.
 * Scene is initialized and all signals are bound to their slots.
 * @param parent Parent widget
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("ICP project");

    //Creates new map
    map = new StreetMap();

    intro *intro_new = new intro(this, map);
    intro_new->setWindowTitle("Welcome");

    intro_new->show();
    intro_new->exec();

    //Creates global timer
    time = new Time();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update_time()));

    initScene();

    timer->start(1000);

    /* --- STATUS BAR --- */
    /* Time */
     connect(ui->Speed_selector, SIGNAL(activated(int)) , this, SLOT(speed_selector(int)));

    /* Line */
    connect(ui->Line_selector, SIGNAL(activated(QString)) , this, SLOT(line_selector(QString)));
    connect(ui->Show_route_selected, &QPushButton::clicked, this, &MainWindow::show_route_clicked);
    connect(ui->editRouteButton, &QPushButton::clicked, this, &MainWindow::editRoute);
    connect(ui->saveEditButton, &QPushButton::clicked, this, &MainWindow::saveEditRoute);
    connect(ui->cancelEditButton, &QPushButton::clicked, this, &MainWindow::cancelEditRoute);

    /* Stop */
    connect(ui->Stop_selector, SIGNAL(activated(QString)) , this, SLOT(stop_selector(QString)));
    connect(ui->timetableButton, &QPushButton::clicked, this, &MainWindow::timetableShow);

    /* Street */
    connect(ui->Street_selector, SIGNAL(activated(QString)) , this, SLOT(street_selector(QString)));
    connect(ui->congestionChoiceBox, SIGNAL(activated(QString)) , this, SLOT(congestionEdit(QString)));

    /* --- FEATURES --- */
    connect(ui->zoomSlider, &QSlider::valueChanged, this, &MainWindow::zoom);
    connect(ui->Show_congestion_check, SIGNAL(stateChanged(int)), this, SLOT(show_congestion_click(int)));
    connect(ui->Show_street_names_check, SIGNAL(stateChanged(int)), this, SLOT(show_str_name_click(int)));
    connect(ui->actionShow_help, SIGNAL(triggered()), this, SLOT(help_task()));
}

/**
 * @brief Destructor of MainWindow class
 */
MainWindow::~MainWindow()
{
    delete time;
    delete timer;
    delete ui;
}

/**
 * @brief Sets street status \n
 * Changes visuals of street when clicked on or chosen from combo box.
 * @param street Selected street
 */
void MainWindow::setStreetStatus(Street *street)
{

    if (lastSelectedStreet != nullptr){
        if (ui->Show_congestion_check->isChecked()){

            switch(*lastSelectedStreet->getCongestion()){
            case 0:
                lastSelectedStreet->getLineHandle()->setPen(QPen(QColor(0x99ff33), 5)); // green
                break;
            case 1:
                lastSelectedStreet->getLineHandle()->setPen(QPen(QColor(0xffff66), 5)); // yellow
                break;
            case 2:
                lastSelectedStreet->getLineHandle()->setPen(QPen(QColor(0xff8000), 5)); // orange
                break;
            case 3:
                lastSelectedStreet->getLineHandle()->setPen(QPen(QColor(0xff6666), 5)); // red
                break;
            }

        } else {

            lastSelectedStreet->getLineHandle()->setPen(QPen({Qt::black}, 5));
        }

        lastSelectedStreet = nullptr;

    }

    if (street == nullptr){
        ui->Start_selected->setText("-");
        ui->End_selected->setText("-");
        ui->congestionChoiceBox->setEnabled(false);
        ui->congestionChoiceBox->setCurrentText("");
        ui->Street_selector->setCurrentText("");
        return;
    }

    // set selector text
    ui->Street_selector->setCurrentText(*street->getId());

    //highlight street
    street->getLineHandle()->setPen(QPen({QColor(24, 110, 196)}, 8));

    QString temp1 = QString::number(street->getBegin()->getX());
    QString temp2 = QString::number(street->getBegin()->getY());
    QString street_label("[ " + temp1 +", " + temp2 + " ]");
    ui->Start_selected->setText(street_label);

    temp1 = QString::number(street->getEnd()->getX());
    temp2 = QString::number(street->getEnd()->getY());
    street_label = ("[ " + temp1 +", " + temp2 + " ]");
    ui->End_selected->setText((street_label));

    // set congestion choice box
    ui->congestionChoiceBox->setEnabled(true);
    switch (*street->getCongestion()){

        case 0:
            ui->congestionChoiceBox->setCurrentText("Normal");
            break;

        case 1:
            ui->congestionChoiceBox->setCurrentText("Slight");
            break;

        case 2:
            ui->congestionChoiceBox->setCurrentText("Hard");
            break;
    }


    //ui->Congestion_selected->setText(QString::number(*street->getCongestion()));

    lastSelectedStreet = street;
}

/**
  * @brief Sets stop status \n
  * Changes visuals of stop when clicked on or chosen from combo box.
  * @param stop Selected stop
  */
 void MainWindow::setStopStatus(Stop *stop)
{
    if (lastSelectedStop != nullptr){
        lastSelectedStop->stopMarkHandle->setRect(lastSelectedStop->stopMarkHandle->rect().x()+4.6, lastSelectedStop->stopMarkHandle->rect().y()+4.4, 15, 15);
        lastSelectedStop->stopMarkHandle->setBrush(QBrush(QColor(11, 181, 25)));
        lastSelectedStop = nullptr;
    }

    if (stop == nullptr){
        ui->Position_stop_selected->setText("-");
        ui->Stop_selector->setCurrentText("");

        // disable timetable button
        ui->timetableButton->setEnabled(false);
        return;
    }

    ui->Stop_selector->setCurrentText(*stop->getId());

    //highlight stop
    stop->stopMarkHandle->setRect(stop->getCoordinate()->getX()-12.5, stop->getCoordinate()->getY()-12.5, 25, 25);
    stop->stopMarkHandle->setBrush(QBrush(QColor(107, 199, 32)));

    QString temp1 = QString::number(stop->getCoordinate()->getX());
    QString temp2 = QString::number(stop->getCoordinate()->getY());
    QString stop_label("[ " + temp1 +", " + temp2 + " ]");
    ui->Position_stop_selected->setText(stop_label);

    // enable timetable button
    ui->timetableButton->setEnabled(true);

    lastSelectedStop = stop;
}

 /**
 * @brief Sets line status \n
 * Changes visuals of line when clicked on or chosen from combo box.
 * @param vehicle Selected vehicle
 */
void MainWindow::setLineStatus(Vehicle *vehicle)
{
    // if lastSelectedLine is set
    if (lastSelectedLine != nullptr){
        // find vehicle to which this object is linked to
        for (auto transportLine : *map->getTransportLineList()){
            for (auto vehicle : *transportLine->getVehicleList()){
                if (lastSelectedLine == vehicle){
                    vehicle->displayed = false;
                    for(auto iter_streets: *vehicle->getRoute()){
                        iter_streets->firstKey()->getLineHandle()->setPen(QPen{Qt::black,5});
                    }

                }
            }
        }

        lastSelectedLine = nullptr;
    }

    // when nullptr appears, status should reset
    if (vehicle == nullptr){
        ui->Vehicle_number_selected->setText("");
        ui->Line_selector->setCurrentText("");
        ui->Position_line_selected->setText("-");
        ui->Position_stop_selected->setText("-");
        ui->Last_stop_selected->setText("-");
        ui->Next_stop_selected->setText("-");
        ui->Next_stop_arrival_selected->setText("-");

        // disable all related buttons
        ui->Show_route_selected->setEnabled(false);
        ui->editRouteButton->setEnabled(false);
        return;
    }

    ui->Line_selector->setCurrentText(vehicle->getFullSignature());
    ui->Vehicle_number_selected->setText(vehicle->getidNumber());

    vehicle->displayed = true;

    QString temp1 = QString::number(qRound(vehicle->getX()));
    QString temp2 = QString::number(qRound(vehicle->getY()));
    QString line_label("[ " + temp1 +", " + temp2 + " ]");
    ui->Position_line_selected->setText(line_label);

    // if vehicle isn't moving in reversal order
    if (!vehicle->reverse){

        // if last stop is an auxiliary one, skip it (last valid stop will be used)
        if (!vehicle->getlastStop()->getIsAux()){
            ui->Last_stop_selected->setText(*vehicle->getlastStop()->getId());
        }

        // if next stop isn't an auxiliary one, display it
        if (!vehicle->getnextStop()->getIsAux()){
            ui->Next_stop_selected->setText(*vehicle->getnextStop()->getId());
        } else {
            // find next non-auxiliary stop in the route list and set that as a next stop
            int i = 0;
            while (vehicle->getRoute()->value(vehicle->getCurrentStopIndex() + i)->begin().value()->getIsAux()){
                i++;
            }
            ui->Next_stop_selected->setText(*vehicle->getRoute()->value(vehicle->getCurrentStopIndex() + i)->begin().value()->getId());
        }

    // vehicle is moving in reversal order
    } else {

        // same as above, search for the next non-auxiliary stop is just done also in reversal order (from the last element to first)
        if (!vehicle->getlastStop()->getIsAux()){
            ui->Last_stop_selected->setText(*vehicle->getlastStop()->getId());
        }

        if (!vehicle->getnextStop()->getIsAux()){
            ui->Next_stop_selected->setText(*vehicle->getnextStop()->getId());
        } else {
            int i = 0;
            while (vehicle->getRoute()->value(vehicle->getCurrentStopIndex() - i)->begin().value()->getIsAux()){
                i++;
            }
            ui->Next_stop_selected->setText(*vehicle->getRoute()->value(vehicle->getCurrentStopIndex() - i)->begin().value()->getId());
        }
    }

    ui->Next_stop_arrival_selected->setText(vehicle->getTimetable()->value(vehicle->getTimetableIndex())->begin().key());

    for(auto iter_streets: *vehicle->getRoute()){
        iter_streets->firstKey()->getLineHandle()->setPen(QPen(QColor(24, 110, 196), 8));
    }

    // enable all related buttons
    ui->Show_route_selected->setEnabled(true);
    ui->editRouteButton->setEnabled(true);

    lastSelectedLine = vehicle;
}

/**
 * @brief Zooming in view \n
 * Method used for zooming in main window.
 * @param val Value of zoom
 */
void MainWindow::zoom(int val){
    auto org = ui->graphicsView->transform();
    qreal scale = val / 10.0;
    ui->graphicsView->setTransform(QTransform(scale, org.m12(), org.m21(), scale, org.dx(), org.dy()));
}

/**
 * @brief Shows route of currently selected line.
 */
void MainWindow::show_route_clicked()
{
    if(lastSelectedLine != nullptr){
        timetable_window = new Timetable(this);
        timetable_window->vehicleTimetable(lastSelectedLine);
        timetable_window->setWindowTitle("Line "+lastSelectedLine->getFullSignature());
        timetable_window->show();
    }
}

/**
 * @brief Lets you edit default route of line.
 */
void MainWindow::editRoute(){

    if (lastSelectedLine != nullptr){
        timer->stop();
        ui->saveEditButton->setEnabled(true);
        ui->cancelEditButton->setEnabled(true);
        ui->editRouteButton->setEnabled(false);
        editRouteMode = true;

    } else {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Please select a vehicle first in order to edit it's route.");
        msgBox.exec();
    }

}


/**
 * @brief Saves newly edited route
 */
void MainWindow::saveEditRoute(){

    editRouteMode = false;
    ui->saveEditButton->setEnabled(false);
    ui->cancelEditButton->setEnabled(false);
    ui->editRouteButton->setEnabled(true);

    // find transport line (changes will affect all vehicles linked to this transport line)
    TransportLine *tl = map->findTransportLine(lastSelectedLine);
    // set editted version of route to this transport line
    tl->routeEdit = TransportLine::createRoute(editRouteStops, editRouteStreets);
    // distribute new editted route to all it's sub-vehicles
    tl->setEditRoute();

    for (auto stop : *editRouteStops){
        stop->stopMarkHandle->setBrush(QBrush(QColor(11, 181, 25)));
    }

    for (auto street : *editRouteStreets){
        street->getLineHandle()->setPen(QPen({Qt::black}, 5));
    }

    editRouteStops->clear();
    editRouteStreets->clear();

    //qDebug() << "Changes to the line " << *tl->getId() << " has been saved. Changes will manifest once vehicle reach start or end destination.";

    timer->start();
}


/**
 * @brief Cancels editing of route
 */
void MainWindow::cancelEditRoute(){

    editRouteMode = false;
    ui->saveEditButton->setEnabled(false);
    ui->cancelEditButton->setEnabled(false);
    ui->editRouteButton->setEnabled(true);

    for (auto stop : *editRouteStops){
        stop->stopMarkHandle->setBrush(QBrush(QColor(11, 181, 25)));
    }

    for (auto street : *editRouteStreets){
        street->getLineHandle()->setPen(QPen({Qt::black}, 5));
    }

    editRouteStops->clear();
    editRouteStreets->clear();

    // find transport line (changes will affect all vehicles linked to this transport line)
    //TransportLine *tl = map->findTransportLine(lastSelectedLine);
    // spread the word that we are going back to normal to all vehicles
    //tl->resetEditRoute();

    timer->start();
}


/**
 * @brief Selects the speed of the simulation
 * @param val Index of speed
 */
void MainWindow::speed_selector(int val)
{
    switch(val){
    case 0:
        timer->start(1000);
        break;
    case 1:
        timer->start(500);
        break;
    case 2:
        timer->start(1000/3);
        break;
    case 3:
        timer->start(1000/4);
        break;
    case 4:
        timer->start(1000/5);
        break;
    case 5:
        timer->start(1000/100);
        break;
    }
}


/**
 * @brief Selects line
 * @param text Name of line
 */
void MainWindow::line_selector(QString text)
{
    if(text == ""){
        setLineStatus(nullptr);
    }
    else{
        for(auto iterator_lines: *map->getTransportLineList()){
            for(auto iterator_vehicle: *iterator_lines->getVehicleList()){
                if(iterator_vehicle->getFullSignature() == text){
                    setLineStatus(iterator_vehicle);
                }
            }
        }
    }
}


/**
 * @brief Selects street.
 * @param text Name of stret
 */
void MainWindow::street_selector(QString text)
{
    if(text == ""){
        setStreetStatus(nullptr);
    }
    else{
        for(auto iterator_streets: *map->getStreetList()){
            if(iterator_streets->getId() == text){
                setStreetStatus(iterator_streets);
            }
        }
    }
}


/**
 * @brief Selects stop.
 * @param text Name of stop.
 */
void MainWindow::stop_selector(QString text)
{
    if(text == ""){
        setStopStatus(nullptr);
    }
    else{
        for(auto iterator_stops: *map->getStopList()){
            if(iterator_stops->getId() == text){
                setStopStatus(iterator_stops);
            }
        }
    }
}


/**
 * @brief Edits congestion
 * @param text Levele of congestion
 */
void MainWindow::congestionEdit(QString text)
{
    if (text == "Normal"){
        lastSelectedStreet->setCongestionLevel(0);
        if (ui->Show_congestion_check->isChecked())
            lastSelectedStreet->getLineHandle()->setPen(QPen(QColor(0x99ff33), 5)); // green

    } else if (text == "Slight") {
        lastSelectedStreet->setCongestionLevel(1);
        if (ui->Show_congestion_check->isChecked())
            lastSelectedStreet->getLineHandle()->setPen(QPen(QColor(0xffff66), 5)); // yellow

    } else if (text == "Hard"){
        lastSelectedStreet->setCongestionLevel(2);
        if (ui->Show_congestion_check->isChecked())
            lastSelectedStreet->getLineHandle()->setPen(QPen(QColor(0xff8000), 5)); // orange

    // set text based on current value of street's congestion level
    } else if (text == ""){
        switch (*lastSelectedStreet->getCongestion()) {
            case 0:
                ui->congestionChoiceBox->setCurrentText("Normal");
                break;
            case 1:
                ui->congestionChoiceBox->setCurrentText("Slight");
                break;
            case 2:
                ui->congestionChoiceBox->setCurrentText("Hard");
                break;
        }
    }

}


/**
 * @brief Shows timetable of line.
 */
void MainWindow::timetableShow()
{
    if (lastSelectedStop != nullptr){
        Timetable *timetable = new Timetable(this);
        timetable->stopTimetable(lastSelectedStop, map);
        timetable->setWindowTitle("Stop "+*lastSelectedStop->getId());
        timetable->show();

    }
}


/**
 * @brief Highlightes streets based on the level of congestion.
 * @param x Check box val
 */
void MainWindow::show_congestion_click(int x)
{
    if(x == 2){

        for (auto street : *map->getStreetList()){

            // if street is currently selected, don't change color and continue
            if (street == lastSelectedStreet){
                continue;
            }

            switch(*street->getCongestion()){
            case 0:
                street->getLineHandle()->setPen(QPen(QColor(0x99ff33), 5)); // green
                break;
            case 1:
                street->getLineHandle()->setPen(QPen(QColor(0xffff66), 5)); // yellow
                break;
            case 2:
                street->getLineHandle()->setPen(QPen(QColor(0xff8000), 5)); // orange
                break;
            case 3:
                street->getLineHandle()->setPen(QPen(QColor(0xff6666), 5)); // red
                break;
            }
        }

    //
    } else if (x == 0){

        for (auto street : *map->getStreetList()){

            // if street is currently selected, don't change color and continue
            if (street == lastSelectedStreet){
                continue;
            }

            street->getLineHandle()->setPen(QPen(Qt::black, 5));
        }
    }

}


/**
 * @brief Shows names of streets.
 * @param x Check box val
 */
void MainWindow::show_str_name_click(int x)
{
    if(x == 2){
        for(auto iterator_street: *map->getStreetList()){
            iterator_street->getTextHandle()->setVisible(true);
        }
    }
    else{
        for(auto iterator_street: *map->getStreetList()){
            iterator_street->getTextHandle()->setVisible(false);
        }
    }
}


/**
 * @brief Pops up a window with information about the program
 */
void MainWindow::help_task()
{
    help_window = new Help_window(this);
    help_window->setWindowTitle("Help");
    help_window->show();
}


/**
 * @brief Method that takes care about several tasks every tick of timer
 */
void MainWindow::update_time()
{
    // update location of all transport lines and their vehicles
    for (auto transportLine : *map->getTransportLineList()){
        for (auto vehicle : *transportLine->getVehicleList()){

            // check whether vehicle has been deployed, if so, calculate new position
            if (vehicle->isDeployed()){
                vehicle->calculateTrajectory();
            // vehicle is hidden, check deployment time
            } else {
                // if it's time for vehicle to deploy, calculate new position + add vehicle into line selector
                if (vehicle->checkDeployTime()){
                    vehicle->calculateTrajectory();
                    ui->Line_selector->addItem(vehicle->getFullSignature());
                }
            }
        }

        // update line's status info if selected
        if (lastSelectedLine != nullptr){
            setLineStatus(lastSelectedLine);
        }
    }

    // update simulated time
    time->updateTime();
    ui->Time_counter->setText(time->getAppTime()->toString("hh:mm:ss"));
}

/*!
 * \brief Creates working canvas \n
 * Method creates the working canvas, loads all information from JSon file. Initializes the scene.
 */
void MainWindow::initScene(){   

    auto *scene = new MyScene(ui->graphicsView, map, this);
    //MyScene *scene = new MyScene(ui->graphicsView, map, this);

    ui->graphicsView->setScene(scene);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    // draw streets
    for(auto iterator_street_draw : *map->getStreetList()){

        //draws street in scene
        auto line = scene->addLine(*iterator_street_draw);
        line->setPen(QPen({Qt::black}, 5));

        //draws street's name in scene
        auto text = scene->addText(*iterator_street_draw->getId());
        text->setPos((iterator_street_draw->p1()+iterator_street_draw->p2())/2);
        text->setVisible(false);

        if(iterator_street_draw->p1().x() == iterator_street_draw->p2().x()){
            // vertical text adjustment
            text->setRotation(-90);
            text->setPos(text->pos().x()+10, text->pos().y()+(text->toPlainText().length()*4));
        }
        else{
            // horizontal text adjustment
            text->setPos(text->pos().x()-(text->toPlainText().length()*4), text->pos().y()-40);
        }


        iterator_street_draw->setTextHandle(text);
        iterator_street_draw->setLineHandle(line);
    }

    //draw stops
    for (auto stop: *map->getStopList()){
        auto rect = scene->addRect(*stop);
        rect->setBrush(QBrush(QColor(11, 181, 25)));
        stop->stopMarkHandle = rect;
    }

    //draw vehicles
    for (auto transportLine : *map->getTransportLineList()){
        auto transportLineColor = randomColors[qrand()%12];
        for (auto vehicle : *transportLine->getVehicleList()){
            auto *mark = new QGraphicsEllipseItem(*vehicle);
            mark->setBrush(transportLineColor);
            scene->addItem(mark);
            vehicle->vehicleMark = mark;
            vehicle->setTimeHandle(time); // time handle

            // add line identifier text
            vehicle->vehicleText = new QGraphicsTextItem();
            vehicle->vehicleText->setPlainText(vehicle->getLineNumber());

            if (vehicle->getLineNumber().toInt() < 10){
                 vehicle->vehicleText->setPos(vehicle->x, vehicle->y);
            } else if (vehicle->getLineNumber().toInt() < 100){
                vehicle->vehicleText->setPos(vehicle->x-12.5, vehicle->y-13);
            }

            scene->addItem(vehicle->vehicleText);

            // hide vehicle for now
            vehicle->vehicleMark->setVisible(false);
            vehicle->vehicleText->setVisible(false);

        }
    }

    /* Pozn. Line_selector je naplňován postupně při úspěchu metody deploymentCheck() */

    //Naplni Street_selector vsemi ulicemi
    for(auto iterator_street: *map->getStreetList()){
        ui->Street_selector->addItem(*iterator_street->getId());
    }
    //Naplni Stop_selector vsemi zastavkami
    for(auto iterator_stop: *map->getStopList()){
        ui->Stop_selector->addItem(*iterator_stop->getId());
    }

    // fill congestion choice box
    ui->congestionChoiceBox->addItem("");
    ui->congestionChoiceBox->addItem("Normal");
    ui->congestionChoiceBox->addItem("Slight");
    ui->congestionChoiceBox->addItem("Hard");

    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
}

