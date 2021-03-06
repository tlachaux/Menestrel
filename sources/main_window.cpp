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
    /* -----------------------------------
     *  Directory
     * -------------------------------- */

    _home = QDir(QDir::homePath() + "/Menestrel");

    if (!_home.exists()) QDir().mkdir(QDir::homePath() + "/Menestrel");

    /* -----------------------------------
     *  Window
     * -------------------------------- */

    this->setMinimumSize    (QSize(1000, 800));
    this->setStyleSheet     ("border: 0px; background-color: rgb(29, 47, 55); \
                            color: rgb(151, 170, 182); font-size: 16px");

    /* -----------------------------------
     *  Central layout
     * -------------------------------- */

    auto lt = new QGridLayout;

    lt->setAlignment        (Qt::AlignHCenter | Qt::AlignTop);
    lt->setContentsMargins  (0, 20, 0, 0);

    /* -----------------------------------
     *  Central widget
     * -------------------------------- */

    auto central_widget = new QWidget;

    central_widget->setLayout(lt);

    /* -----------------------------------
     *  Scroll area
     * -------------------------------- */

    auto scroll_area = new QScrollArea;

    scroll_area->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOn);
    scroll_area->setWidget                  (central_widget);
    scroll_area->setWidgetResizable         (true);
    scroll_area->setStyleSheet              ("QScrollBar::sub-line:vertical {border:0px;}\
                                              QScrollBar::add-line:vertical {border:0px;}\
                                              QScrollBar::handle::vertical {border-radius:5px;\
                                              background-color:rgb(160, 160, 160)}");

    this->setCentralWidget(scroll_area);

    /* -----------------------------------
     *  Font
     * -------------------------------- */

    int id = QFontDatabase::addApplicationFont("../fonts/AppleGaramond.ttf");
    _font  = QFont(QFontDatabase::applicationFontFamilies(id).at(0));

    /* -----------------------------------
     *  Novel title widget
     * -------------------------------- */

    _novel_title_wgt = new TextWidget(_font);

    _novel_title_wgt->setText       ("...");
    _novel_title_wgt->setStyleSheet ("font-size: 50px");
    _novel_title_wgt->setFixedWidth (880);

    lt->addWidget(_novel_title_wgt, 0, 2, 1, 4);

    connect(_novel_title_wgt, &TextWidget::textChanged, this, [this]() {_text_changed = true;});

    /* -----------------------------------
     *  Novel sub title widget
     * -------------------------------- */

    _novel_sub_title_wgt = new TextWidget(_font);

    _novel_sub_title_wgt->setText       ("...");
    _novel_sub_title_wgt->setStyleSheet ("font-size: 30px");
    _novel_sub_title_wgt->setFixedWidth (880);

    //lt->addWidget(_novel_sub_title_wgt, 1, 2, 1, 4);

    connect(_novel_sub_title_wgt, &TextWidget::textChanged, this, [this]() {_text_changed = true;});

    /* -----------------------------------
     *  Chapter title widget
     * -------------------------------- */

    _chapter_title_wgt = new TextWidget(_font);

    _chapter_title_wgt->setText       ("Chapitre X");
    _chapter_title_wgt->setStyleSheet ("font-size: 20px");
    _chapter_title_wgt->setFixedWidth (880);

    //lt->addWidget(_chapter_title_wgt, 2, 2, 1, 4);

    connect(_chapter_title_wgt, &TextWidget::textChanged, this, [this]() {_text_changed = true;});

    /* -----------------------------------
     *  Chapter dynamic layout
     * -------------------------------- */

    _dynamic_chapter_lt->setAlignment       (Qt::AlignTop);
    _dynamic_chapter_lt->setContentsMargins (0, 0, 0, 0);

    /* -----------------------------------
     *  Chapter add button
     * -------------------------------- */

    auto button = new QPushButton("+");

    button->setFixedHeight(40);

    _dynamic_chapter_lt->addWidget(button);

    connect(button, &QPushButton::clicked, this, [this]()
    {
        this->addChapter();
        this->clearAllParagraph();
        this->loadChapter(_chapters.size());
        this->addParagraph(false);
        this->changeCurrentChapter(_chapters.size());
    });
    /* -----------------------------------
     *  Chapter menu widget
     * -------------------------------- */

    auto chapter_wgt = new QWidget;

    chapter_wgt->setLayout      (_dynamic_chapter_lt);
    chapter_wgt->setStyleSheet  ("QPushButton {font-size: 20px; background-color : rgb(14, 32, 40); border-radius: 10px}\
                                  QPushButton:pressed {font-size: 20px; background-color: rgb(04, 12, 30)}\
                                  QPushButton:hover {font-size: 25px; color: rgb(161, 180, 192)}\
                                  QLabel {color : rgb(151, 170, 182); border: 0px solid rgb(151, 170, 182);\
                                  border-radius: 5px; font-size: 14px;}");
    chapter_wgt->setFixedWidth  (200);

    lt->addWidget(chapter_wgt, 1, 0, 1, 1);

    /* -----------------------------------
     *  Paragraph dynamic layout
     * -------------------------------- */

    _dynamic_paragraph_lt->setAlignment(Qt::AlignTop);
    _dynamic_paragraph_lt->setContentsMargins(0, 0, 0, 0);

    /* -----------------------------------
     *  Paragraphs widget
     * -------------------------------- */

    auto paragraphs_wgt = new QWidget;

    paragraphs_wgt->setLayout       (_dynamic_paragraph_lt);
    paragraphs_wgt->setStyleSheet   ("background-color: rgb(24, 42, 50); font-size: 18px");
    paragraphs_wgt->setFixedWidth   (880);

    lt->addWidget(paragraphs_wgt, 1, 2, 1, 1);

    /* -----------------------------------
     *  Auto save
     * -------------------------------- */

    connect(&_timer, &QTimer::timeout, this, [this]()
    {
        this->quickSave();
    });

    _timer.start(60000);

    /* -----------------------------------
     *  Occurences counter (Ctrl + R)
     * -------------------------------- */

    QAction* refresh_occ_shortcut = new QAction(this);

    refresh_occ_shortcut->setShortcut(QKeySequence("Ctrl+R"));

    this->addAction(refresh_occ_shortcut);

    connect(refresh_occ_shortcut, &QAction::triggered, this, [this]()
    {
        this->countOccurences();
        this->displayOccurences();
    });

    /* -----------------------------------
     *  Orthograph checker (Ctrl + E)
     * -------------------------------- */

    QAction* refresh_errors_shortcut = new QAction(this);

    refresh_errors_shortcut->setShortcut(QKeySequence("Ctrl+E"));

    this->addAction(refresh_errors_shortcut);

    connect(refresh_errors_shortcut, &QAction::triggered, this, [this]()
    {
        int counter = 0;

        for (auto paragraph : _paragraphs)
        {
            counter += paragraph->updateStats();

            if (paragraph->isModified())
            {
                paragraph->updateOrthograph();
                paragraph->format();
            }
        }

        std::cout << "Letters : " << counter << std::endl;
    });
}

MainWindow::~MainWindow()
{
    this->save("chapter_" + QString::number(_current_chapter) + "/last_content");
}

/**
 * @brief MainWindow::keyPressEvent
 * @param event
 */
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        this->close();
    }
}

/**
 * @brief MainWindow::openProject
 * @param project
 */
void MainWindow::openProject(QString project)
{
    _project = QDir(_home.path() + "/" + project);

    if (!_project.exists()) QDir().mkdir(_home.path() + "/" + project);

    if (!QDir(_project.path() + "/chapter_1/").exists())
    {
        this->addChapter();
        this->loadChapter(1);
        this->addParagraph(false);
        _project.mkdir("chapter_1");
    }
    else
    {
        int i = 1;

        while (QDir(_project.path() + "/chapter_" + QString::number(i) + "/").exists())
        {
            this->addChapter();
            ++i;
        }

        this->loadChapter(i - 1);
        this->addParagraph(false);

        this->changeCurrentChapter(i - 1);
    }
}

/**
 * @brief MainWindow::loadFile
 * @param path
 */
void MainWindow::loadChapter(int number)
{
    QString chapter_path =  + "chapter_" + QString::number(number);

    QFile readFile(_project.path() + "/" + chapter_path + "/last_content");

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
               _novel_title_wgt->setText(line);
               _novel_title_wgt->setTextFormat();
               first_line = false;
           }
           else if (line.isEmpty() && !buffer.isEmpty())
           {
               this->addParagraph();
               _paragraphs.back()->setText(buffer);
               _paragraphs.back()->format();
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
}

/**
 * @brief MainWindow::addParagraph
 */
void MainWindow::addParagraph(bool locked)
{
    auto paragraph = new ParagraphWidget(_font, "", locked);

    _paragraphs.append(paragraph);
    _dynamic_paragraph_lt->addWidget(paragraph);

    this->connectParagraph(paragraph);
}

/**
 * @brief MainWindow::addChapter
 */
void MainWindow::addChapter()
{
    int number = _chapters.size() + 1;

    auto new_button = new QPushButton("Chapter " + QString::number(number));

    new_button->setFixedHeight(60);

    _dynamic_chapter_lt->insertWidget(_chapters.size(), new_button);
    _chapters.push_back(new_button);

    connect(new_button, &QPushButton::clicked, this, [this, number]()
    {
        this->clearAllParagraph();
        this->loadChapter(number);
        this->addParagraph();
        this->changeCurrentChapter(number);
    });

    QString chapter_path =  + "chapter_" + QString::number(number);

    if (!QDir(_project.path() + "/" + chapter_path).exists())
    {
        _project.mkdir(chapter_path);
    }
}

/**
 * @brief MainWindow::insertParagraph
 * @param widget
 */
void MainWindow::insertParagraph(ParagraphWidget * widget)
{
    auto paragraph = new ParagraphWidget(_font, "", false);

    for (int i=0; i < _paragraphs.size(); ++i)
    {
        if (_paragraphs[i] == widget)
        {
            _dynamic_paragraph_lt->insertWidget(i + 1, paragraph);
            _paragraphs.insert(i + 1, paragraph);
        }
    }

    this->connectParagraph(paragraph);
}

/**
 * @brief MainWindow::removeParagraph
 * @param widget
 */
void MainWindow::removeParagraph(ParagraphWidget * widget)
{
    if (!widget->text().isEmpty())
    {
        this->save("chapter_" + QString::number(_current_chapter)
                  + "/paragraph_removed_" + QDateTime::currentDateTime().toString());
    }

    _dynamic_paragraph_lt->removeWidget(widget);
    _paragraphs.removeOne(widget);
    delete widget;
}

/**
 * @brief MainWindow::quickSave
 */
void MainWindow::quickSave()
{
    if (!_text_changed) return;

    _timer.start(60000);
    _text_changed = false;

    this->save("chapter_" + QString::number(_current_chapter)
              + "/quick_save_" + QDateTime::currentDateTime().toString());
}

/**
 * @brief MainWindow::save
 * @param title
 */
void MainWindow::save(QString path)
{
    QFile save_file(_project.path() + "/" + path);

    if(save_file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        QTextStream stream(&save_file);

        stream << _novel_title_wgt->toPlainText() << "\n\n";

        for (auto paragraph : _paragraphs)
        {
            stream << paragraph->text() << "\n\n";
        }

        stream.flush();
        save_file.close();
    }
}

/**
 * @brief MainWindow::countOccurences
 */
void MainWindow::countOccurences()
{
    QRegExp separator("[(,|'|;| |.|/-)]");

    _occurences.clear();

    for (auto paragraph : _paragraphs)
    {
        for (auto word : paragraph->text().split(separator))
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

/**
 * @brief MainWindow::displayOccurences
 */
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

/**
 * @brief MainWindow::changeCurrentChapter
 * @param number
 */
void MainWindow::changeCurrentChapter(int number)
{
    _current_chapter = number;

    for (auto chapter : _chapters)
    {
        chapter->setStyleSheet("");
    }

    _chapters[number - 1]->setStyleSheet("background-color: rgb(54, 72, 80)");
}

/**
 * @brief MainWindow::connectParagraph
 * @param widget
 */
void MainWindow::connectParagraph(ParagraphWidget * widget)
{
    connect(widget, &ParagraphWidget::addParagraphSignal, this, [this, widget]()
    {
        this->insertParagraph(widget);
    });

    connect(widget, &ParagraphWidget::deleteParagraphSignal, this, [this, widget]()
    {
        this->removeParagraph(widget);
    });

    connect(widget, &ParagraphWidget::updateParagraphSignal, this, [this, widget]()
    {
        _text_changed = true;
        widget->updateStats();
    });
}

void MainWindow::clearAllParagraph()
{
    this->save("chapter_" + QString::number(_current_chapter) + "/last_content");

    for (auto paragraph : _paragraphs)
    {
        _dynamic_paragraph_lt->removeWidget(paragraph);
        delete paragraph;
    }

    _paragraphs.clear();
}


