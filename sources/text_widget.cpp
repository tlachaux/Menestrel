#include "text_widget.h"
#include <iostream>
#include <QTextBlock>

TextWidget::TextWidget(QFont font, QString text)
    : QTextEdit(text)
{
    this->setFont(font);
    this->setAcceptRichText(true);
    this->setAlignment(Qt::AlignJustify);
    this->setStyleSheet("background-color: rgb(24, 42, 50); selection-background-color: rgb(44, 62, 70); font-size: 24px");
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setLineSpacement(10);

    connect(this, &QTextEdit::textChanged, this, [this]()
    {
        document()->setTextWidth(700);
        this->setFixedSize(document()->size().width(), document()->size().height());
    });

    emit textChanged();
}

/**
 * @brief TextWidget::deselectAll
 */
void TextWidget::deselectAll()
{
    QTextCursor c = this->textCursor();
    c.clearSelection();
    this->setTextCursor(c);
}

/**
 * @brief TextWidget::setTextFormat
 */
void TextWidget::setTextFormat()
{
    this->selectAll();
    this->setAlignment(Qt::AlignJustify);
    this->setLineSpacement(10);
    this->deselectAll();
}

/**
 * @brief TextWidget::setLineSpacement
 * @param space
 */
void TextWidget::setLineSpacement(int space)
{
    QTextBlockFormat bf = this->textCursor().blockFormat();
    bf.setLineHeight(space, QTextBlockFormat::LineDistanceHeight) ;
    this->textCursor().setBlockFormat(bf);
}

/**
 * @brief TextWidget::highlightWord
 * @param word
 * @param color
 */
void TextWidget::highlightWord(QString word, QColor color)
{
    QTextCursor cursor(this->document());

    while (!cursor.isNull())
    {
        cursor = this->document()->find(word, cursor);

        QTextCharFormat tf = cursor.charFormat();
        tf.setBackground(QBrush(color));
        cursor.setCharFormat(tf);
    }
}

/**
 * @brief TextWidget::underlineWord
 * @param word
 * @param color
 */
void TextWidget::underlineWord(QString word, QColor color)
{
    QTextCursor cursor(this->document());

    while (!cursor.isNull())
    {
        cursor = this->document()->find(word, cursor);
        _underlined_cursors.push_back(cursor);

        QTextCharFormat tf = cursor.charFormat();
        tf.setFontUnderline(true);
        tf.setUnderlineColor(color);
        cursor.setCharFormat(tf);
    }
}

/**
 * @brief TextWidget::cleanHighligthed
 */
void TextWidget::cleanHighligthed()
{
    for (auto cursor : _highlighted_cursors)
    {
        QTextCharFormat tf = cursor.charFormat();
        tf.setBackground(QBrush());
        cursor.setCharFormat(tf);
    }

    _highlighted_cursors.clear();
}

/**
 * @brief TextWidget::cleanUnderlined
 */
void TextWidget::cleanUnderlined()
{
    for (auto cursor : _underlined_cursors)
    {
        QTextCharFormat tf = cursor.charFormat();
        tf.setFontUnderline(false);
        cursor.setCharFormat(tf);
    }

    _underlined_cursors.clear();
}

/**
 * @brief TextWidget::cleanHighligthedCursor
 * @param cursor
 */
void TextWidget::cleanHighligthedCursor(QTextCursor cursor)
{
    QTextCharFormat tf = cursor.charFormat();
    tf.setBackground(QBrush());
    cursor.setCharFormat(tf);
}

/**
 * @brief TextWidget::cleanUnderlinedCursor
 * @param cursor
 */
void TextWidget::cleanUnderlinedCursor(QTextCursor cursor)
{
    QTextCharFormat tf = cursor.charFormat();
    tf.setFontUnderline(false);
    cursor.setCharFormat(tf);
}

/**
 * @brief TextWidget::countLetters
 * @return
 */
int TextWidget::countLetters()
{
    QString text = this->toPlainText();
    int counter = 0;

    for (QChar c : text)
    {
        if (c.isLetter()) counter++;
    }

    return counter;
}
