/*!
 * @file
 * @brief This file contains class that represents myscene
 *
 * @author Michal Rein, Ondřej Motyčka
 */

#ifndef MYSCENE_H
#define MYSCENE_H

#include <QGraphicsScene>
#include "model/streetmap.h"
#include "model/stop.h"
#include "model/street.h"
#include <QGraphicsLineItem>
#include <QGridLayout>
#include "mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief This class represents MyScene class
 */
class MyScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit MyScene(QObject *parent = nullptr, StreetMap *map = nullptr, MainWindow *mainWindow = nullptr);
    StreetMap* mapHandle;
    MainWindow *mainWindowHandle;

private:
    QGraphicsLineItem* lastSelectedStreet = nullptr;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // MYSCENE_H
