#include "Translation.h"

#include <QDebug>

template<typename T>
QList<T> map(QVariantList const &list)
{
    QList<T> mapped;
    foreach (QVariant const &variant, list)
        mapped.append(T(qvariant_cast<QVariantList>(variant)));
    return mapped;
}


Translation::Primary::Primary()
{
}

Translation::Primary::Primary(QVariantList const &list)
    : translated(qvariant_cast<QString>(list.at(0)))
    , sourceText(qvariant_cast<QString>(list.at(1)))
    , translatedPhonetic(qvariant_cast<QString>(list.at(2)))
    , sourcePhonetic(qvariant_cast<QString>(list.at(3)))
{
}


char const *Translation::Dictionary::wordTypeName(WordType wordType)
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
    case Unknown:
    case WordTypeCount:
        break;
    }

    return "<unknown>";
}

char const *Translation::Dictionary::wordTypeName() const
{
    return wordTypeName(wordType);
}


Translation::Dictionary::Dictionary()
    : wordType(Unknown)
{
}

Translation::Dictionary::Dictionary(QVariantList const &list)
    : wordType(Unknown)
    , alternatives(list.at(1).toStringList())
{
    QString wordType = qvariant_cast<QString>(list.at(0));

    if (wordType.isEmpty())
        this->wordType = None;
    else
    {
        for (WordType i = Unknown; i < WordTypeCount; i = WordType(i + 1))
            if (wordType == wordTypeName(i))
                this->wordType = i;
    }
    if (this->wordType == Unknown)
        qFatal("unknown word type: %s", wordType.toUtf8().constData());

    foreach (QVariant const &element, qvariant_cast<QVariantList>(list.at(2)))
    {
        QVariantList alternative = qvariant_cast<QVariantList>(element);
        alternativeTranslations.insert(
                    qvariant_cast<QString>(alternative.at(0)),
                    alternative.at(1).toStringList());
    }
}


Translation::Translation()
{
}

Translation::Translation(QVariantList const &list)
    : primary(map<Primary>(qvariant_cast<QVariantList>(list.at(0))))
    , dictionary(map<Dictionary>(qvariant_cast<QVariantList>(list.at(1))))
    , sourceLanguage(qvariant_cast<QString>(list.at(2)))
{
}


static QDataStream &operator << (QDataStream &data, Translation::Primary const &v)
{
    data << v.translated;
    data << v.sourceText;
    data << v.translatedPhonetic;
    data << v.sourcePhonetic;
    return data;
}

static QDataStream &operator << (QDataStream &data, Translation::Dictionary const &v)
{
    data << (quint32)v.wordType;
    data << v.alternatives;
    data << v.alternativeTranslations;
    return data;
}

QDataStream &operator << (QDataStream &data, Translation const &v)
{
    data << Translation::serialVersion;
    data << v.primary;
    data << v.dictionary;
    data << v.sourceLanguage;
    return data;
}


static QDataStream &operator >> (QDataStream &data, Translation::Primary &v)
{
    data >> v.translated;
    data >> v.sourceText;
    data >> v.translatedPhonetic;
    data >> v.sourcePhonetic;
    return data;
}

static QDataStream &operator >> (QDataStream &data, Translation::Dictionary::WordType &v)
{
    quint32 value;
    data >> value;
    v = (Translation::Dictionary::WordType)value;
    return data;
}

static QDataStream &operator >> (QDataStream &data, Translation::Dictionary &v)
{
    data >> v.wordType;
    data >> v.alternatives;
    data >> v.alternativeTranslations;
    return data;
}

QDataStream &operator >> (QDataStream &data, Translation &v)
{
    int serialVersion;
    data >> serialVersion;
    if (serialVersion != Translation::serialVersion)
    {
        qWarning("file format version 0x%x does not match current version 0x%x; not loading", serialVersion, Translation::serialVersion);
        return data;
    }

    data >> v.primary;
    data >> v.dictionary;
    data >> v.sourceLanguage;
    return data;
}
