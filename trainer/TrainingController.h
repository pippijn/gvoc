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
    TrainingController(int level, QString sourceLanguage, QString targetLanguage, QObject *parent = 0);
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
    QString answer() const;
    bool hasHint() const;
    QString hint() const;
    void rotateHints();
    bool correct(QString answer) const;

    void nextWord();
    void skip();
    void giveAnswer(QString answer);

    int totalWords() const;
    int remainingWords() const;

signals:
    void statusChanged(TrainingStatus status);
    void mistake();
    void finished(TrainingStatus status);
    void nextQuestion();

private:
    TrainingStatus status;
    LanguageMode languageMode;
    bool started;

    int level;
    QString sourceLanguage;
    QString targetLanguage;

    Vocabulary *const vocabulary;
    QStringList wordList;
    QStringList::const_iterator currentWord;

    QMap<QString, int> retries;
};

#endif // TRAININGMANAGER_H
