#include "Dictionary.h"

#include <QDebug>

QString Dictionary::wordTypeName(WordType wordType)
{
    switch (wordType)
    {
    case Abbreviation: return "abbreviation";
    case Adjective: return "adjective";
    case Adverb: return "adverb";
    case Article: return "article";
    case Conjunction: return "conjunction";
    case Interjection: return "interjection";
    case None: return "unknown";
    case Noun: return "noun";
    case Phrase: return "phrase";
    case Preposition: return "preposition";
    case Pronoun: return "pronoun";
    case Verb: return "verb";

    case Unknown: return "<unknown>";
    case WordTypeCount: return "<invalid>";
    }

    qFatal("invalid word type enumeration: %d", wordType);
    return QString();
}

Dictionary::WordType Dictionary::wordType(QString name)
{
    if (name.isEmpty())
        return None;

    for (WordType i = Unknown; i < WordTypeCount; i = WordType(i + 1))
        if (name == wordTypeName(i))
            return i;

    qFatal("unknown word type: %s", name.toUtf8().constData());
    return WordTypeCount;
}


Dictionary::Dictionary()
{
}

Dictionary::Dictionary(QVariantList const &list)
{
    foreach (QVariant const &element, list)
    {
        QVariantList list = element.value<QVariantList>();
        WordType kind = wordType(list.at(0).value<QString>());
        sections.insert(kind, DictionarySection(list.at(2).value<QVariantList>()));
    }
}


bool Dictionary::isEmpty() const
{
    return sections.isEmpty();
}


static QDataStream &operator << (QDataStream &data, Dictionary::WordType v)
{
    data << Dictionary::wordTypeName(v);
    return data;
}

static QDataStream &operator >> (QDataStream &data, Dictionary::WordType &v)
{
    QString wordTypeName;
    data >> wordTypeName;
    v = Dictionary::wordType(wordTypeName);
    return data;
}


QDataStream &operator << (QDataStream &data, Dictionary const &v)
{
    data << v.sections;
    return data;
}

QDataStream &operator >> (QDataStream &data, Dictionary &v)
{
    data >> v.sections;
    return data;
}
