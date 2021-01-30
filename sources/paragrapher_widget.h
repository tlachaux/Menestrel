#ifndef PARAGRAPHERWIDGET_H
#define PARAGRAPHERWIDGET_H

#include "orthograph_corrector.h"
#include "smart_text_widget.h"
#include "text_widget.h"

#include <QLabel>
#include <QPushButton>

class ParagrapherWidget : public QWidget
{
    Q_OBJECT

public:
    ParagrapherWidget           (QFont const & font, QString text="");

    void setText                (QString text);
    QString text                ();
    void format                 ();
    void updateOrthograph        ();
    int  updateStats            ();
    bool isModified             ();

signals:
    void addParagrapherSignal   ();
    void deleteParagrapherSignal();
    void updateParagrapherSignal();

private:
    SmartTextWidget *   _text_wgt;
    QPushButton *       _add_btn;
    QPushButton *       _del_btn;
    QPushButton *       _hide_btn;
    QLabel *            _counter_lbl;

    bool                _formated = false;
    bool                _modified = false;
};

#endif // PARAGRAPHERWIDGET_H
