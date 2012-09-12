#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Trainer.h"
#include "OptionsDialog.h"
#include "WordListEditor.h"
#include "PhoneticsEditor.h"
#include "TextToSpeechEditor.h"

#include <QSettings>
#include <QDebug>
#include <QMessageBox>

static QString languageName(QString code)
{
    if (code == "zh-CN") return "Chinese";
    if (code == "nl")    return "Dutch";
    if (code == "en")    return "English";
    if (code == "fr")    return "French";
    if (code == "de")    return "German";
    if (code == "it")    return "Italian";
    if (code == "ja")    return "Japanese";
    if (code == "no")    return "Norwegian";
    if (code == "pt")    return "Portuguese";
    if (code == "ru")    return "Russian";
    if (code == "es")    return "Spanish";
    if (code == "sw")    return "Swedish";
    if (code == "th")    return "Thai";
    if (code == "tr")    return "Turkish";

    return "<unknown>";
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , translationModel(settings.value("sourceLanguage", "en").toString(),
                       settings.value("targetLanguage", "zh-CN").toString())
{
    ui->setupUi(this);

    ui->detailed->setModel(&translationModel);
    ui->input->setFocus();

    connect(&translationModel, SIGNAL(updated(Translation)), SLOT(translateSuccess(Translation)));
    connect(&translationModel, SIGNAL(translateError(QString)), SLOT(translateError(QString)));

    connect(&tts, SIGNAL(synthesisFinished()), SLOT(ttsFinished()));
    connect(&tts, SIGNAL(synthesisError(QString)), SLOT(ttsError(QString)));

    updateLanguageLabels();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setLanguages(QString sourceLanguage, QString targetLanguage)
{
    translationModel.setSourceLanguage(sourceLanguage);
    translationModel.setTargetLanguage(targetLanguage);

    settings.setValue("sourceLanguage", sourceLanguage);
    settings.setValue("targetLanguage", targetLanguage);

    updateLanguageLabels();
}

void MainWindow::updateLanguageLabels()
{
    ui->sourceLanguage->setText(languageName(translationModel.sourceLanguage()));
    ui->targetLanguage->setText(languageName(translationModel.targetLanguage()));
}

void MainWindow::on_input_returnPressed()
{
    ui->input->setEnabled(false);
    translationModel.translate(ui->input->text());
}

void MainWindow::translateSuccess(Translation translation)
{
    foreach (Translation::Primary const &primary, translation.primary)
    {
        ui->sourceText->setText(primary.sourceText);
        ui->sourcePhonetic->setText(primary.sourcePhonetic);
        ui->translatedText->setText(primary.translated);
        ui->translatedPhonetic->setText(primary.translatedPhonetic);
    }

    ui->input->setEnabled(true);
    ui->input->setFocus();

    QString translated = ui->translatedText->text();
    ui->listen->setEnabled(!translated.isEmpty());
    if (!translation.dictionary.isEmpty())
        tts.prepare(translationModel.targetLanguage(), ui->translatedText->text());
}

void MainWindow::translateError(QString message)
{
    ui->input->setEnabled(true);
    ui->input->setFocus();
}

void MainWindow::on_detailed_activated(const QModelIndex &index)
{
    if (index.parent().isValid())
        translationModel.updatePhonetic(index);
}


void MainWindow::on_action_Tools_Trainer_triggered()
{
    Trainer trainer(translationModel.sourceLanguage(),
                    translationModel.targetLanguage(),
                    translationModel.wordList());

    this->setVisible(false);
    trainer.exec();
    this->setVisible(true);
}

void MainWindow::on_action_Tools_Options_triggered()
{
    OptionsDialog options(translationModel.sourceLanguage(), translationModel.targetLanguage());
    if (options.exec() == QDialog::Accepted)
        setLanguages(options.sourceLanguage(),
                     options.targetLanguage());
}

void MainWindow::on_listen_clicked()
{
    ui->listen->setEnabled(false);
    tts.synthesise(translationModel.targetLanguage(), ui->translatedText->text());
}

void MainWindow::ttsFinished()
{
    ui->listen->setEnabled(true);
}

void MainWindow::ttsError(QString message)
{
    ui->listen->setEnabled(true);
}

void MainWindow::on_swapLanguages_clicked()
{
    setLanguages(translationModel.targetLanguage(), translationModel.sourceLanguage());
}

void MainWindow::on_action_Edit_WordList_triggered()
{
    WordListEditor editor(translationModel.wordList());
    if (editor.exec() == QDialog::Accepted)
        translationModel.setWordList(editor.wordList());
}

void MainWindow::on_action_Edit_Phonetics_triggered()
{
    PhoneticsEditor editor(translationModel.phonetics());
    if (editor.exec() == QDialog::Accepted)
        translationModel.setPhonetics(editor.phonetics());
}

void MainWindow::on_action_Edit_TextToSpeech_triggered()
{
    QString language = translationModel.targetLanguage();

    QMap<QString, QByteArray> cache = tts.soundFiles(language);
    TextToSpeechEditor editor(cache);
    if (editor.exec() == QDialog::Accepted)
        tts.setSoundFiles(language, editor.soundFiles());
}

void MainWindow::on_action_Help_About_triggered()
{
    QMessageBox::information(this, "About GVoc", QString::fromUtf8("Copyright © 2012 Pippijn van Steenhoven & 周丽娜"));
}
