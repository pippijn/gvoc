#ifndef PHONETICSMANAGER_H
#define PHONETICSMANAGER_H

#include <QObject>
#include <QVariant>

#include "PersistentMap.h"
class PhoneticsDownloader;

class PhoneticsManager : public QObject
{
    Q_OBJECT

public:
    explicit PhoneticsManager(PhoneticsDownloader &downloader, QDir cacheDir, QObject *parent = 0);

    // Asynchronously get phonetics
    void phonetic(QString targetLanguage, QString targetText, QVariant userData);

    QMap<QString, QString> phonetics(QString targetLanguage) const;
    void setPhonetics(QString targetLanguage, QMap<QString, QString> phonetics);

private slots:
    void phoneticSuccess(QString targetLanguage, QString targetText, QVariant userData, QString phonetic);

signals:
    void failure(QString targetLanguage, QString targetText, QVariant userData, QString errorMessage) const;
    void success(QString targetLanguage, QString targetText, QVariant userData, QString phonetic) const;

private:
    typedef PersistentMap<QString, QString> Cache;

    PhoneticsDownloader &downloader;
    Cache cache;
};

#endif // PHONETICSMANAGER_H
