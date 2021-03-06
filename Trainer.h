#ifndef TRAINER_H
#define TRAINER_H

#include <QDialog>
#include <QStringList>
#include <QMap>

#include "TrainingController.h"
class Translation;
class TextToSpeech;

namespace Ui {
    class Trainer;
}

class Trainer : public QDialog {
    Q_OBJECT

public:
    Trainer(int minLevel, int maxLevel,
            QString sourceLanguage, QString targetLanguage,
            QList<Translation> wordList,
            TextToSpeech &tts,
            PhoneticsManager &phoneticsManager,
            QWidget *parent = 0);
    ~Trainer();

private slots:
    void updateStatus(TrainingStatus status);
    void madeMistake();
    void trainingFinished(TrainingStatus status);
    void gotNextQuestion();
    void enableListen();

    void on_reverseMode_toggled(bool checked);

    void on_btnListen_clicked();
    void on_btnHint_clicked();
    void on_btnAnswer_clicked();
    void on_btnOK_clicked();
    void on_btnTranslations_clicked();

private: // data
    Ui::Trainer *ui;
    TextToSpeech &tts;
    QString sourceLanguage;
    QString targetLanguage;
    TrainingController trainingManager;
};

#endif // TRAINER_H
