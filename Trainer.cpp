#include "Trainer.h"
#include "ui_Trainer.h"

#include "Translation.h"
#include "TextToSpeech.h"

#include <QMessageBox>
#include <QDebug>
#include <QSettings>

Trainer::Trainer(int level, QString sourceLanguage, QString targetLanguage, QList<Translation> wordList, TextToSpeech &tts, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Trainer)
    , tts(tts)
    , sourceLanguage(sourceLanguage)
    , targetLanguage(targetLanguage)
    , manager(level, sourceLanguage, targetLanguage)
{
    ui->setupUi(this);

    connect(&manager, SIGNAL(statusChanged(TrainingStatus)), SLOT(updateStatus(TrainingStatus)));
    connect(&manager, SIGNAL(mistake()), SLOT(madeMistake()));
    connect(&manager, SIGNAL(finished(TrainingStatus)), SLOT(trainingFinished(TrainingStatus)));
    connect(&manager, SIGNAL(nextQuestion()), SLOT(gotNextQuestion()));

    connect(&tts, SIGNAL(synthesisReady()), SLOT(enableListen()));

    QSettings settings;
    ui->reverseMode->setChecked(settings.value("reverseMode").value<bool>());

    manager.loadWords(wordList);
    manager.loadWords(QDir(":/wordlists"));

    QDir dir = QDir::current();
    if (dir.cd("wordlists"))
        manager.loadWords(dir);

    manager.start();
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
    ui->question->setText(QString("<font color='red'>%0</font>").arg(manager.question()));
}


void Trainer::trainingFinished(TrainingStatus status)
{
    QMessageBox::information(this, "Round complete", status.finishMessage());
    manager.start();
}


void Trainer::gotNextQuestion()
{
    int totalWords = manager.totalWords();
    int remainingWords = manager.remainingWords();
    ui->progress->setMaximum(totalWords);
    ui->progress->setValue(remainingWords);
    ui->statusBar->setText(QString("%0 / %1").arg(remainingWords).arg(totalWords));

    ui->question->setText(manager.question());

    ui->answer->setText(QString());
    ui->answer->setEnabled(true);
    ui->answer->setFocus();

    ui->reverseMode->setEnabled(true);

    ui->btnAnswer->setEnabled(true);
    ui->btnHint->setEnabled(manager.hasHint());
    ui->btnListen->setEnabled(false);
    ui->btnOK->setEnabled(true);

    tts.prepare(manager.questionLanguage(), manager.questionText());
}


void Trainer::enableListen()
{
    ui->btnListen->setEnabled(true);
}


void Trainer::on_reverseMode_toggled(bool checked)
{
    manager.toggleLanguageMode();
}

void Trainer::on_btnListen_clicked()
{
    tts.synthesise(manager.questionLanguage(), manager.questionText());
}

void Trainer::on_btnHint_clicked()
{
    QMessageBox::information(this, tr("Hint for %0").arg(manager.question()), manager.hint());
    manager.rotateHints();
}

void Trainer::on_btnAnswer_clicked()
{
    ui->answer->setText(manager.answer());
    ui->answer->setEnabled(false);
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
        manager.giveAnswer(answer);
    else
        manager.skip();
}
