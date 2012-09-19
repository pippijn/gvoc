#include "Trainer.h"
#include "ui_Trainer.h"

#include "Translation.h"
#include "TextToSpeech.h"

#include <QMessageBox>
#include <QDebug>
#include <QSettings>

Trainer::Trainer(int minLevel, int maxLevel,
                 QString sourceLanguage, QString targetLanguage,
                 QList<Translation> wordList,
                 TextToSpeech &tts,
                 PhoneticsManager &phoneticsManager,
                 QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Trainer)
    , tts(tts)
    , sourceLanguage(sourceLanguage)
    , targetLanguage(targetLanguage)
    , trainingManager(minLevel, maxLevel, sourceLanguage, targetLanguage, phoneticsManager)
{
    ui->setupUi(this);

    connect(&trainingManager, SIGNAL(statusChanged(TrainingStatus)), SLOT(updateStatus(TrainingStatus)));
    connect(&trainingManager, SIGNAL(mistake()), SLOT(madeMistake()));
    connect(&trainingManager, SIGNAL(finished(TrainingStatus)), SLOT(trainingFinished(TrainingStatus)));
    connect(&trainingManager, SIGNAL(nextQuestion()), SLOT(gotNextQuestion()));

    connect(&tts, SIGNAL(synthesisReady(QString,QString)), SLOT(enableListen()));
    connect(&tts, SIGNAL(synthesisSuccess()), SLOT(enableListen()));

    QSettings settings;
    ui->reverseMode->setChecked(settings.value("reverseMode").value<bool>());

    trainingManager.loadWords(wordList);

    QStringList const dirs = QStringList()
            << ":/wordlists"
            << "./wordlists";

    foreach (QString const &dir, dirs)
        trainingManager.loadWords(QDir(dir));
    foreach (QString const &dir, dirs)
        trainingManager.loadHints(QDir(dir));

    trainingManager.start();
}

Trainer::~Trainer()
{
    QSettings settings;
    settings.setValue("reverseMode", ui->reverseMode->isChecked());
    delete ui;
}


void Trainer::updateStatus(TrainingStatus status)
{
    ui->status->setText(status.statusMessage());
}


void Trainer::madeMistake()
{
    ui->question->setText(QString("<font color='red'>%0</font>").arg(trainingManager.question()));
}


void Trainer::trainingFinished(TrainingStatus status)
{
    QMessageBox::information(this, "Round complete", status.finishMessage());
    trainingManager.start();
}


void Trainer::gotNextQuestion()
{
    int totalWords = trainingManager.totalWords();
    int remainingWords = trainingManager.remainingWords();
    ui->progress->setMaximum(totalWords);
    ui->progress->setValue(remainingWords);
    ui->statusBar->setText(QString("%0 / %1").arg(remainingWords).arg(totalWords));

    ui->question->setText(trainingManager.question());

    ui->answer->setText(QString());
    ui->answer->setEnabled(true);
    ui->answer->setFocus();

    ui->reverseMode->setEnabled(true);

    ui->btnTranslations->setEnabled(false);
    ui->btnAnswer->setEnabled(true);
    ui->btnHint->setEnabled(trainingManager.hasHint());
    ui->btnListen->setEnabled(false);
    ui->btnOK->setEnabled(true);

    tts.prepare(trainingManager.questionLanguage(), trainingManager.questionText());
}


void Trainer::enableListen()
{
    ui->btnListen->setEnabled(true);
}


void Trainer::on_reverseMode_toggled(bool checked)
{
    trainingManager.toggleLanguageMode();
}

void Trainer::on_btnListen_clicked()
{
    ui->btnListen->setEnabled(false);
    tts.queue(trainingManager.questionLanguage(), trainingManager.questionText());

    if (!ui->btnAnswer->isEnabled())
        tts.queue(trainingManager.answerLanguage(), trainingManager.answerText());

    tts.runQueue();
}

void Trainer::on_btnHint_clicked()
{
    bool withTranslation = !ui->btnAnswer->isEnabled();
    QString hint = trainingManager.hint(withTranslation);
    trainingManager.rotateHints();
    QMessageBox::information(this, tr("Hint for %0").arg(trainingManager.question()), hint);
}

void Trainer::on_btnAnswer_clicked()
{
    ui->answer->setText(trainingManager.answerText());
    ui->answer->setEnabled(false);
    ui->btnTranslations->setEnabled(true);
    ui->btnAnswer->setEnabled(false);
}

void Trainer::on_btnOK_clicked()
{
    QString answer = ui->answer->text();

    if (answer.isEmpty())
    {
        on_btnAnswer_clicked();
        return;
    }

    if (ui->btnAnswer->isEnabled())
        trainingManager.giveAnswer(answer);
    else
        trainingManager.skip();
}

void Trainer::on_btnTranslations_clicked()
{
    QMessageBox::information(this, "Other translations", trainingManager.answerOptions().join("<br/>"));
}
