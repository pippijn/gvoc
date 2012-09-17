#include "Vocabulary_en_zh.h"

#include <QDebug>

template<typename T, size_t N>
static size_t arraySize(T (&)[N])
{
    return N;
}

struct UTF8String : QString
{
    UTF8String(char const *s)
    {
        QString::operator = (fromUtf8(s));
    }
};

struct AccentedCharacter
{
    UTF8String accented;
    UTF8String normal;
};

static AccentedCharacter const accentMap[] = {
    { "ā", "a" },
    { "á", "a" },
    { "ǎ", "a" },
    { "à", "a" },

    { "ō", "o" },
    { "ó", "o" },
    { "ǒ", "o" },
    { "ò", "o" },

    { "ē", "e" },
    { "é", "e" },
    { "ě", "e" },
    { "è", "e" },

    { "iū", "iu" },
    { "iú", "iu" },
    { "iǔ", "iu" },
    { "iù", "iu" },

    { "ī", "i" },
    { "í", "i" },
    { "ǐ", "i" },
    { "ì", "i" },

    { "ū", "u" },
    { "ú", "u" },
    { "ǔ", "u" },
    { "ù", "u" },

    { "ǖ", "ü" },
    { "ǘ", "ü" },
    { "ǚ", "ü" },
    { "ǜ", "ü" },
};


QString const Vocabulary_en_zh::className = "en.zh-CN";


Vocabulary_en_zh::Vocabulary_en_zh()
{
}


QString Vocabulary_en_zh::normalisePhonetic(QString targetPhonetic) const
{
    QStringList list = targetPhonetic.split(' ');
    bool changed = false;

    if (!list.at(0).contains(QRegExp("[0-9]")))
        return targetPhonetic;

    for (int i = 0; i < list.size(); i++)
    {
        QString part = list[i].replace("v", QString::fromUtf8("ü"));
        if (part.isEmpty())
        {
            qWarning() << "empty part:" << targetPhonetic;
            continue;
        }

        QChar toneChar = part[part.size() - 1];
        if (toneChar > '5' || toneChar < '1')
        {
            qWarning() << "invalid tone:" << part;
            continue;
        }

        int tone = toneChar.toAscii() - '0';

        if (tone == 5)
            part = part.left(part.size() - 1);
        else
        {
            static int const toneCount = 4;
            for (size_t letter = 0; letter < arraySize(accentMap) / toneCount; letter++)
            {
                int index = letter * toneCount + (tone - 1);

                QString replaced = part;
                replaced.replace(accentMap[index].normal, accentMap[index].accented);
                if (replaced != part)
                {
                    part = replaced.left(replaced.size() - 1);
                    break;
                }
            }
        }

        list[i] = part;
        changed = true;
    }

    if (!changed)
        return targetPhonetic;

    return list.join(" ");
}

QString Vocabulary_en_zh::phoneticToLatin(QString targetPhonetic) const
{
    for (size_t i = 0; i < arraySize(accentMap); i++)
        targetPhonetic.replace(accentMap[i].accented, accentMap[i].normal);
    return targetPhonetic;
}

bool Vocabulary_en_zh::matchTranslation(QString actual, QString expected) const
{
    if (Base::matchTranslation(actual, expected))
        return true;

    QRegExp nonLatin("[^a-zA-Z0-9]");
    if (expected.contains(nonLatin))
    {
        QString withoutNonLatin = expected;
        // Remove all non-latin characters
        if (Base::matchTranslation(actual, withoutNonLatin.remove(nonLatin).simplified()))
            return true;
    }

    if (expected.startsWith("to ") || expected.startsWith("a ") || expected.startsWith("an ") || expected.startsWith("the "))
    {
        // Word is a verb, compare without "to"
        if (Base::matchTranslation(actual, expected.right(expected.size() - (expected.indexOf(' ') + 1))))
            return true;
    }

    if (expected.contains('('))
    {
        QString withoutBracketedTerms = expected;
        // Remove all brackets
        if (Base::matchTranslation(actual, withoutBracketedTerms.remove(QRegExp("\\([^)]+\\)")).simplified()))
            return true;
    }

    return false;
}
