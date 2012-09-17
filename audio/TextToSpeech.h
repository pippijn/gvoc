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

    struct QueuedText
    {
        QString language;
        QString text;

        QueuedText(QString language, QString text)
            : language(language)
            , text(text)
        {
        }
    };

public:
    explicit TextToSpeech(AudioManager &audioManager, QObject *parent = 0);
    
    void queue(QString language, QString text);
    void runQueue();

    void prepare(QString language, QString text);
    void synthesise(QString language, QString text);

private:
    void synthesise(QueuedText const &text, bool downloadOnly);

signals:
    void synthesisFailure(QString language, QString text, QString message) const;
    void synthesisReady(QString language, QString text);
    void synthesisSuccess();

private slots:
    void audioFailure(QString targetLanguage, QString targetText, QVariant userData, QString errorMessage) const;
    void audioSuccess(QString targetLanguage, QString targetText, QVariant userData, QByteArray audio);

    void play(QByteArray data);
    void playFinished();

private:
    AudioManager &audioManager;
    AudioPlayer player;
    QList<QueuedText> ttsQueue;
};

#endif // TEXTTOSPEECH_H
