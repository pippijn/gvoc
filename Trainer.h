#ifndef TRAINER_H
#define TRAINER_H

#include <QDialog>
#include <QStringList>
#include <QMap>

class Translation;

namespace Ui {
    class Trainer;
}

class Trainer : public QDialog {
    Q_OBJECT

public:
    Trainer(QString sourceLanguage, QString targetLanguage, QList<Translation> wordList, QWidget *parent = 0);
    ~Trainer();

private:
    void readWordList(QString list);

private slots:
    void giveAnswer();
    void giveHint();
    void answerGiven();
    void reverseModeActivated(bool checked);

private: // data
    Ui::Trainer *ui;

    struct Vocable
    {
        QString word;
        QString phonetic;
        QStringList translations;
        int retries;

        Vocable()
            : retries(1)
        {
        }
    };

    typedef QList<Vocable> WordList;

    WordList words;
    WordList::const_iterator current;

    void addWord(Vocable voc);


    struct Hint
    {
        QString word;
        QString phonetic;
    };

    typedef QMap<QString, QList<Hint> > HintMap;

    HintMap hints;


    bool reverseMode;


    struct Status
    {
        int correct;
        int mistakes;
        int skipped;

        Status()
            : correct(0)
            , mistakes(0)
            , skipped(0)
        {
        }
    };

    Status status;

private: // functions
    void updateStatus();
    bool correct(QString answer, Vocable const &voc);
    void reset();
    bool hasNextWord() const;
    void nextWord();
    QString question();
};

#endif // TRAINER_H
