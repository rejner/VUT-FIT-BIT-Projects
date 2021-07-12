/*!
 * @file
 * @brief This file contains class that represents timetable
 *
 * @author Michal Rein, Ondřej Motyčka
 */

#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <QDialog>
#include "model/vehicle.h"
#include "model/streetmap.h"
#include <QLabel>

namespace Ui {
class Timetable;
}

/**
 * @brief This class represents Timetable class
 */
class Timetable : public QDialog
{
    Q_OBJECT

public:
    explicit Timetable(QWidget *parent = nullptr);
    void vehicleTimetable(Vehicle *vehicle);
    void stopTimetable(Stop *stop, StreetMap *map);
    ~Timetable();

private:
    Ui::Timetable *ui;
};

#endif // TIMETABLE_H
