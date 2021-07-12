/*!
 * @file
 * @brief This file contains class that represents mainwindow
 *
 * @author Michal Rein, Ondřej Motyčka
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QTime>
#include <QMainWindow>
#include <QComboBox>
#include <QGraphicsEllipseItem>
#include "help_window.h"
#include "model/streetmap.h"
#include "timetable.h"
#include "model/time.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief This class represents MainWindow class
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Ui::MainWindow *ui;

    void setStreetStatus(Street *street);
    void setStopStatus(Stop *stop);
    void setLineStatus(Vehicle *vehicle);

    Time* time = nullptr;
    Street* lastSelectedStreet = nullptr;
    Stop* lastSelectedStop = nullptr;
    Vehicle* lastSelectedLine = nullptr;
    QColor randomColors[12] = { QColor(217, 115, 26), QColor(217, 176, 26), QColor(137, 26, 217), QColor(194, 41, 133), QColor(194, 41, 74),QColor(194, 41, 41),QColor(217, 26, 106),
                             QColor(217, 198, 26),QColor(217, 118, 26),QColor(217, 26, 26),QColor(219, 165, 57),QColor(57, 79, 219)};

    bool editRouteMode = false;
    QList<Street *> *editRouteStreets = new QList<Street*>;
    QList<Stop *> *editRouteStops = new QList<Stop*>;

private slots:
    void zoom(int z);
    void update_time();
    void show_route_clicked();
    void editRoute();
    void saveEditRoute();
    void cancelEditRoute();
    void speed_selector(int val);
    void line_selector(QString text);
    void street_selector(QString text);
    void stop_selector(QString text);
    void congestionEdit(QString text);
    void timetableShow();
    void show_congestion_click(int x);
    void show_str_name_click(int x);
    void help_task();

private:
    void initScene();
    Help_window *help_window;
    Timetable *timetable_window;
    QTime *my_time;
    QTimer *timer;
    StreetMap *map;

};
#endif // MAINWINDOW_H
