/********************************************************************************
** Form generated from reading UI file 'timetable.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TIMETABLE_H
#define UI_TIMETABLE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_timetable
{
public:
    QVBoxLayout *verticalLayout_2;
    QScrollArea *scrollArea;
    QWidget *insert_here;
    QVBoxLayout *verticalLayout;

    void setupUi(QDialog *timetable)
    {
        if (timetable->objectName().isEmpty())
            timetable->setObjectName(QStringLiteral("timetable"));
        timetable->resize(464, 537);
        verticalLayout_2 = new QVBoxLayout(timetable);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        scrollArea = new QScrollArea(timetable);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        insert_here = new QWidget();
        insert_here->setObjectName(QStringLiteral("insert_here"));
        insert_here->setGeometry(QRect(0, 0, 444, 517));
        verticalLayout = new QVBoxLayout(insert_here);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        scrollArea->setWidget(insert_here);

        verticalLayout_2->addWidget(scrollArea);


        retranslateUi(timetable);

        QMetaObject::connectSlotsByName(timetable);
    } // setupUi

    void retranslateUi(QDialog *timetable)
    {
        timetable->setWindowTitle(QApplication::translate("timetable", "Dialog", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Timetable: public Ui_timetable {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TIMETABLE_H
