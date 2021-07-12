/*!
 * @file
 * @brief This file contains class that represents help_window
 *
 * @author Michal Rein, Ondřej Motyčka
 */

#ifndef HELP_WINDOW_H
#define HELP_WINDOW_H

#include <QDialog>

namespace Ui {
class Help_window;
}

/**
 * @brief This class represents Help_window class
 */
class Help_window : public QDialog
{
    Q_OBJECT

public:
    explicit Help_window(QWidget *parent = nullptr);
    ~Help_window();

private:
    Ui::Help_window *ui;
};

#endif // HELP_WINDOW_H
