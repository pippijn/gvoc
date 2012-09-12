#ifndef TEXTTOSPEECH_H
#define TEXTTOSPEECH_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "AudioPlayer.h"

class TextToSpeechPrivate;

class TextToSpeech : public QObject
{
    Q_OBJECT
public:
    explicit TextToSpeech(QObject *parent = 0);
    ~TextToSpeech();
    
    QMap<QString, QByteArray> soundFiles(QString language) const;
    void setSoundFiles(QString language, QMap<QString, QByteArray> soundFiles);

    void prepare(QString language, QString text);
    void synthesise(QString language, QString text);

private:
    void synthesise(QString language, QString text, bool downloadOnly);

signals:
    void synthesisFinished();
    void synthesisError(QString message);

private slots:
    void soundFileError(QNetworkReply::NetworkError error);
    void soundFileDownloaded();
    void play(QByteArray data);

private:
    Q_DECLARE_PRIVATE(TextToSpeech)
    TextToSpeechPrivate *d_ptr;
};

#endif // TEXTTOSPEECH_H
