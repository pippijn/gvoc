#include "AudioManager.h"
#include "AudioDownloader.h"

AudioManager::AudioManager(AudioDownloader &downloader, QDir cacheDir, QObject *parent)
    : QObject(parent)
    , downloader(downloader)
    , cache("tts", cacheDir)
{
    connect(&downloader, SIGNAL(failure(QString,QString,QVariant,QString)), SIGNAL(failure(QString,QString,QVariant,QString)));
    connect(&downloader, SIGNAL(success(QString,QString,QVariant,QByteArray)), SLOT(audioSuccess(QString,QString,QVariant,QByteArray)));
}


void AudioManager::audio(QString targetLanguage, QString targetText, QVariant userData)
{
    Cache::Map dictionary = cache(targetLanguage).all();
    if (dictionary.contains(targetText))
    {
        emit success(targetLanguage, targetText, userData, dictionary[targetText]);
        return;
    }

    downloader.download(targetLanguage, targetText, userData);
}

void AudioManager::audioSuccess(QString targetLanguage, QString targetText, QVariant userData, QByteArray audio)
{
    cache(targetLanguage).insert(targetText, audio);
    emit success(targetLanguage, targetText, userData, audio);
}


QMap<QString, QByteArray> AudioManager::audio(QString targetLanguage) const
{
    return cache(targetLanguage).all();
}

void AudioManager::setAudio(QString targetLanguage, QMap<QString, QByteArray> audio)
{
    cache(targetLanguage).overwrite(audio);
}
