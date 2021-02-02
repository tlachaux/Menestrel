#include "paragraph_widget.h"

#include <QGridLayout>

ParagraphWidget::ParagraphWidget(QFont const & font, QString text, bool locked)
    : _text_wgt     (new SmartTextWidget(font))
    , _add_btn      (new QPushButton("Insert"))
    , _lock_btn     (new QPushButton(locked ? "Locked" : "Unlocked"))
    , _del_btn      (new QPushButton("Remove"))
    , _correct_btn  (new QPushButton("Correct"))
    , _counter_lbl  (new QLabel())
{
    /* -----------------------------------
     *  This
     * -------------------------------- */

    this->setStyleSheet ("background-color : rgb(44, 62, 70);");
    this->setText       (text);

    /* -----------------------------------
     *  Text widget
     * -------------------------------- */

    _text_wgt->setReadOnly(locked);

    /* -----------------------------------
     *  Counter label
     * -------------------------------- */

    _counter_lbl->setContentsMargins(10, 5, 10, 5);

    /* -----------------------------------
     *  Margin menu
     * -------------------------------- */

    auto vlt    = new QVBoxLayout;
    auto margin = new QWidget;

    margin->setFixedWidth(130);

    _add_btn->setMinimumHeight  (25);
    _lock_btn->setMinimumHeight (25);
    _correct_btn->setMinimumHeight (25);
    _del_btn->setMinimumHeight  (25);
    _del_btn->setDisabled       (locked);

    if (!locked)
    {
        _lock_btn->setStyleSheet("border : 1px solid rgb(230, 230, 230); color: rgb(230, 230, 230)");
    }

    vlt->addWidget(_counter_lbl);
    vlt->insertStretch(1);
    vlt->addWidget(_correct_btn);
    vlt->addWidget(_lock_btn);
    vlt->addWidget(_del_btn);
    vlt->addWidget(_add_btn);

    margin->setLayout(vlt);
    margin->setStyleSheet("QPushButton {background-color : rgb(14, 32, 40); border-radius: 10px; font-size: 12px}\
                          QPushButton:pressed {background-color: rgb(04, 12, 30)}\
                          QPushButton:hover {font-size: 16px; color: rgb(161, 180, 192)}\
                          QPushButton:disabled {background-color: rgb(30, 48, 58);}  \
                          QLabel {color : rgb(151, 170, 182); border: 0px solid rgb(151, 170, 182);\
                          border-radius: 5px; font-size: 12px;}");


    /* -----------------------------------
     *  Main layout
     * -------------------------------- */

    auto lt = new QGridLayout;
    lt->setContentsMargins(20, 0, 0, 0);
    lt->setSpacing(20);
    lt->addWidget(_text_wgt, 0, 0, 9, 10);
    lt->addWidget(margin, 0, 10, 10, 1);

    this->setLayout(lt);

    /* -----------------------------------
     *  Buttons
     * -------------------------------- */

    connect(_add_btn, &QPushButton::clicked, this, [this]()
    {
        emit addParagraphSignal();
    });

    connect(_del_btn, &QPushButton::clicked, this, [this]()
    {
        emit deleteParagraphSignal();
    });

    connect(_text_wgt, &TextWidget::textChanged, this, [this]()
    {
        _modified = true;

        static QString text = _text_wgt->toPlainText();

        if (_correction && !_correction_running && _text_wgt->toPlainText() != text)
        {
            text = _text_wgt->toPlainText();
            _text_wgt->cleanUnderlined();
            _text_wgt->checkOrthograph();
        }

        emit updateParagraphSignal();
    });

    connect(_lock_btn, &QPushButton::clicked, this, [this]()
    {
        bool locked = _lock_btn->text() != "Locked";

        if (locked)
        {
            _lock_btn->setStyleSheet("");
            _lock_btn->setText("Locked");
            _text_wgt->setReadOnly(true);
            _del_btn->setDisabled(true);

        }
        else
        {
            _lock_btn->setStyleSheet("border : 1px solid rgb(230, 230, 230); color: rgb(230, 230, 230)");
            _lock_btn->setText("Unlocked");
            _text_wgt->setReadOnly(false);
            _del_btn->setDisabled(false);
        }
    });

    connect(_correct_btn, &QPushButton::clicked, this, [this]()
    {
        _correction = !_correction;

        if (_correction)
        {
            _correct_btn->setStyleSheet("border : 1px solid rgb(230, 230, 230); color: rgb(230, 230, 230)");
            _text_wgt->checkOrthograph();
        }
        else
        {
            _correct_btn->setStyleSheet("");
            _text_wgt->cleanUnderlined();
        }
    });
}

/**
 * @brief ParagraphWidget::setText
 * @param text
 */
void ParagraphWidget::setText(QString text)
{
    _text_wgt->setText(text);
    this->updateStats();
}

/**
 * @brief ParagraphWidget::text
 * @return
 */
QString ParagraphWidget::text()
{
    return _text_wgt->toPlainText();
}

/**
 * @brief ParagraphWidget::format
 */
void ParagraphWidget::format()
{
    if (!_formated)
    {
        _text_wgt->setTextFormat();
        _formated = true;
    }
}

/**
 * @brief ParagraphWidget::updateOrthograph
 */
void ParagraphWidget::updateOrthograph()
{
    _text_wgt->cleanUnderlined();
    _text_wgt->checkOrthograph();
}

/**
 * @brief ParagraphWidget::updateStats
 * @return
 */
int ParagraphWidget::updateStats()
{
    int counter = _text_wgt->countLetters();
    _counter_lbl->setText("Chars : " + QString::number(counter));

    return counter;
}

/**
 * @brief ParagraphWidget::isModified
 * @return
 */
bool ParagraphWidget::isModified()
{
    if (_modified)
    {
        _modified = false;
        return true;
    }

    return false;
}
