#ifndef PARAGRAPH_WIDGET_H
#define PARAGRAPH_WIDGET_H

#include "orthograph_corrector.h"
#include "smart_text_widget.h"
#include "text_widget.h"

#include <QLabel>
#include <QPushButton>

class ParagraphWidget : public QWidget
{
    Q_OBJECT

public:
    ParagraphWidget             (QFont const & font, QString text="", bool locked=true);

    void setText                (QString text);
    QString text                ();
    void format                 ();
    void updateOrthograph       ();
    int  updateStats            ();
    bool isModified             ();

signals:
    void addParagraphSignal   ();
    void deleteParagraphSignal();
    void updateParagraphSignal();

private:
    SmartTextWidget *   _text_wgt;
    QPushButton *       _add_btn;
    QPushButton *       _lock_btn;
    QPushButton *       _del_btn;
    QPushButton *       _correct_btn;
    QLabel *            _counter_lbl;

    bool                _formated   = false;
    bool                _modified   = false;
    bool                _correction = false;
    bool                _correction_running = false;
};

#endif // PARAGRAPH_WIDGET_H
