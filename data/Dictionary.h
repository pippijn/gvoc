#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QVariant>

#include "DictionarySection.h"

struct Dictionary
{
    enum WordType
    {
        Unknown,
        Abbreviation,
        Adjective,
        Adverb,
        Article,
        Conjunction,
        Interjection,
        None,
        Noun,
        Phrase,
        Prefix,
        Preposition,
        Pronoun,
        Verb,
        WordTypeCount
    };

    static QString wordTypeName(WordType wordType);
    static WordType wordType(QString wordTypeName);

    QMap<WordType, DictionarySection> sections;

public:
    Dictionary();
    Dictionary(QVariantList const &list);

    bool isEmpty() const;
};

QDataStream &operator << (QDataStream &data, Dictionary const &v);
QDataStream &operator >> (QDataStream &data, Dictionary &v);

#endif // DICTIONARY_H
