#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

#include "TextToSpeech.h"
#include "Translator.h"
#include "TranslationModel.h"

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

private slots:
    void on_input_returnPressed();
    void translateSuccess(Translation translation);
    void translateError(QString message);

    void on_detailed_activated(QModelIndex const &index);

    void on_action_Tools_Trainer_triggered();
    void on_action_Tools_Options_triggered();

    void on_listen_clicked();
    void ttsFinished();
    void ttsError(QString message);

    void on_swapLanguages_clicked();

    void on_action_Edit_WordList_triggered();

    void on_action_Edit_Phonetics_triggered();

    void on_action_Edit_TextToSpeech_triggered();

    void on_action_Help_About_triggered();

private:
    Ui::MainWindow *ui;
    QSettings settings;
    TranslationModel translationModel;
    TextToSpeech tts;
};

#endif // MAINWINDOW_H
