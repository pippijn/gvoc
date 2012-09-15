#ifndef DICTIONARYSECTION_H
#define DICTIONARYSECTION_H

#include <QVariant>
#include <QStringList>

struct DictionarySection
{
    QMap<QString, QStringList> translations;

public:
    DictionarySection();
    DictionarySection(QVariantList const &list);
};

QDataStream &operator << (QDataStream &data, DictionarySection const &v);
QDataStream &operator >> (QDataStream &data, DictionarySection &v);

#endif // DICTIONARYSECTION_H
