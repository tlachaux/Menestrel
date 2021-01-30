#ifndef ORTHOGRAPCORRECTOR_H
#define ORTHOGRAPCORRECTOR_H

#include <QMap>
#include <QTextCursor>
#include <QVector>

class OrthographCorrector
{
public:
    static OrthographCorrector & singleton();

    bool doesWordExist(QString word);
    bool doesWordExist(QTextCursor cursor);

protected:
    static OrthographCorrector * _singleton;

    OrthographCorrector();
    void loadDictionary();

    QMap<QChar, QMap<QChar, QVector<QString>>> _dictionary;
};

#endif // ORTHOGRAPCORRECTOR_H
