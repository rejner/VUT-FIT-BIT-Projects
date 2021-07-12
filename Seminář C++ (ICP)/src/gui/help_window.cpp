/*!
 * @file
 * @brief This file contains implementation of help_window
 *
 * @author Michal Rein, Ondřej Motyčka
 */

#include "help_window.h"
#include "ui_help_window.h"
#include <QString>

/**
 * @brief Constructor of help_window \n
 * Makes a new popup window with help.
 * @param parent Parent widget
 */
Help_window::Help_window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Help_window)
{
    QString a = ("Welcome to our app!\n"
                 "Here you will find all the information needed for operating this application!\n\n"
                 "----------------\n"
                 " CONTROLS\n"
                 "----------------\n"
                 " - Entire map can be moved by left clicking on the map's surface and dragging it into desired direction.\n"
                 " - All entities (streets/vehicles/stops) are clickable objects. Clicking on these objects will select them and display\n"
                 "   information about them on the status bar, which is located in the left part of the app.\n"
                 " - Selection of entities can be undone by right click on the map.\n"
                 " - Map can also be zoomed by moving the slide bar on the right.\n\n"
                 "------------------\n"
                 "  STATUS BAR\n"
                 "------------------\n"
                 " - Here, you can find all information that you need.\n"
                 " - It splits into 5 sections:\n"
                 "       Time:\n"
                 "           - Time section hold information about application simulated time.\n"
                 "           - You can speed up pace of time by selecting one of preset modifiers from the choice box.\n"
                 "       Vehicle:\n"
                 "           - Here you will see info about a selected vehicle.\n"
                 "           - Vehicle can be selected either by clicking it directly on the map or from choice box.\n"
                 "           - If you want to view vehicle's time table, just click on 'Show route' button!\n"
                 "       Street:\n"
                 "           - Displays information about selected street.\n"
                 "       Stop:\n"
                 "           - Displays information about selected stop.\n"
                 "       Features:\n"
                 "           - Contains check boxes for turning on/off some special features.\n\n\n"
                 "---------------------------------------\n"
                 "  MAP REPRESENTATION FORMAT\n"
                 "---------------------------------------\n"
                 " - To show a custom map and lines, map representation must be created using JSON format including certain objects.\n"
                 " - Every map representation should be structured like this sample:\n\n"
                 "{\n"
                 "    \"streets\": [\n"
                 "        {\n"
                 "            \"streetName\": \"Example street\",\n"
                 "            \"begin\": [ 100, 100 ],\n"
                 "            \"end\": [ 500, 100 ],\n"
                 "            \"congestion\": 2,\n"
                 "            \"stops\": [\n"
                 "                {\n"
                 "                    \"stopName\": \"Example stop 1\",\n"
                 "                    \"coordinates\": [ 200, 100 ]\n"
                 "                },\n"
                 "                {\n"
                 "                    \"stopName\": \"Example stop 2\",\n"
                 "                    \"coordinates\": [ 400, 100 ]\n"
                 "                }\n"
                 "            ]\n"
                 "        }\n"
                 "     ],\n"
                 "    \"lines\": [\n"
                 "        {\n"
                 "            \"lineNumber\": \"42\",\n"
                 "            \"route\": [\n"
                 "                {\n"
                 "                    \"streetName\": \"Example street\",\n"
                 "                    \"stops\": [ \"Example stop 1\", \"Example stop 2\" ]\n"
                 "                }\n"
                 "            ],\n"
                 "            \"vehicles\": [\n"
                 "                {\n"
                 "                    \"id\": 101,\n"
                 "                    \"deploy\": \"00:00:10\"\n"
                 "                },\n"
                 "                {\n"
                 "                    \"id\": 102,\n"
                 "                    \"deploy\": \"00:00:30\"\n"
                 "                }\n"
                 "            ]\n"
                 "         }\n"
                 "      ]\n"
                 "}\n\n"
                 " - This example will create a street with the beginning coordinates at [100, 100] and end at [500, 100], which\n"
                 "   has level of congestion set to 2 (vehicles will move 4x slower) and contains 2 stops (Example stop 1 and Example stop 2).\n"
                 " - Also, this representation contains another objects with information about lines. Each defined line has it's line number,\n"
                 "   route, which is described only by street and stop names (no coordinates needed, since all stops were already created in\n"
                 "   streets object) and array of vehicles, where each vehicle has it's own identifier and time of deployment, that\n"
                 "   has to be given in HH:MM:SS time format.\n"
                 "\n\n Authors: Michal Rein and Ondřej Motyčka\n");
    ui->setupUi(this);
    ui->Help->setText(a);

}

/**
 * @brief Destructor of class Help_window
 */
Help_window::~Help_window()
{
    delete ui;
}
