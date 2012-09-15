#ifndef PHONETICSDOWNLOADER_H
#define PHONETICSDOWNLOADER_H

#include "Downloader.h"

class PhoneticsDownloader : public Downloader
{
    Q_OBJECT

public:
    explicit PhoneticsDownloader(QNetworkAccessManager &manager, QObject *parent = 0);

    void download(QString targetLanguage, QString targetText, QVariant userData);

private:
    void downloadFailure(QNetworkReply *reply, QNetworkReply::NetworkError error) const;
    void downloadSuccess(QNetworkReply *reply, QByteArray body) const;

signals:
    void failure(QString targetLanguage, QString targetText, QVariant userData, QString errorMessage) const;
    void success(QString targetLanguage, QString targetText, QVariant userData, QString phonetics) const;
};

#endif // PHONETICSDOWNLOADER_H
