/*!
 * @file
 * @brief This file contains implementation of intro
 *
 * @author Michal Rein, Ondřej Motyčka
 */
#include "intro.h"
#include "ui_intro.h"

/**
 * @brief Constructor of class intro
 * @param parent Parent widget
 * @param map Street map
 */
intro::intro(QWidget *parent,StreetMap *map) :
    QDialog(parent),
    ui(new Ui::intro)
{
    this->map = map;
    ui->setupUi(this);
    connect(ui->ref_1, &QPushButton::clicked, this, &intro::example_1);
}

/**
 * @brief Destructor of class intro
 */
intro::~intro()
{
    delete ui;
}

/**
 * @brief Example 1
 */
void intro::example_1()
{
    map->createReferenceMap(":/resources/examples/example.json");
    intro::close();
}
