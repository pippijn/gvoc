#include "Trainer.h"
#include "ui_Trainer.h"

#include "Translation.h"

#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>

#include <algorithm>
#include <sstream>
#include <ctime>


template<typename T, size_t N>
size_t array_size(T (&)[N])
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
    { " ", "" },
    { "'", "" },
    { "-", "" },

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

static QStringList split(std::string const &str, char sep)
{
    QStringList words;

    std::istringstream is(str);
    while (is)
    {
        std::string part;
        std::getline(is, part, sep);
        QString word = QString::fromUtf8(part.data(), part.length()).trimmed();
        if (!word.isEmpty())
            words.push_back(word);
    }

    return words;
}

Trainer::Trainer(QString sourceLanguage, QString targetLanguage, QList<Translation> wordList, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Trainer)
    , reverseMode(false)
{
    ui->setupUi(this);
    ui->answer->setFocus();

    srand(time(NULL));

    readWordList(":/wordlists/words." + sourceLanguage + "." + targetLanguage + ".txt");

    foreach (Translation const &word, wordList)
    {
#if DICT_ONLY
        if (!word.dictionary.isEmpty())
#endif
        {
            Translation::Primary const &primary = word.primary.at(0);

            Vocable voc;
            voc.phonetic = primary.translatedPhonetic.toLower();
            voc.word = primary.translated.toLower();
            voc.translations.append(primary.sourceText.toLower());

#if DICT_ONLY
            foreach (Translation::Dictionary const &dict, word.dictionary)
                foreach (QStringList const &list, dict.alternativeTranslations.values())
                    voc.translations.append(list);
#endif

            addWord(voc);
        }
    }

    if (words.empty())
    {
        QMessageBox::information(this, "Error", "Error: no words");
        return;
    }

    ui->answer->setEnabled(true);
    ui->btnAnswer->setEnabled(true);
    ui->btnHint->setEnabled(true);
    ui->btnOK->setEnabled(true);
    ui->reverseMode->setEnabled(true);

    QFile hintsFile(":/wordlists/hints." + targetLanguage + ".txt");
    if (hintsFile.open(QFile::ReadOnly))
    {
        std::istringstream in(hintsFile.readAll().constData());

        while (in)
        {
            QString word;
            Hint hint;

            std::string item;
            if (in.peek() == '#' || in.peek() == '\n')
            {
                std::getline(in, item);
                continue;
            }

            std::getline(in, item, ',');
            word = QString::fromUtf8(item.data(), item.length()).trimmed().replace("\n", "");

            std::getline(in, item, ',');
            hint.word = QString::fromUtf8(item.data(), item.length()).trimmed().replace("\n", "");

            std::getline(in, item, '\n');
            hint.phonetic = QString::fromUtf8(item.data(), item.length()).trimmed();

            if (word.isEmpty() || hint.word.isEmpty() || hint.phonetic.isEmpty())
                continue;
            hints[word].push_back(hint);
        }
    }

    ui->progress->setMaximum(words.size());

    current = words.end() - 1;

    nextWord();

    QSettings settings;
    if (settings.value("reverseMode").toBool())
        ui->reverseMode->setChecked(true);
}

Trainer::~Trainer()
{
    QSettings settings;
    settings.setValue("reverseMode", reverseMode);
    delete ui;
}

void Trainer::addWord(Vocable voc)
{
    for (int i = 0; i < words.size(); i++)
    {
        if (words.at(i).word == voc.word)
        {
            words[i].translations.append(voc.translations);
            return;
        }
    }
    words.append(voc);
}

void Trainer::readWordList(QString list)
{
    QFile wordsFile(list);
    if (wordsFile.open(QFile::ReadOnly))
    {
        std::istringstream in(wordsFile.readAll().constData());

        while (in)
        {
            Vocable voc;

            std::string item;
            if (in.peek() == '#' || in.peek() == '\n')
            {
                std::getline(in, item);
                continue;
            }

            std::getline(in, item, ',');
            voc.word = QString::fromUtf8(item.data(), item.length()).trimmed().replace("\n", "");

            std::getline(in, item, ',');
            voc.phonetic = QString::fromUtf8(item.data(), item.length()).trimmed().replace("\n", "");

            std::getline(in, item, '\n');
            voc.translations = split(item, ';');

            if (voc.word.isEmpty() || voc.phonetic.isEmpty() || voc.translations.empty())
                continue;
            addWord(voc);
        }
    }
}

void Trainer::giveAnswer()
{
    ui->answer->setText("");
    answerGiven();
    ui->answer->setFocus();
}

void Trainer::giveHint()
{
    Q_ASSERT(!reverseMode);

    Vocable const &current = *this->current;

    HintMap::iterator found = hints.find(current.word);
    if (found == hints.end())
        QMessageBox::information(this, "No hints", "No hints available for this vocable.");
    else
    {
        QList<Hint> &hintList = found.value();
        Hint firstHint = hintList.front();

        QString simplified = firstHint.word;
        QString phonetic = firstHint.phonetic;

        simplified.replace(current.word, "<font color='brown'>" + current.word + "</font>");
        phonetic.replace(current.phonetic, "<font color='brown'>" + current.phonetic + "</font>");

        QMessageBox::information(this, "Hint for " + found.key(),
                                 QString (
                                     "<center>"
                                     "<h1>%0</h1>"
                                     "<h3>%1</h3>"
                                     "</center>"
                                 )
                                 .arg(simplified)
                                 .arg(phonetic));

        std::rotate(hintList.begin(), hintList.begin() + 1, hintList.end());
    }

    ui->answer->setFocus();
}

void Trainer::answerGiven()
{
    Vocable const &current = *this->current;

    QString answer = ui->answer->text();
    if (answer.isEmpty())
    {
        if (ui->btnAnswer->isEnabled())
        {
            words[this->current - words.begin()].retries = 4;
            status.skipped++;

            ui->btnAnswer->setEnabled(false);
        }
        ui->answer->setText(current.translations.front());
    }
    else
    {
        if (!correct(ui->answer->text(), current))
        {
            if (!ui->btnAnswer->isEnabled())
            {
                ui->answer->setText(current.translations.front());
            }
            else
            {
                words[this->current - words.begin()].retries = 2;
                status.mistakes++;
                updateStatus();

                ui->answer->selectAll();
                ui->question->setText("<font color='red'>" + question() + "</font>");
            }
        }
        else
        {
            words[this->current - words.begin()].retries--;
            status.correct++;

            ui->btnAnswer->setEnabled(true);
            ui->answer->setText("");

            nextWord();
        }
    }
}

void Trainer::updateStatus()
{
    ui->status->setText(QString (
            " Correct: <font color='green'><strong>%0</strong></font>"
            " Mistakes: <font color='red'><strong>%1</strong></font>"
            " Skipped: <font color='yellow'><strong>%2</strong></font>"
            ).arg(status.correct - status.skipped)
             .arg(status.mistakes)
             .arg(status.skipped));
    ui->statusBar->setText(QString("%0 / %1 words done").arg(current - words.begin()).arg(words.size()));
    ui->progress->setValue(current - words.begin());
}

bool Trainer::correct(QString answer, Vocable const &voc)
{
    if (reverseMode)
    {
        if (answer == voc.word)
            return true;
        else
        {
            QString pinyin = voc.phonetic;
            for (size_t i = 0; i < array_size(accentMap); i++)
                pinyin.replace(accentMap[i].accented, accentMap[i].normal);
            return answer.compare(pinyin, Qt::CaseInsensitive) == 0;
        }
    }

    foreach (QString const &candidate, voc.translations)
        if (answer.compare(candidate, Qt::CaseInsensitive) == 0)
            return true;

    return false;
}

static int
percent (int x, int y)
{
  return y ? x * 100 / y : 100;
}

void Trainer::reset()
{
    if (status.correct || status.mistakes || status.skipped)
    {
        updateStatus();
        QMessageBox::information(this, "Finished", QString (
                "Correct: <font color='green'><strong>%0</strong></font><br/>"
                "Mistakes: <font color='red'><strong>%1</strong></font><br/>"
                "Skipped: <font color='yellow'><strong>%2</strong></font><br/>"
                "<br/>"
                "Knowledge: %3%"
#if 0
                "Accuracy: %4%"
#endif
                ).arg(status.correct - status.skipped)
                 .arg(status.mistakes)
                 .arg(status.skipped)
                 .arg(percent (status.correct - status.skipped, status.correct))
#if 0
                 .arg(100 - percent (status.mistakes, status.correct - status.skipped))
#endif
                );
    }

    std::random_shuffle(words.begin(), words.end());
    current = words.begin();
    status.correct = 0;
    status.mistakes = 0;
    status.skipped = 0;

    if (!hasNextWord())
        for (int i = 0; i < words.size(); i++)
            words[i].retries = 1;
 }

bool Trainer::hasNextWord() const
{
    foreach (Vocable const &voc, words)
        if (voc.retries != 0)
            return true;

    return false;
}

void Trainer::nextWord()
{
    if (++current == words.end())
        reset();
    if (current->retries == 0)
        return nextWord();

    updateStatus();

    Vocable const &current = *this->current;

    bool haveHint = !reverseMode && hints.find(current.word) != hints.end();
    ui->btnHint->setVisible(haveHint);

    ui->question->setText(question());
}

QString Trainer::question()
{
    Vocable const &current = *this->current;

    if (reverseMode)
        return current.translations.front();
    return current.word + " (" + current.phonetic + ")";
}

void Trainer::reverseModeActivated(bool checked)
{
    reverseMode = checked;

    ui->question->setText(question());
}
