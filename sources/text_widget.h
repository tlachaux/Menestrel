#ifndef CUSTOMTEXTEDIT_H
#define CUSTOMTEXTEDIT_H

#include <QTextEdit>
#include <iostream>

class TextWidget : public QTextEdit
{
public:
    TextWidget(QFont font, QString text="");

public slots:
    void deselectAll            ();
    void setTextFormat          ();
    void setLineSpacement       (int space);

    void highlightWord          (QString word, QColor color);
    void underlineWord          (QString word, QColor color);

    void cleanHighligthed       ();
    void cleanUnderlined        ();

    void cleanHighligthedCursor (QTextCursor cursor);
    void cleanUnderlinedCursor  (QTextCursor cursor);

    int  countLetters           ();

protected:
    QVector<QTextCursor>    _highlighted_cursors;
    QVector<QTextCursor>    _underlined_cursors;
};

#endif // CUSTOMTEXTEDIT_H
