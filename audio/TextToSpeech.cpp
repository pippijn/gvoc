#include "TextToSpeech.h"
#include "AudioManager.h"

#include <QDebug>


TextToSpeech::TextToSpeech(AudioManager &audioManager, QObject *parent)
    : QObject(parent)
    , audioManager(audioManager)
{
    connect(&player, SIGNAL(finished()), SIGNAL(synthesisFinished()));

    connect(&audioManager, SIGNAL(failure(QString,QString,QVariant,QString)), SLOT(audioFailure(QString,QString,QVariant,QString)));
    connect(&audioManager, SIGNAL(success(QString,QString,QVariant,QByteArray)), SLOT(audioSuccess(QString,QString,QVariant,QByteArray)));
}


void TextToSpeech::prepare(QString language, QString text)
{
    synthesise(language, text, true);
}

void TextToSpeech::synthesise(QString language, QString text)
{
    synthesise(language, text, false);
}

void TextToSpeech::synthesise(QString language, QString text, bool downloadOnly)
{
    audioManager.audio(language, text, downloadOnly);
}

void TextToSpeech::audioFailure(QString targetLanguage, QString targetText, QVariant userData, QString errorMessage) const
{
    emit synthesisError(errorMessage);
}

void TextToSpeech::audioSuccess(QString targetLanguage, QString targetText, QVariant userData, QByteArray audio)
{
    bool downloadOnly = userData.value<bool>();

    if (downloadOnly)
        emit synthesisReady();
    else
        play(audio);
}

void TextToSpeech::play(QByteArray data)
{
    player.playMP3(data);
}
