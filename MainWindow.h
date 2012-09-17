#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

#include "LanguageManager.h"
#include "TextToSpeech.h"
#include "Translator.h"
#include "TranslationModel.h"

#include "PhoneticsDownloader.h"
#include "PhoneticsManager.h"

#include "TranslationDownloader.h"
#include "TranslationManager.h"

#include "AudioDownloader.h"
#include "AudioManager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void setLanguages(QString sourceLanguage, QString targetLanguage);
    void updateLanguageLabels();

    int minLevel() const;
    void setMinLevel(int minLevel);
    int maxLevel() const;
    void setMaxLevel(int maxLevel);

private slots:
    void on_input_editingFinished();
    void on_input_returnPressed();
    void translateSuccess(Translation translation);
    void translateFailure(QString message);

    void on_action_Tools_Trainer_triggered();
    void on_action_Tools_Options_triggered();

    void on_listen_clicked();
    void ttsFailure(QString language, QString text, QString message);
    void ttsSuccess();

    void on_swapLanguages_clicked();

    void on_action_Edit_WordList_triggered();
    void on_action_Edit_Phonetics_triggered();
    void on_action_Edit_TextToSpeech_triggered();

    void on_action_Help_About_triggered();

    void on_detailed_activated(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QSettings settings;
    QNetworkAccessManager networkManager;

    TranslationDownloader translationDownloader;
    TranslationManager translationManager;

    PhoneticsDownloader phoneticsDownloader;
    PhoneticsManager phoneticsManager;

    AudioDownloader audioDownloader;
    AudioManager audioManager;

    TranslationModel translationModel;
    TextToSpeech tts;

    LanguageManager languageManager;
};

#endif // MAINWINDOW_H
