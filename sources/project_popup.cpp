#include "project_popup.h"

#include <QApplication>
#include <QGuiApplication>
#include <QLabel>
#include <QScreen>
#include <QVBoxLayout>
#include <QDesktopWidget>
#include <iostream>
#include <QKeyEvent>

ProjectPopup::ProjectPopup()
    : _text_line(new QLineEdit)
{   
    /* -----------------------------------
     *  Window
     * -------------------------------- */

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    int height = screenGeometry.height();
    int width = screenGeometry.width();

    this->move(width/2 - 250, height/2 - 100);
    this->setModal(true);

    this->setFixedSize(500, 100);
    this->setStyleSheet("border: 0px; background-color: rgb(21, 36, 43); color: rgb(151, 170, 182); font-size: 18px");
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    /* -----------------------------------
     *  Project label
     * -------------------------------- */

    auto lb = new QLabel("Project : ");
    auto lt = new QVBoxLayout;
    lt->addWidget(lb);
    lt->addWidget(_text_line);
    this->setLayout(lt);

    _text_line->setStyleSheet("background-color: rgb(151, 170, 182); color: rgb(29, 47, 55); font-size: 18px");
}

/**
 * @brief ProjectPopup::keyPressEvent
 * @param e
 */
void ProjectPopup::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Return)
    {
        if (!_text_line->text().isEmpty())
        {
            this->close();
            emit openProjectSignal(_text_line->text());
        }
    }
    else if (e->key() == Qt::Key_Escape)
    {
        this->close();
        emit killApplication();
    }
}
