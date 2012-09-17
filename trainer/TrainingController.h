#ifndef TRAININGMANAGER_H
#define TRAININGMANAGER_H

#include "TrainingStatus.h"
class Vocabulary;
class Translation;

#include <QObject>
#include <QDir>
#include <QMap>
#include <QStringList>

class TrainingController : public QObject
{
    Q_OBJECT

    enum LanguageMode
    {
        TargetLanguage,
        SourceLanguage
    };

public:
    struct LanguageSelector;
    friend struct TargetLanguageSelector;
    friend struct SourceLanguageSelector;

private:
    LanguageSelector &questionSelector();
    LanguageSelector const &questionSelector() const;

    LanguageSelector &answerSelector();
    LanguageSelector const &answerSelector() const;

public:
    TrainingController(int minLevel, int maxLevel, QString sourceLanguage, QString targetLanguage, QObject *parent = 0);
    ~TrainingController();

    void loadWords(QDir location);
    void loadHints(QDir location);

    void loadWords(QList<Translation> const &translations);

    QStringList const &words() const;

    void start();
    void toggleLanguageMode();
    QString questionLanguage() const;
    QString questionText() const;
    QString questionPhonetic() const;
    QString question() const;
    QString answerLanguage() const;
    QString answerText() const;
    QString answerPhonetic() const;
    QStringList answerOptions() const;
    QString answer() const;
    bool hasHint() const;
    QString hint() const;
    void rotateHints();
    bool hasRetries() const;

    void nextWord();
    void skip();
    void giveAnswer(QString answerText);

    int totalWords() const;
    int remainingWords() const;

signals:
    void statusChanged(TrainingStatus status);
    void mistake();
    void finished(TrainingStatus status);
    void nextQuestion();

private:
    TrainingStatus status;
    LanguageSelector *languageMode[2];
    bool started;

    int minLevel;
    int maxLevel;
    QString sourceLanguage;
    QString targetLanguage;

    Vocabulary *const vocabulary;
    QStringList wordList;
    QStringList::const_iterator currentWord;

    QMap<QString, int> retries;
};

#endif // TRAININGMANAGER_H
