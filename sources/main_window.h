#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "paragraph_widget.h"
#include "text_widget.h"
#include "orthograph_corrector.h"

#include <QMainWindow>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QDir>
#include <QTextStream>
#include <QPushButton>
#include <QLineEdit>
#include <QKeyEvent>
#include <QDate>
#include <QTimer>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow                  (QWidget *parent = nullptr);
    ~MainWindow                 ();

    virtual void keyPressEvent  (QKeyEvent * event);

public slots:
    void openProject            (QString project);
    void loadChapter            (int number);
    void addParagraph           (bool locked=true);
    void addChapter             ();
    void insertParagraph        (ParagraphWidget * widget);
    void removeParagraph        (ParagraphWidget * widget);
    void quickSave              ();
    void save                   (QString title);
    void countOccurences        ();
    void displayOccurences      ();
    void changeCurrentChapter   (int number);

private:
    void connectParagraph       (ParagraphWidget * widget);
    void clearAllParagraph      ();

    QVBoxLayout *               _dynamic_paragraph_lt;
    QList<ParagraphWidget *>    _paragraphs;
    QVBoxLayout *               _dynamic_chapter_lt;
    QList<QPushButton *>        _chapters;
    TextWidget *                _novel_title_wgt;

    QDir                        _home;
    QDir                        _project;

    QFont                       _font;

    bool                        _text_changed = false;
    QTimer                      _timer;

    int                         _current_chapter = 1;

    QMap<QString, int>          _occurences;
};
#endif // MAINWINDOW_H
