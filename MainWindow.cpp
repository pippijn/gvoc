#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Trainer.h"
#include "OptionsDialog.h"
#include "WordListEditor.h"
#include "PhoneticsEditor.h"
#include "TextToSpeechEditor.h"
#include "PhoneticsDialog.h"

#include <QSettings>
#include <QDebug>
#include <QMessageBox>


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

    connect(&tts, SIGNAL(synthesisSuccess()), SLOT(ttsSuccess()));
    connect(&tts, SIGNAL(synthesisFailure(QString,QString,QString)), SLOT(ttsFailure(QString,QString,QString)));

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
    ui->sourceLanguage->setText(languageManager.languageName(translationModel.sourceLanguage()));
    ui->targetLanguage->setText(languageManager.languageName(translationModel.targetLanguage()));
}

int MainWindow::minLevel() const
{
    return settings.value("minLevel", 0).value<int>();
}
void MainWindow::setMinLevel(int minLevel)
{
    settings.setValue("minLevel", minLevel);
}

int MainWindow::maxLevel() const
{
    return settings.value("maxLevel", 0).value<int>();
}
void MainWindow::setMaxLevel(int maxLevel)
{
    settings.setValue("maxLevel", maxLevel);
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
    ui->statusBar->showMessage(message, 5000);
    ui->input->setEnabled(true);
    ui->input->setFocus();
}


void MainWindow::on_action_Tools_Trainer_triggered()
{
    ui->statusBar->showMessage("Starting trainer...");

    Trainer trainer(minLevel(), maxLevel(),
                    translationModel.sourceLanguage(),
                    translationModel.targetLanguage(),
                    translationModel.wordList(),
                    tts, phoneticsManager);

    this->setVisible(false);
    trainer.exec();
    this->setVisible(true);

    ui->statusBar->clearMessage();
}

void MainWindow::on_action_Tools_Options_triggered()
{
    OptionsDialog options(minLevel(), maxLevel(),
                          translationModel.sourceLanguage(),
                          translationModel.targetLanguage(),
                          languageManager);
    if (options.exec() == QDialog::Accepted)
    {
        setLanguages(options.sourceLanguage(),
                     options.targetLanguage());
        setMinLevel(options.minLevel());
        setMaxLevel(options.maxLevel());
    }
}

void MainWindow::on_listen_clicked()
{
    ui->listen->setEnabled(false);
    tts.synthesise(translationModel.targetLanguage(), ui->translatedText->text());
}

void MainWindow::ttsFailure(QString language, QString text, QString message)
{
    ui->statusBar->showMessage(message, 10000);
    ui->listen->setEnabled(true);
}

void MainWindow::ttsSuccess()
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

void MainWindow::on_action_Tools_Phonetics_triggered()
{
    PhoneticsDialog dialog(phoneticsManager, translationModel.targetLanguage());
    dialog.exec();
}
