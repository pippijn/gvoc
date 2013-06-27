#include "TrainingController.h"
#include "Vocabulary.h"
#include "PhoneticsManager.h"


struct TrainingController::LanguageSelector
{
    virtual QString language() const = 0;
    virtual QString text() const = 0;
    virtual QString phonetic() const = 0;
    virtual QStringList options() const = 0;
    virtual Vocabulary::Hint const *hint() const = 0;
    virtual void rotateHints() = 0;
    virtual bool matches(QString answer) const = 0;

    LanguageSelector(TrainingController &self)
        : self(self)
    {
    }

    virtual ~LanguageSelector()
    {
    }

    TrainingController &self;
};


struct TargetLanguageSelector : TrainingController::LanguageSelector
{
    TargetLanguageSelector(TrainingController &self)
        : LanguageSelector(self)
    {
    }

    QString language() const
    {
        return self.targetLanguage;
    }
    QString text() const
    {
        return self.vocabulary->targetText(*self.currentWord);
    }
    QString phonetic() const
    {
        return self.vocabulary->targetPhonetics(*self.currentWord);
    }
    QStringList options() const
    {
        return self.vocabulary->targetOptions(*self.currentWord);
    }
    Vocabulary::Hint const *hint() const
    {
        return self.vocabulary->hint(*self.currentWord, self.maxLevel);
    }
    void rotateHints()
    {
        return self.vocabulary->rotateHints(*self.currentWord, self.maxLevel);
    }
    bool matches(QString answer) const
    {
        return self.vocabulary->matchesTargetText(*self.currentWord, answer);
    }
};

struct SourceLanguageSelector : TrainingController::LanguageSelector
{
    SourceLanguageSelector(TrainingController &self)
        : LanguageSelector(self)
    {
    }

    QString language() const
    {
        return self.sourceLanguage;
    }
    QString text() const
    {
        return self.vocabulary->sourceText(*self.currentWord);
    }
    QString phonetic() const
    {
        return self.vocabulary->sourcePhonetics(*self.currentWord);
    }
    QStringList options() const
    {
        return self.vocabulary->sourceOptions(*self.currentWord);
    }
    Vocabulary::Hint const *hint() const
    {
        return NULL;
    }
    void rotateHints()
    {
    }
    bool matches(QString answer) const
    {
        return self.vocabulary->matchesSourceText(*self.currentWord, answer);
    }
};


TrainingController::TrainingController(int minLevel, int maxLevel,
                                       QString sourceLanguage, QString targetLanguage,
                                       PhoneticsManager &phoneticsManager,
                                       QObject *parent)
    : QObject(parent)
    , started(false)
    , minLevel(minLevel)
    , maxLevel(maxLevel)
    , sourceLanguage(sourceLanguage)
    , targetLanguage(targetLanguage)
    , phoneticsManager(phoneticsManager)
    , vocabulary(Vocabulary::create(sourceLanguage, targetLanguage))
{
    srand(time(NULL));

    languageMode[0] = new SourceLanguageSelector(*this);
    languageMode[1] = new TargetLanguageSelector(*this);

    QFile retries("retries.dat");
    if (retries.open(QFile::ReadOnly))
    {
        QDataStream stream(&retries);
        stream >> retriesMap;
    }
}

TrainingController::~TrainingController()
{
    QFile retries("retries.dat");
    if (retries.open(QFile::WriteOnly))
    {
        QDataStream stream(&retries);
        stream << retriesMap;
    }

    delete languageMode[1];
    delete languageMode[0];
    delete vocabulary;
}


TrainingController::LanguageSelector &TrainingController::answerSelector()
{
    return *languageMode[0];
}
TrainingController::LanguageSelector const &TrainingController::answerSelector() const
{
    return *languageMode[0];
}

TrainingController::LanguageSelector &TrainingController::questionSelector()
{
    return *languageMode[1];
}
TrainingController::LanguageSelector const &TrainingController::questionSelector() const
{
    return *languageMode[1];
}


void TrainingController::loadWords(QDir location)
{
    Q_ASSERT(!started);
    if (!location.cd("words." + sourceLanguage + "." + targetLanguage))
        return;
    foreach (QFileInfo const &info, location.entryInfoList(QStringList("*.csv")))
        vocabulary->loadWordList(info.filePath());
}

void TrainingController::loadHints(QDir location)
{
    Q_ASSERT(!started);
    if (!location.cd("hints." + sourceLanguage + "." + targetLanguage))
        return;
    foreach (QFileInfo const &info, location.entryInfoList(QStringList("*.txt")))
        vocabulary->loadHintList(info.filePath());
}


void TrainingController::loadWords(QList<Translation> const &translations)
{
    Q_ASSERT(!started);
    vocabulary->loadWordList(translations);
}


QStringList const &TrainingController::words() const
{
    return wordList;
}


void TrainingController::start()
{
    Q_ASSERT(!started);
    wordList.clear();

    QStringList const &vocWordList = vocabulary->wordList(minLevel, maxLevel);
    foreach (QString word, vocWordList)
    {
        // Add missing words to retries map.
        if (retries(word) == -1)
            setRetries(word, 1);
        // Add words with at least 1 retry to the training list.
        if (retries(word) > 0)
            wordList.append(word);
    }

    // If there were no words with at least 1 retry, add all words and
    // reset their number of retries to 1.
    if (wordList.isEmpty())
    {
        wordList = vocWordList;
        foreach (QString const &word, wordList)
            setRetries(word, 1);
    }

    std::random_shuffle(wordList.begin(), wordList.end());
    currentWord = wordList.begin();

    if (currentWord == wordList.end())
        return;

    status = TrainingStatus();
    statusChanged(status);

    started = true;
    emit nextQuestion();
}

void TrainingController::toggleLanguageMode()
{
    Q_ASSERT(started);
    qSwap(languageMode[0], languageMode[1]);

    nextWord();
}

QString TrainingController::questionLanguage() const { return questionSelector().language(); }
QString TrainingController::questionText() const { return questionSelector().text(); }
QString TrainingController::questionPhonetic() const { return questionSelector().phonetic(); }

QString TrainingController::question() const
{
    Q_ASSERT(started);
    QString text = questionText();
    QString phonetic = questionPhonetic();

    if (!phonetic.isEmpty())
        return QString("%0 (%1)").arg(text, phonetic);
    return text;
}

QString TrainingController::answerLanguage() const { return answerSelector().language(); }
QString TrainingController::answerText() const { return answerSelector().text(); }
QString TrainingController::answerPhonetic() const { return answerSelector().phonetic(); }
QStringList TrainingController::answerOptions() const { return answerSelector().options(); }

QString TrainingController::answer() const
{
    Q_ASSERT(started);
    QString text = answerText();
    QString phonetic = answerPhonetic();

    if (!phonetic.isEmpty())
        return QString("%0 (%1)").arg(text, phonetic);
    return text;
}

QString TrainingController::formatHint(Vocabulary::Hint const &hint, bool withTranslation) const
{
    QString word = questionText();
    QString phonetic = questionPhonetic();

    QString phraseText = vocabulary->hintText(hint);
    QString phrasePhonetic = phoneticsManager.cachedPhonetic(questionLanguage(), phraseText);
    if (phrasePhonetic.isEmpty())
        phrasePhonetic = vocabulary->hintPhonetic(hint);
    QString translation = withTranslation ? vocabulary->hintTranslation(hint) : QString();

    phraseText.replace(word, "<font color='brown'>" + word + "</font>", Qt::CaseInsensitive);
    phrasePhonetic.replace(phonetic, "<font color='brown'>" + phonetic + "</font>", Qt::CaseInsensitive);

    QString hintText
            = "<center>"
            + QString("<h1>%0</h1>").arg(phraseText)
            + QString("<h3>%0</h3>").arg(phrasePhonetic)
            + QString("<h4>%0</h4>").arg(translation)
            + "</center>"
            ;

    return hintText;
}

QString TrainingController::hint(bool withTranslation) const
{
    Vocabulary::Hint const *hint = questionSelector().hint();
    if (hint != NULL)
        return formatHint(*hint, withTranslation);
    return QString();
}

void TrainingController::rotateHints()
{
    Q_ASSERT(started);
    questionSelector().rotateHints();
}

bool TrainingController::hasHint() const
{
    Q_ASSERT(started);
    return questionSelector().hint() != NULL;
}

bool TrainingController::hasRetries() const
{
    foreach (QString word, wordList)
        if (retries(word) > 0)
            return true;
    return false;
}

int TrainingController::retries(QString word) const
{
    if (retriesMap.contains(word))
        return retriesMap[word];
    return -1;
}

void TrainingController::setRetries(QString word, int retries)
{
    Q_ASSERT(retries >= 0);
    retriesMap[word] = retries;
}


void TrainingController::nextWord()
{
    Q_ASSERT(started);
    if (wordList.isEmpty())
        return;

    do
        ++currentWord;
    while (currentWord != wordList.end() && retries(*currentWord) <= 0);

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
    setRetries(*currentWord, 3);
    nextWord();
}

void TrainingController::giveAnswer(QString answer)
{    
    Q_ASSERT(started);
    answer = answer.simplified();

    if (answerSelector().matches(answer))
    {
        status.correct++;
        statusChanged(status);
        setRetries(*currentWord, retries(*currentWord) - 1);
        nextWord();
    }
    else
    {
        status.mistakes++;
        statusChanged(status);
        setRetries(*currentWord, qMax(retries(*currentWord), 2));
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
