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
    QString targetPhrase;
    QString targetPhonetic;

    Hint()
    {
    }

    Hint(QString targetPhrase, QString targetPhonetic)
        : targetPhrase(targetPhrase)
        , targetPhonetic(targetPhonetic)
    {
    }

    QString format(QString word, QString phonetic) const
    {
        QString phraseText = targetPhrase;
        QString phrasePhonetic = targetPhonetic;

        phraseText.replace(word, "<font color='brown'>" + word + "</font>");
        phrasePhonetic.replace(phonetic, "<font color='brown'>" + phonetic + "</font>");

        QString hintText
                = "<center>"
                + QString("<h1>%0</h1>").arg(phraseText)
                + QString("<h3>%0</h3>").arg(phrasePhonetic)
                + "</center>"
                ;

        return hintText;
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


void Vocabulary::loadWordlist(QString fileName)
{
    QFile wordsFile(fileName);
    if (wordsFile.open(QFile::ReadOnly))
    {
        std::istringstream in(wordsFile.readAll().constData());

        int line = 0;
        std::string item;

        while (in)
        {
            line++;

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
            QString targetText = processTargetText(QString::fromUtf8(item.data(), item.length()).simplified());

            std::getline(in, item, ',');
            QString targetPhonetic = processTargetPhonetic(QString::fromUtf8(item.data(), item.length()).simplified());

            std::getline(in, item, '\n');
            QStringList translations = QString::fromUtf8(item.data(), item.length()).split(';', QString::SkipEmptyParts);
            for (int i = 0; i < translations.size(); i++)
                translations[i] = processTranslation(translations[i].trimmed());

            if (targetText.isEmpty() || translations.empty())
                continue;

            addWord(Vocable(level, targetText, targetPhonetic, translations));
        }
    }
}

void Vocabulary::loadWordlist(QList<Translation> const &wordList)
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
        std::istringstream in(hintsFile.readAll().constData());

        int line = 0;
        std::string item;

        while (in)
        {
            line++;

            if (in.peek() == '#' || in.peek() == '\n')
            {
                std::getline(in, item);
                continue;
            }

            std::getline(in, item, ',');
            QString targetText = QString::fromUtf8(item.data(), item.length()).simplified();

            std::getline(in, item, ',');
            QString targetPhrase = QString::fromUtf8(item.data(), item.length()).simplified();

            std::getline(in, item, '\n');
            QString targetPhonetic = QString::fromUtf8(item.data(), item.length()).simplified();

            if (targetText.isEmpty() || targetPhrase.isEmpty())
                continue;

            hints[targetText].append(Hint(targetPhrase, targetPhonetic));
        }
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
}


QStringList Vocabulary::wordList(int maxLevel) const
{
    QStringList words;

    foreach (Vocable const &voc, vocabulary)
        if (voc.level <= maxLevel)
            words.append(voc.targetText);

    return words;
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
        if (candidate.compare(text, Qt::CaseInsensitive) == 0)
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
    QString targetPhonetic = latinPhonetic(vocable.targetPhonetic);

#if 0
    qDebug() << text << "<matches?>" << targetText << "<or>" << targetPhonetic;
#endif
    return targetText.compare(text, Qt::CaseInsensitive) == 0
        || targetPhonetic.compare(text, Qt::CaseInsensitive) == 0;
}


QString Vocabulary::targetHint(QString word) const
{
    HintMap::const_iterator found = hints.find(word);
    if (found == hints.end())
        return QString();
    QList<Hint> const &hintList = found.value();
    Hint const &hint = hintList.front();

    QString phonetic = targetPhonetics(word);
    return hint.format(word, phonetic);
}

void Vocabulary::rotateTargetHints(QString word)
{
    HintMap::iterator found = hints.find(word);
    if (found == hints.end())
        return;
    QList<Hint> &hintList = found.value();
    std::rotate(hintList.begin(), hintList.begin() + 1, hintList.end());
}


QString Vocabulary::sourceHint(QString word) const
{
    // TODO: source hints
    return QString();
}

void Vocabulary::rotateSourceHints(QString word)
{
    // TODO: source hints
}


QString Vocabulary::processTargetText(QString targetText) const
{
    return targetText;
}

QString Vocabulary::processTargetPhonetic(QString targetPhonetic) const
{
    return targetPhonetic;
}

QString Vocabulary::processTranslation(QString translation) const
{
    return translation;
}

QString Vocabulary::latinPhonetic(QString targetPhonetic) const
{
    return targetPhonetic;
}
