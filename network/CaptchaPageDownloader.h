#ifndef CAPTCHAPAGEDOWNLOADER_H
#define CAPTCHAPAGEDOWNLOADER_H

#include <Downloader.h>

class CaptchaPageDownloader : public Downloader
{
    Q_OBJECT

public:
    explicit CaptchaPageDownloader(QNetworkAccessManager &manager, QObject *parent = 0);

    void download(QUrl location);

private:
    void downloadFailure(QNetworkReply *reply, QNetworkReply::NetworkError error) const;
    void downloadSuccess(QNetworkReply *reply, QByteArray body) const;

signals:
    void failure(QUrl location, QString errorMessage) const;
    void success(QUrl location, QString captchaId) const;
};

#endif // CAPTCHAPAGEDOWNLOADER_H
