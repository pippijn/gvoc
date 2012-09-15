#ifndef TEXTTOSPEECH_H
#define TEXTTOSPEECH_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "AudioPlayer.h"
class AudioManager;

class TextToSpeechPrivate;

class TextToSpeech : public QObject
{
    Q_OBJECT

public:
    explicit TextToSpeech(AudioManager &audioManager, QObject *parent = 0);
    
    void prepare(QString language, QString text);
    void synthesise(QString language, QString text);

private:
    void synthesise(QString language, QString text, bool downloadOnly);

signals:
    void synthesisReady();
    void synthesisFinished();
    void synthesisError(QString message) const;

private slots:
    void audioFailure(QString targetLanguage, QString targetText, QVariant userData, QString errorMessage) const;
    void audioSuccess(QString targetLanguage, QString targetText, QVariant userData, QByteArray audio);

    void play(QByteArray data);

private:
    AudioManager &audioManager;
    AudioPlayer player;
};

#endif // TEXTTOSPEECH_H
