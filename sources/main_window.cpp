#include "main_window.h"

#include <QFontDatabase>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <iostream>
#include <QScrollArea>
#include <QDate>
#include <QAction>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow           (parent)
    , _dynamic_paragraph_lt (new QVBoxLayout)
    , _dynamic_chapter_lt   (new QVBoxLayout)
{
    // Directory -------------

    _home = QDir(QDir::homePath() + "/Menestrel");

    if (!_home.exists()) QDir().mkdir(QDir::homePath() + "/Menestrel");

    // Window -------------

    this->setMinimumSize(QSize(1000, 800));
    this->setStyleSheet("border: 0px; background-color: rgb(29, 47, 55); color: rgb(151, 170, 182); font-size: 16px");

    // Central widget -------------

    auto lt = new QGridLayout;
    lt->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    lt->setContentsMargins(0, 20, 0, 0);

    auto central_widget = new QWidget;
    central_widget->setLayout(lt);

    auto scroll_widget = new QScrollArea;
    scroll_widget->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    scroll_widget->setWidget(central_widget);
    scroll_widget->setWidgetResizable(true);
    scroll_widget->setStyleSheet("QScrollBar::sub-line:vertical {border:0px;}\
                                  QScrollBar::add-line:vertical {border:0px;}\
                                  QScrollBar::handle::vertical {border-radius:5px ; background-color:rgb(160, 160, 160)}");

    this->setCentralWidget(scroll_widget);

    // Font

    int id = QFontDatabase::addApplicationFont("../fonts/AppleGaramond.ttf");
    _font  = QFont(QFontDatabase::applicationFontFamilies(id).at(0));

    // Title label -------------

    _project_title_wgt = new TextWidget(_font);
    lt->addWidget(_project_title_wgt, 0, 2, 1, 4);

    _project_title_wgt->setText("...");
    _project_title_wgt->setStyleSheet("font-size: 50px");
    _project_title_wgt->setFixedWidth(880);

    connect(_project_title_wgt, &TextWidget::textChanged, this, [this]() {_text_changed = true;});

    // Chapter view -------------

    _dynamic_chapter_lt->setAlignment(Qt::AlignTop);
    _dynamic_chapter_lt->setContentsMargins(0, 0, 0, 0);

    auto button = new QPushButton("+");
    button->setFixedHeight(40);
    //button->setStyleSheet("QPushButton {font-size: 20px; background-color : rgb(44, 62, 70); border-radius: 10px}\
                          QPushButton:pressed {font-size: 20px; background-color: rgb(54, 72, 80)}\
                          QPushButton:hover {font-size: 25px; color: rgb(161, 180, 192)}\
                          QLabel {color : rgb(151, 170, 182); border: 0px solid rgb(151, 170, 182);\
                          border-radius: 5px; font-size: 14px;}");

    connect(button, &QPushButton::clicked, this, [this](){
        auto new_button = new QPushButton("Chapter " + QString::number(_chapter_counter));
        new_button->setFixedHeight(60);
        _dynamic_chapter_lt->insertWidget(_chapters.size(), new_button);
        _chapters.push_back(new_button);
        _chapter_counter++;
    });

    _dynamic_chapter_lt->addWidget(button);

    auto chapter_wgt = new QWidget;

    chapter_wgt->setLayout(_dynamic_chapter_lt);
    chapter_wgt->setStyleSheet("QPushButton {font-size: 20px; background-color : rgb(14, 32, 40); border-radius: 10px}\
                               QPushButton:pressed {font-size: 20px; background-color: rgb(04, 12, 30)}\
                               QPushButton:hover {font-size: 25px; color: rgb(161, 180, 192)}\
                               QLabel {color : rgb(151, 170, 182); border: 0px solid rgb(151, 170, 182);\
                               border-radius: 5px; font-size: 14px;}");
    chapter_wgt->setFixedWidth(200);

    lt->addWidget(chapter_wgt, 1, 0, 1, 1);

    // Paragraph view -------------

    _dynamic_paragraph_lt->setAlignment(Qt::AlignTop);
    _dynamic_paragraph_lt->setContentsMargins(0, 0, 0, 0);

    auto paragraph_wgt = new QWidget;

    paragraph_wgt->setLayout(_dynamic_paragraph_lt);
    paragraph_wgt->setStyleSheet("background-color: rgb(24, 42, 50); font-size: 18px");
    paragraph_wgt->setFixedWidth(880);

    lt->addWidget(paragraph_wgt, 1, 2, 1, 1);

    connect(&_timer, &QTimer::timeout, this, [this]()
    {
        this->quickSave();
    });

    _timer.start(60000);

    QAction* refresh_occ_shortcut = new QAction(this);
    refresh_occ_shortcut->setShortcut(QKeySequence("Ctrl+R"));

    connect(refresh_occ_shortcut, &QAction::triggered, this, [this]()
    {
        this->countOccurences();
        this->displayOccurences();
    });

    this->addAction(refresh_occ_shortcut);

    QAction* refresh_errors_shortcut = new QAction(this);
    refresh_errors_shortcut->setShortcut(QKeySequence("Ctrl+E"));

    connect(refresh_errors_shortcut, &QAction::triggered, this, [this]()
    {
        int counter = 0;

        for (auto paragrapher : _paragraphers)
        {
            counter += paragrapher->updateStats();

            if (paragrapher->isModified())
            {
                paragrapher->updateOrthograph();
                paragrapher->format();
            }
        }

        std::cout << "Letters : " << counter << std::endl;
    });

    this->addAction(refresh_errors_shortcut);
}

MainWindow::~MainWindow()
{
    this->save("last_version");
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        this->close();
    }
}

void MainWindow::openProject(QString project)
{
    _project = QDir(_home.path() + "/" + project);

    if (!_project.exists()) QDir().mkdir(_home.path() + "/" + project);

    this->loadFile(_project.path() + "/last_version");
}

void MainWindow::loadFile(QString path)
{
    QFile readFile(path);

    if(readFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&readFile);
        QString buffer;

        bool first_line = true;

        while (!stream.atEnd())
        {
           QString line = stream.readLine();

           if (first_line)
           {
               _project_title_wgt->setText(line);
               _project_title_wgt->setTextFormat();
               first_line = false;
           }
           else if (line.isEmpty() && !buffer.isEmpty())
           {
               this->addParagrapher();
               _paragraphers.back()->setText(buffer);
               _paragraphers.back()->format();
               buffer = "";
           }
           else
           {
               if (!buffer.isEmpty()) buffer += "\n";
               buffer += line;
           }
        }

        readFile.close();
    }

    if (_paragraphers.isEmpty()) this->addParagrapher();
}

void MainWindow::addParagrapher()
{
    auto paragrapher = new ParagrapherWidget(_font);

    _paragraphers.append(paragrapher);
    _dynamic_paragraph_lt->addWidget(paragrapher);

    this->connectParagrapher(paragrapher);
}

void MainWindow::insertParagrapher(ParagrapherWidget * widget)
{
    auto paragrapher = new ParagrapherWidget(_font);

    for (int i=0; i < _paragraphers.size(); ++i)
    {
        if (_paragraphers[i] == widget)
        {
            _dynamic_paragraph_lt->insertWidget(i + 1, paragrapher);
            _paragraphers.insert(i + 1, paragrapher);
        }
    }

    this->connectParagrapher(paragrapher);
}

void MainWindow::removeParagrapher(ParagrapherWidget * widget)
{
    _dynamic_paragraph_lt->removeWidget(widget);
    _paragraphers.removeOne(widget);
    delete widget;
}

void MainWindow::quickSave()
{
    if (!_text_changed) return;

    _timer.start(60000);
    _text_changed = false;

    this->save("quick_save_" + QDateTime::currentDateTime().toString());
}

void MainWindow::save(QString title)
{
    QFile save_file(_project.path() + "/" + title);

    if(save_file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        QTextStream stream(&save_file);

        stream << _project_title_wgt->toPlainText() << "\n\n";

        for (auto paragrapher : _paragraphers)
        {
            stream << paragrapher->text() << "\n\n";
        }

        stream.flush();
        save_file.close();
    }
}

void MainWindow::countOccurences()
{
    QRegExp separator("[(,|'|;| |.|/-)]");

    _occurences.clear();

    for (auto paragrapher : _paragraphers)
    {
        for (auto word : paragrapher->text().split(separator))
        {
            if (word.size() > 2)
            {
                if (_occurences.find(word) == _occurences.end())
                {
                    _occurences[word] = 1;
                }
                else
                {
                    _occurences[word]++;
                }
            }
        }
    }
}

void MainWindow::displayOccurences()
{
    int sum = 0;

    for (int i=0; i < _occurences.size(); ++i)
    {
        int max_value       = 0;
        QString max_key     = "";

        for (auto it=_occurences.begin(); it != _occurences.end(); ++it)
        {
            if (it.value() > max_value)
            {
                max_value = it.value();
                max_key = it.key();
            }
        }

        std::cout << max_key.toStdString() << " : " << max_value << std::endl;
        sum += max_value;
        _occurences[max_key] = -1;
    }

    std::cout << "Total words : " << sum << std::endl;
    std::cout << "Total variety : " << _occurences.size() << std::endl;
}

void MainWindow::connectParagrapher(ParagrapherWidget * widget)
{
    connect(widget, &ParagrapherWidget::addParagrapherSignal, this, [this, widget]()
    {
        this->insertParagrapher(widget);
    });

    connect(widget, &ParagrapherWidget::deleteParagrapherSignal, this, [this, widget]()
    {
        this->removeParagrapher(widget);
    });

    connect(widget, &ParagrapherWidget::updateParagrapherSignal, this, [this, widget]()
    {
        _text_changed = true;
        widget->updateStats();
    });
}

