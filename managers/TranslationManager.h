#ifndef TRANSLATIONMANAGER_H
#define TRANSLATIONMANAGER_H

#include <QObject>

#include "PersistentMap.h"
#include "Translation.h"
class TranslationDownloader;

class TranslationManager : public QObject
{
    Q_OBJECT

public:
    explicit TranslationManager(TranslationDownloader &downloader, QDir cacheDir, QObject *parent = 0);

    // Asynchronously get a translation
    void translation(QString sourceLanguage, QString targetLanguage, QString sourceText, QVariant userData = QVariant());

    QList<Translation> translations(QString sourceLanguage, QString targetLanguage) const;
    void setTranslations(QString sourceLanguage, QString targetLanguage, QList<Translation> translations);

private slots:
    void translationSuccess(QString sourceLanguage, QString targetLanguage, QString sourceText, QVariant userData, Translation entry);

signals:
    void failure(QString sourceLanguage, QString targetLanguage, QString sourceText, QVariant userData, QString errorMessage) const;
    void success(QString sourceLanguage, QString targetLanguage, QString sourceText, QVariant userData, Translation entry) const;

private:
    typedef PersistentMap<QString, Translation> Cache;

    TranslationDownloader &downloader;
    Cache cache;
};

#endif // TRANSLATIONMANAGER_H
