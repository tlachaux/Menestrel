#include "main_window.h"
#include "project_popup.h"

#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    ProjectPopup p;

    w.connect(&p, &ProjectPopup::openProjectSignal, &w, &MainWindow::openProject);
    w.connect(&p, &ProjectPopup::killApplication, &w, &MainWindow::close);

    w.showFullScreen();
    p.show();

    return a.exec();
}
