#include "TrainingController.h"
#include "Vocabulary.h"

TrainingController::TrainingController(int level, QString sourceLanguage, QString targetLanguage, QObject *parent)
    : QObject(parent)
    , languageMode(TargetLanguage)
    , started(false)
    , level(level)
    , sourceLanguage(sourceLanguage)
    , targetLanguage(targetLanguage)
    , vocabulary(Vocabulary::create(sourceLanguage, targetLanguage))
{
}

TrainingController::~TrainingController()
{
    delete vocabulary;
}


void TrainingController::loadWords(QDir location)
{
    Q_ASSERT(!started);
    if (!location.cd("words." + sourceLanguage + "." + targetLanguage))
        return;
    foreach (QFileInfo const &info, location.entryInfoList(QStringList("*.csv")))
        vocabulary->loadWordlist(info.filePath());
}

void TrainingController::loadHints(QDir location)
{
    Q_ASSERT(!started);
    vocabulary->loadHintList(location.filePath("hints." + targetLanguage + ".txt"));
}


void TrainingController::loadWords(QList<Translation> const &translations)
{
    Q_ASSERT(!started);
    vocabulary->loadWordlist(translations);
}


QStringList const &TrainingController::words() const
{
    return wordList;
}


void TrainingController::start()
{
    Q_ASSERT(!started);
    wordList = vocabulary->wordList(level);
    currentWord = wordList.begin();

    if (currentWord == wordList.end())
        return;

    retries.clear();
    foreach (QString const &word, wordList)
        retries.insert(word, 1);

    status = TrainingStatus();
    statusChanged(status);

    started = true;
    emit nextQuestion();
}

void TrainingController::toggleLanguageMode()
{
    Q_ASSERT(started);
    switch (languageMode)
    {
    case TargetLanguage:
        languageMode = SourceLanguage;
        break;
    case SourceLanguage:
        languageMode = TargetLanguage;
        break;
    }

    nextWord();
}

QString TrainingController::questionLanguage() const
{
    Q_ASSERT(started);
    switch (languageMode)
    {
    case TargetLanguage:
        return targetLanguage;
    case SourceLanguage:
        return sourceLanguage;
    }

    qFatal("invalid language mode: %d", languageMode);
    return QString();
}

QString TrainingController::questionText() const
{
    Q_ASSERT(started);
    switch (languageMode)
    {
    case TargetLanguage:
        return vocabulary->targetText(*currentWord);
    case SourceLanguage:
        return vocabulary->sourceText(*currentWord);
    }

    qFatal("invalid language mode: %d", languageMode);
    return QString();
}

QString TrainingController::questionPhonetic() const
{
    Q_ASSERT(started);
    switch (languageMode)
    {
    case TargetLanguage:
        return vocabulary->targetPhonetics(*currentWord);
    case SourceLanguage:
        return vocabulary->sourcePhonetics(*currentWord);
    }

    qFatal("invalid language mode: %d", languageMode);
    return QString();
}

QString TrainingController::question() const
{
    Q_ASSERT(started);
    QString text = questionText();
    QString phonetic = questionPhonetic();

    if (!phonetic.isEmpty())
        return QString("%0 (%1)").arg(text, phonetic);
    return text;
}

QString TrainingController::answer() const
{
    Q_ASSERT(started);
    switch (languageMode)
    {
    case TargetLanguage:
        return vocabulary->sourceText(*currentWord);
    case SourceLanguage:
        return vocabulary->targetText(*currentWord);
    }

    qFatal("invalid language mode: %d", languageMode);
    return QString();
}

bool TrainingController::hasHint() const
{
    Q_ASSERT(started);
    return !hint().isEmpty();
}

QString TrainingController::hint() const
{
    Q_ASSERT(started);
    switch (languageMode)
    {
    case TargetLanguage:
        return vocabulary->targetHint(*currentWord);
    case SourceLanguage:
        return vocabulary->sourceHint(*currentWord);
    }

    qFatal("invalid language mode: %d", languageMode);
    return QString();
}

void TrainingController::rotateHints()
{
    Q_ASSERT(started);
    switch (languageMode)
    {
    case TargetLanguage:
        return vocabulary->rotateTargetHints(*currentWord);
    case SourceLanguage:
        return vocabulary->rotateSourceHints(*currentWord);
    }
}

bool TrainingController::correct(QString answer) const
{
    Q_ASSERT(started);
    switch (languageMode)
    {
    case TargetLanguage:
        return vocabulary->matchesSourceText(*currentWord, answer);
    case SourceLanguage:
        return vocabulary->matchesTargetText(*currentWord, answer);
    }

    qFatal("invalid language mode: %d", languageMode);
    return false;
}


void TrainingController::nextWord()
{
    Q_ASSERT(started);
    if (wordList.isEmpty())
        return;

    do
        ++currentWord;
    while (currentWord != wordList.end() && retries[*currentWord] == 0);

    if (currentWord == wordList.end())
    {
        started = false;
        emit finished(status);
    }
    else
    {
        emit nextQuestion();
    }
}

void TrainingController::skip()
{
    Q_ASSERT(started);
    status.skipped++;
    statusChanged(status);
    retries[*currentWord] = 3;
    nextWord();
}

void TrainingController::giveAnswer(QString answer)
{
    Q_ASSERT(started);
    if (correct(answer))
    {
        status.correct++;
        statusChanged(status);
        nextWord();
    }
    else
    {
        status.mistakes++;
        statusChanged(status);
        retries[*currentWord] = qMax(retries[*currentWord], 2);
        emit mistake();
    }
}


int TrainingController::totalWords() const
{
    Q_ASSERT(started);
    return wordList.size();
}

int TrainingController::remainingWords() const
{
    Q_ASSERT(started);
    return currentWord - wordList.begin();
}
