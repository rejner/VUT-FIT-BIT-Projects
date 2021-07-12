/*!
 * @file
 * @brief This file contains class that represents intro
 *
 * @author Michal Rein, Ondřej Motyčka
 */

#ifndef INTRO_H
#define INTRO_H

#include <QDialog>
#include "model/streetmap.h"

namespace Ui {
class intro;
}

/**
 * @brief This class represents intro class
 */
class intro : public QDialog
{
    Q_OBJECT

public:
    explicit intro(QWidget *parent = nullptr, StreetMap *map = nullptr);
    ~intro();

private slots:
    void example_1();

private:
    Ui::intro *ui;
    StreetMap *map;
};

#endif // INTRO_H
