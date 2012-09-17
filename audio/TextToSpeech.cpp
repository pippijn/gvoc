#include "TextToSpeech.h"
#include "AudioManager.h"

#include <QDebug>


TextToSpeech::TextToSpeech(AudioManager &audioManager, QObject *parent)
    : QObject(parent)
    , audioManager(audioManager)
{
    connect(&player, SIGNAL(finished()), SLOT(playFinished()));

    connect(&audioManager, SIGNAL(failure(QString,QString,QVariant,QString)), SLOT(audioFailure(QString,QString,QVariant,QString)));
    connect(&audioManager, SIGNAL(success(QString,QString,QVariant,QByteArray)), SLOT(audioSuccess(QString,QString,QVariant,QByteArray)));
}


void TextToSpeech::queue(QString language, QString text)
{
    ttsQueue.append(QueuedText(language, text));
}

void TextToSpeech::runQueue()
{
    synthesise(ttsQueue.takeFirst(), false);
}

void TextToSpeech::prepare(QString language, QString text)
{
    synthesise(QueuedText(language, text), true);
}

void TextToSpeech::synthesise(QString language, QString text)
{
    synthesise(QueuedText(language, text), false);
}

void TextToSpeech::synthesise(QueuedText const &text, bool downloadOnly)
{
    audioManager.audio(text.language, text.text, downloadOnly);
}

void TextToSpeech::audioFailure(QString targetLanguage, QString targetText, QVariant userData, QString errorMessage) const
{
    emit synthesisFailure(targetLanguage, targetText, errorMessage);
}

void TextToSpeech::audioSuccess(QString targetLanguage, QString targetText, QVariant userData, QByteArray audio)
{
    bool downloadOnly = userData.value<bool>();

    if (downloadOnly)
        emit synthesisReady(targetLanguage, targetText);
    else
        play(audio);
}

void TextToSpeech::play(QByteArray data)
{
    player.playMP3(data);
}

void TextToSpeech::playFinished()
{
    if (!ttsQueue.isEmpty())
        synthesise(ttsQueue.takeFirst(), false);
    else
        emit synthesisSuccess();
}
