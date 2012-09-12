#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <QVariant>
#include <QStringList>

struct Translation
{
    static int const serialVersion = 0x00000001;

    struct Primary
    {
        QString translated;
        QString sourceText;
        QString translatedPhonetic;
        QString sourcePhonetic; // TODO: find out what this is

    public:
        Primary();
        Primary(QVariantList const &list);
    };

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
            Preposition,
            Pronoun,
            Verb,
            WordTypeCount
        };

        static char const *wordTypeName(WordType wordType);
        char const *wordTypeName() const;

        WordType wordType;
        QStringList alternatives;
        QMap<QString, QStringList> alternativeTranslations;

    public:
        Dictionary();
        Dictionary(QVariantList const &list);
    };

    QList<Primary> primary;
    QList<Dictionary> dictionary;
    QString sourceLanguage;

public:
    Translation();
    Translation(QVariantList const &list);
};

QDataStream &operator << (QDataStream &data, Translation const &v);
QDataStream &operator >> (QDataStream &data, Translation &v);

#endif // TRANSLATION_H
