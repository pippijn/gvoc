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

    , translationDownloader(networkManager)
    , translationManager(translationDownloader, QDir::current())

    , phoneticsDownloader(networkManager)
    , phoneticsManager(phoneticsDownloader, QDir::current())

    , audioDownloader(networkManager)
    , audioManager(audioDownloader, QDir::current())

    , translationModel(translationManager, phoneticsManager,
                       settings.value("sourceLanguage", "en").toString(),
                       settings.value("targetLanguage", "zh-CN").toString())
    , tts(audioManager)
{
    ui->setupUi(this);

    ui->detailed->setModel(&translationModel);
    ui->input->setFocus();

    connect(&translationModel, SIGNAL(translateSuccess(Translation)), SLOT(translateSuccess(Translation)));
    connect(&translationModel, SIGNAL(translateFailure(QString)), SLOT(translateFailure(QString)));

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

void MainWindow::on_input_editingFinished()
{
    QString text = ui->input->text();
    if (text.isEmpty() || text == ui->sourceText->text())
        return;
    ui->sourceText->setText(text);

    ui->input->setEnabled(false);
    translationModel.translate(text);
}

void MainWindow::on_input_returnPressed()
{
    on_input_editingFinished();
}

void MainWindow::translateSuccess(Translation translation)
{
    PrimaryTranslation const &primary = translation.primary;
    ui->sourceText->setText(primary.sourceText);
    ui->sourcePhonetic->setText(primary.sourcePhonetic);
    ui->translatedText->setText(primary.translated);
    ui->translatedPhonetic->setText(primary.translatedPhonetic);

    ui->input->setEnabled(true);
    ui->input->setFocus();

    ui->listen->setEnabled(!primary.translated.isEmpty());
    if (!translation.dictionary.isEmpty())
        tts.prepare(translationModel.targetLanguage(), ui->translatedText->text());
}

void MainWindow::translateFailure(QString message)
{
    ui->statusBar->showMessage(message, 10000);
    ui->input->setEnabled(true);
    ui->input->setFocus();
}


void MainWindow::on_action_Tools_Trainer_triggered()
{
    Trainer trainer(settings.value("level", 0).value<int>(),
                    translationModel.sourceLanguage(),
                    translationModel.targetLanguage(),
                    translationModel.wordList(),
                    tts);

    this->setVisible(false);
    trainer.exec();
    this->setVisible(true);
}

void MainWindow::on_action_Tools_Options_triggered()
{
    OptionsDialog options(settings.value("level", 0).value<int>(),
                          translationModel.sourceLanguage(),
                          translationModel.targetLanguage());
    if (options.exec() == QDialog::Accepted)
    {
        setLanguages(options.sourceLanguage(),
                     options.targetLanguage());
        settings.setValue("level", options.level());
    }
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
    ui->statusBar->showMessage(message, 10000);
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

    QMap<QString, QByteArray> cache = audioManager.audio(language);
    TextToSpeechEditor editor(cache);
    if (editor.exec() == QDialog::Accepted)
        audioManager.setAudio(language, editor.soundFiles());
}

void MainWindow::on_action_Help_About_triggered()
{
    QMessageBox::information(this, "About GVoc", QString::fromUtf8("Copyright © 2012 Pippijn van Steenhoven"));
}

void MainWindow::on_detailed_activated(const QModelIndex &index)
{
    if (index.parent().isValid())
        tts.synthesise(translationModel.targetLanguage(), index.sibling(index.row(), 0).data().value<QString>());
}
