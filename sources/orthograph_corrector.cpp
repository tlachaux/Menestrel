#include "orthograph_corrector.h"
#include <QDir>
#include <QTextStream>
#include <QTextBlock>
#include <iostream>

OrthographCorrector * OrthographCorrector::_singleton;

OrthographCorrector::OrthographCorrector()
{
    this->loadDictionary();
}

/**
 * @brief OrthographCorrector::singleton
 * @return
 */
OrthographCorrector & OrthographCorrector::singleton()
{
    if (!OrthographCorrector::_singleton)
    {
        OrthographCorrector::_singleton = new OrthographCorrector();
    }

    return *OrthographCorrector::_singleton;
}

/**
 * @brief OrthographCorrector::loadDictionary
 */
void OrthographCorrector::loadDictionary()
{
    QFile file(QDir::homePath() + "/Menestrel/dictionary_fr3.txt");

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        QString buffer;

        while (!stream.atEnd())
        {
            QString word = stream.readLine();

            if (word.size() > 1)
            {
                _dictionary[word[0]][word[1]].push_back(word);
            }
        }

        file.close();
    }
}

/**
 * @brief OrthographCorrector::doesWordExist
 * @param word
 * @return
 */
bool OrthographCorrector::doesWordExist(QString word)
{
    QVector<QString> & v = _dictionary[word[0]][word[1]];

    return std::find_if(v.begin(), v.end(), [word](QString str) { return str == word; }) != v.end();
}

/**
 * @brief OrthographCorrector::doesWordExist
 * @param cursor
 * @return
 */
bool OrthographCorrector::doesWordExist(QTextCursor cursor)
{
    return this->doesWordExist(cursor.block().text());
}
