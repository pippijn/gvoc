#include "Vocabulary.h"

#include "Translation.h"

#include <QDebug>
#include <QFile>
#include <sstream>


// Words with translations
struct Vocabulary::Vocable
{
    QString targetText;
    QString targetPhonetic;
    QStringList translations;
    int level;

    Vocable()
        : level(0)
    {
    }

    Vocable(int level, QString targetText, QString targetPhonetic, QStringList translations)
        : targetText(targetText)
        , targetPhonetic(targetPhonetic)
        , translations(translations)
        , level(level)
    {
    }
};

struct Vocabulary::Hint
{
    int level;
    QString targetPhrase;
    QString targetPhonetic;
    QString translation;

    Hint()
    {
    }

    Hint(int level, QString targetPhrase, QString targetPhonetic, QString translation)
        : level(level)
        , targetPhrase(targetPhrase)
        , targetPhonetic(targetPhonetic)
        , translation(translation)
    {
    }
};


Vocabulary *Vocabulary::create(QString sourceLanguage, QString targetLanguage)
{
    if (Vocabulary *v = ClassRegistry<Vocabulary>::create(sourceLanguage + "." + targetLanguage))
        return v;
    return new Vocabulary;
}


Vocabulary::Vocabulary()
{
}

Vocabulary::~Vocabulary()
{
}


void Vocabulary::loadWordList(QString fileName)
{
    QFile wordsFile(fileName);
    if (wordsFile.open(QFile::ReadOnly))
    {
        std::istringstream in(wordsFile.readAll().constData());

        int line = 0;

        while (in)
        {
            line++;

            std::string item;
            if (in.peek() == '#' || in.peek() == '\n')
            {
                std::getline(in, item);
                continue;
            }

            if (!std::getline(in, item, ','))
                break;
            bool ok;
            int level = QString::fromUtf8(item.data(), item.length()).simplified().toInt(&ok);
            if (!ok)
            {
                qWarning("format error on line %d: %s", line, item.c_str());
                std::getline(in, item);
                continue;
            }

            std::getline(in, item, ',');
            QString targetText = QString::fromUtf8(item.data(), item.length()).simplified();

            std::getline(in, item, ',');
            QString targetPhonetic = normalisePhonetic(QString::fromUtf8(item.data(), item.length()).simplified());

            std::getline(in, item, '\n');
            QStringList translations = QString::fromUtf8(item.data(), item.length()).split(';', QString::SkipEmptyParts);
            for (int i = 0; i < translations.size(); i++)
                translations[i] = translations[i].trimmed();

            if (targetText.isEmpty() || translations.empty())
                continue;

            addWord(Vocable(level, targetText, targetPhonetic, translations));
        }
    }
}

void Vocabulary::loadWordList(QList<Translation> const &wordList)
{
    foreach (Translation const &word, wordList)
    {
        PrimaryTranslation const &primary = word.primary;
        // skip the empty word
        if (primary.sourceText.isEmpty())
            continue;

        QStringList translations(primary.sourceText);
        foreach (DictionarySection const &section, word.dictionary.sections)
            translations.append(section.translations[primary.translated]);

        addWord(Vocable(word.level, primary.translated, primary.translatedPhonetic.toLower(), translations));
    }
}

void Vocabulary::loadHintList(QString fileName)
{
    QFile hintsFile(fileName);
    if (hintsFile.open(QFile::ReadOnly))
    {
        int level = hintsFile.readLine().toInt();
        std::istringstream in(hintsFile.readAll().constData());

        int line = 0;

        QMap<QString, Hint> readHints;
        foreach (Hint const &hint, hints)
            readHints.insert(hint.targetPhrase, hint);

        while (in)
        {
            line++;

            std::string item;
            if (in.peek() == '#' || in.peek() == '\n')
            {
                std::getline(in, item);
                continue;
            }

            std::getline(in, item);
            QString targetPhrase = QString::fromUtf8(item.data(), item.length()).simplified();

            std::getline(in, item);
            QString targetPhonetic = QString::fromUtf8(item.data(), item.length()).simplified();

            std::getline(in, item);
            QString translation = QString::fromUtf8(item.data(), item.length()).simplified();

            if (targetPhrase.isEmpty() || translation.isEmpty())
                continue;

            readHints.insert(targetPhrase, Hint(level, targetPhrase, targetPhonetic, translation));
        }

        hints = readHints.values();
    }
}


void Vocabulary::addWord(Vocable const &voc)
{
    Q_ASSERT(!voc.targetText.isEmpty());
    Q_ASSERT(!voc.translations.isEmpty());

    Vocable &existing = vocabulary[voc.targetText];
    if (!existing.targetText.isEmpty())
        existing.translations.append(voc.translations);
    else
        existing = voc;

    existing.translations.removeDuplicates();
}


QStringList Vocabulary::wordList(int minLevel, int maxLevel) const
{
    QStringList words;

    foreach (Vocable const &voc, vocabulary)
        if (voc.level >= minLevel && voc.level <= maxLevel)
            words.append(voc.targetText);

    return words;
}


QStringList Vocabulary::targetOptions(QString word) const
{
    VocableMap::const_iterator found = vocabulary.find(word);
    if (found == vocabulary.end())
        return QStringList();
    Vocable const &vocable = found.value();

    return QStringList(vocable.targetText);
}

QString Vocabulary::targetText(QString word) const
{
    VocableMap::const_iterator found = vocabulary.find(word);
    if (found == vocabulary.end())
        return QString();
    Vocable const &vocable = found.value();

    return vocable.targetText;
}

QString Vocabulary::targetPhonetics(QString word) const
{
    VocableMap::const_iterator found = vocabulary.find(word);
    if (found == vocabulary.end())
        return QString();
    Vocable const &vocable = found.value();

    return vocable.targetPhonetic;
}

QStringList Vocabulary::sourceOptions(QString word) const
{
    VocableMap::const_iterator found = vocabulary.find(word);
    if (found == vocabulary.end())
        return QStringList();
    Vocable const &vocable = found.value();

    return vocable.translations;
}

QString Vocabulary::sourceText(QString word) const
{
    VocableMap::const_iterator found = vocabulary.find(word);
    if (found == vocabulary.end())
        return QString();
    Vocable const &vocable = found.value();

    return vocable.translations.front();
}

QString Vocabulary::sourcePhonetics(QString word) const
{
    VocableMap::const_iterator found = vocabulary.find(word);
    if (found == vocabulary.end())
        return QString();
    Vocable const &vocable = found.value();

    // TODO: no source phonetics, yet
    return QString();
}


bool Vocabulary::matchesSourceText(QString word, QString text) const
{
    VocableMap::const_iterator found = vocabulary.find(word);
    if (found == vocabulary.end())
        return false;
    Vocable const &vocable = found.value();

    foreach (QString candidate, vocable.translations)
        if (matchTranslation(text, candidate))
            return true;

    return false;
}

bool Vocabulary::matchesTargetText(QString word, QString text) const
{
    VocableMap::const_iterator found = vocabulary.find(word);
    if (found == vocabulary.end())
        return false;
    Vocable const &vocable = found.value();

    QString targetText = vocable.targetText;
    QString targetPhonetic = phoneticToLatin(vocable.targetPhonetic);

#if 0
    qDebug() << text << "<matches?>" << targetText << "<or>" << targetPhonetic;
#endif
    return targetText.compare(text, Qt::CaseInsensitive) == 0
        || targetPhonetic.compare(text, Qt::CaseInsensitive) == 0;
}


QString Vocabulary::hintText(Hint const &hint) const
{
    return hint.targetPhrase;
}

QString Vocabulary::hintPhonetic(Hint const &hint) const
{
    return hint.targetPhonetic;
}

QString Vocabulary::hintTranslation(Hint const &hint) const
{
    return hint.translation;
}


Vocabulary::HintList &Vocabulary::hintList(QString word, int maxLevel) const
{
    if (hintIndexLevel != maxLevel)
    {
        hintIndex.clear();
        hintIndexLevel = maxLevel;
    }

    HintMap::iterator found = hintIndex.find(word);
    if (found == hintIndex.end())
    {
        HintList &list = hintIndex[word];
        foreach (Hint const &hint, hints)
            if (hint.level <= maxLevel && hint.targetPhrase.contains(word))
                hintIndex[word].append(hint);

        return list;
    }
    return found.value();
}

Vocabulary::Hint const *Vocabulary::hint(QString word, int maxLevel) const
{
    HintList const &list = hintList(word, maxLevel);
    if (!list.isEmpty())
        return &list.front();

    return NULL;
}

void Vocabulary::rotateHints(QString word, int maxLevel)
{
    HintList &list = hintList(word, maxLevel);
    if (!list.isEmpty())
        std::rotate(list.begin(), list.begin() + 1, list.end());
}


QString Vocabulary::normalisePhonetic(QString phonetic) const
{
    return phonetic;
}

QString Vocabulary::phoneticToLatin(QString phonetic) const
{
    return phonetic;
}

bool Vocabulary::matchTranslation(QString actual, QString expected) const
{
    return actual.compare(expected, Qt::CaseInsensitive) == 0;
}
