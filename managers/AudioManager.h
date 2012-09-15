#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QObject>
#include <QVariant>

#include "PersistentMap.h"
class AudioDownloader;

class AudioManager : public QObject
{
    Q_OBJECT

public:
    explicit AudioManager(AudioDownloader &downloader, QDir cacheDir, QObject *parent = 0);

    // Asynchronously get audio
    void audio(QString targetLanguage, QString targetText, QVariant userData);

    QMap<QString, QByteArray> audio(QString targetLanguage) const;
    void setAudio(QString targetLanguage, QMap<QString, QByteArray> audio);

private slots:
    void audioSuccess(QString targetLanguage, QString targetText, QVariant userData, QByteArray audio);

signals:
    void failure(QString targetLanguage, QString targetText, QVariant userData, QString errorMessage) const;
    void success(QString targetLanguage, QString targetText, QVariant userData, QByteArray audio) const;

private:
    typedef PersistentMap<QString, QByteArray> Cache;

    AudioDownloader &downloader;
    Cache cache;
};

#endif // AUDIOMANAGER_H
