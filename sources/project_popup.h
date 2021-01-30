#ifndef PROJECTPOPUP_H
#define PROJECTPOPUP_H

#include <QLineEdit>
#include <QDialog>

class ProjectPopup : public QDialog
{
    Q_OBJECT
public:
    ProjectPopup();

    void keyPressEvent(QKeyEvent * e);

private:
    QLineEdit * _text_line;

signals:
    void openProjectSignal(QString path);
    void killApplication();
};

#endif // PROJECTPOPUP_H
