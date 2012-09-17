#include "TrainingController.h"
#include "Vocabulary.h"


struct TrainingController::LanguageSelector
{
    virtual QString language() const = 0;
    virtual QString text() const = 0;
    virtual QString phonetic() const = 0;
    virtual QStringList options() const = 0;
    virtual QString hint() const = 0;
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
    QString hint() const
    {
        return self.vocabulary->targetHint(*self.currentWord);
    }
    void rotateHints()
    {
        return self.vocabulary->rotateTargetHints(*self.currentWord);
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
    QString hint() const
    {
        return self.vocabulary->sourceHint(*self.currentWord);
    }
    void rotateHints()
    {
        return self.vocabulary->rotateSourceHints(*self.currentWord);
    }
    bool matches(QString answer) const
    {
        return self.vocabulary->matchesSourceText(*self.currentWord, answer);
    }
};


TrainingController::TrainingController(int minLevel, int maxLevel, QString sourceLanguage, QString targetLanguage, QObject *parent)
    : QObject(parent)
    , started(false)
    , minLevel(minLevel)
    , maxLevel(maxLevel)
    , sourceLanguage(sourceLanguage)
    , targetLanguage(targetLanguage)
    , vocabulary(Vocabulary::create(sourceLanguage, targetLanguage))
{
    srand(time(NULL));

    languageMode[0] = new SourceLanguageSelector(*this);
    languageMode[1] = new TargetLanguageSelector(*this);
}

TrainingController::~TrainingController()
{
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
    wordList = vocabulary->wordList(minLevel, maxLevel);
    std::random_shuffle(wordList.begin(), wordList.end());
    currentWord = wordList.begin();

    if (currentWord == wordList.end())
        return;

    if (!hasRetries())
    {
        retries.clear();
        foreach (QString const &word, wordList)
            retries.insert(word, 1);
    }

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

QString TrainingController::hint() const { return answerSelector().hint(); }
void TrainingController::rotateHints() { answerSelector().rotateHints(); }

bool TrainingController::hasHint() const
{
    Q_ASSERT(started);
    return !hint().isEmpty();
}

bool TrainingController::hasRetries() const
{
    foreach (int value, retries.values())
        if (value > 0)
            return true;
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
    answer = answer.simplified();

    if (answerSelector().matches(answer))
    {
        status.correct++;
        statusChanged(status);
        retries[*currentWord]--;
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
