#include "PhoneticsManager.h"
#include "PhoneticsDownloader.h"

PhoneticsManager::PhoneticsManager(PhoneticsDownloader &downloader, QDir cacheDir, QObject *parent)
    : QObject(parent)
    , downloader(downloader)
    , cache("phonetics", cacheDir)
{
    connect(&downloader, SIGNAL(failure(QString,QString,QVariant,QString)), SIGNAL(failure(QString,QString,QVariant,QString)));
    connect(&downloader, SIGNAL(success(QString,QString,QVariant,QString)), SLOT(phoneticSuccess(QString,QString,QVariant,QString)));
}


void PhoneticsManager::phonetic(QString targetLanguage, QString targetText, QVariant userData)
{
    Cache::Map dictionary = cache(targetLanguage).all();
    if (dictionary.contains(targetText))
    {
        emit success(targetLanguage, targetText, userData, dictionary[targetText]);
        return;
    }

    downloader.download(targetLanguage, targetText, userData);
}

void PhoneticsManager::phoneticSuccess(QString targetLanguage, QString targetText, QVariant userData, QString phonetic)
{
    cache(targetLanguage).insert(targetText, phonetic);
    emit success(targetLanguage, targetText, userData, phonetic);
}


QMap<QString, QString> PhoneticsManager::phonetics(QString targetLanguage) const
{
    return cache(targetLanguage).all();
}

void PhoneticsManager::setPhonetics(QString targetLanguage, QMap<QString, QString> phonetics)
{
    cache(targetLanguage).overwrite(phonetics);
}
