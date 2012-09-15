#ifndef AUDIODOWNLOADER_H
#define AUDIODOWNLOADER_H

#include "Downloader.h"

class AudioDownloader : public Downloader
{
    Q_OBJECT

public:
    explicit AudioDownloader(QNetworkAccessManager &manager, QObject *parent = 0);

    void download(QString targetLanguage, QString targetText, QVariant userData);

private:
    void downloadFailure(QNetworkReply *reply, QNetworkReply::NetworkError error) const;
    void downloadSuccess(QNetworkReply *reply, QByteArray body) const;

signals:
    void failure(QString targetLanguage, QString targetText, QVariant userData, QString errorMessage) const;
    void success(QString targetLanguage, QString targetText, QVariant userData, QByteArray audio) const;
};

#endif // AUDIODOWNLOADER_H
