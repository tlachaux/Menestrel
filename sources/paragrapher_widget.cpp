#include "paragrapher_widget.h"

#include <QGridLayout>

ParagrapherWidget::ParagrapherWidget(QFont const & font, QString text)
    : _text_wgt     (new SmartTextWidget(font))
    , _add_btn      (new QPushButton("+"))
    , _del_btn      (new QPushButton("-"))
    , _hide_btn     (new QPushButton("^"))
    , _counter_lbl  (new QLabel())
{
    // Widget

    this->setStyleSheet("background-color : rgb(44, 62, 70);");

    _counter_lbl->setContentsMargins(10, 5, 10, 5);

    // Margin

    auto vlt = new QVBoxLayout;
    auto margin = new QWidget;

    margin->setFixedWidth(130);

    vlt->addWidget(_counter_lbl);
    vlt->insertStretch(1);
    vlt->addWidget(_add_btn);
    vlt->addWidget(_del_btn);

    margin->setLayout(vlt);
    margin->setStyleSheet("QPushButton {background-color : rgb(14, 32, 40); border-radius: 10px}\
                          QPushButton:pressed {background-color: rgb(04, 12, 30)}\
                          QPushButton:hover {font-size: 20px; color: rgb(161, 180, 192)}\
                          QLabel {color : rgb(151, 170, 182); border: 0px solid rgb(151, 170, 182);\
                          border-radius: 5px; font-size: 14px;}");


    // Main layout

    auto lt = new QGridLayout;
    lt->setContentsMargins(20, 0, 0, 0);
    lt->setSpacing(20);
    lt->addWidget(_text_wgt, 0, 0, 9, 10);
    lt->addWidget(margin, 0, 10, 10, 1);

    this->setLayout(lt);

    // Connections

    connect(_add_btn, &QPushButton::clicked, this, [this]()
    {
        emit addParagrapherSignal();
    });

    connect(_del_btn, &QPushButton::clicked, this, [this]()
    {
        emit deleteParagrapherSignal();
    });

    connect(_text_wgt, &TextWidget::textChanged, this, [this]()
    {
        _modified = true;
        emit updateParagrapherSignal();
    });

    connect(_hide_btn, &QPushButton::clicked, this, [this]()
    {
        if (_text_wgt->isVisible())
        {
            _hide_btn->setText("v");
            _add_btn->setVisible(false);
            _del_btn->setVisible(false);
            _text_wgt->setVisible(false);
        }
        else
        {
            _hide_btn->setText("^");
            _add_btn->setVisible(true);
            _del_btn->setVisible(true);
            _text_wgt->setVisible(true);
        }
    });

    this->setText(text);
}

void ParagrapherWidget::setText(QString text)
{
    _text_wgt->setText(text);
    this->updateStats();
    //_text_wgt->checkOrthograph();
}

QString ParagrapherWidget::text()
{
    return _text_wgt->toPlainText();
}

void ParagrapherWidget::format()
{
    if (!_formated)
    {
        _text_wgt->setTextFormat();
        _formated = true;
    }
}

void ParagrapherWidget::updateOrthograph()
{
    _text_wgt->cleanUnderlined();
    _text_wgt->checkOrthograph();
}

int ParagrapherWidget::updateStats()
{
    int counter = _text_wgt->countLetters();
    _counter_lbl->setText("Chars : " + QString::number(counter));
    return counter;
}

bool ParagrapherWidget::isModified()
{
    if (_modified)
    {
        _modified = false;
        return true;
    }

    return false;
}
