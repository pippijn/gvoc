#include "TranslationManager.h"
#include "TranslationDownloader.h"

TranslationManager::TranslationManager(TranslationDownloader &downloader, QDir cacheDir, QObject *parent)
    : QObject(parent)
    , downloader(downloader)
    , cache("translations", cacheDir)
{
    connect(&downloader, SIGNAL(failure(QString,QString,QString,QVariant,QString)), SIGNAL(failure(QString,QString,QString,QVariant,QString)));
    connect(&downloader, SIGNAL(success(QString,QString,QString,QVariant,Translation)), SLOT(translationSuccess(QString,QString,QString,QVariant,Translation)));
}


void TranslationManager::translation(QString sourceLanguage, QString targetLanguage, QString sourceText, QVariant userData)
{
    Cache::Map dictionary = cache(sourceLanguage, targetLanguage).all();
    if (dictionary.contains(sourceText))
    {
        emit success(sourceLanguage, targetLanguage, sourceText, userData, dictionary[sourceText]);
        return;
    }

    downloader.download(sourceLanguage, targetLanguage, sourceText, userData);
}

void TranslationManager::translationSuccess(QString sourceLanguage, QString targetLanguage, QString sourceText, QVariant userData, Translation entry)
{
    cache(sourceLanguage, targetLanguage).insert(sourceText, entry);
    emit success(sourceLanguage, targetLanguage, sourceText, userData, entry);
}


QList<Translation> TranslationManager::translations(QString sourceLanguage, QString targetLanguage) const
{
    return cache(sourceLanguage, targetLanguage).all().values();
}

void TranslationManager::setTranslations(QString sourceLanguage, QString targetLanguage, QList<Translation> translations)
{
    Cache::Map dictionary;
    foreach (Translation const &translation, translations)
        dictionary.insert(translation.primary.sourceText, translation);
    cache(sourceLanguage, targetLanguage).overwrite(dictionary);
}
