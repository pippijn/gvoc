#ifndef CAPTCHAIMAGEDOWNLOADER_H
#define CAPTCHAIMAGEDOWNLOADER_H

#include "Downloader.h"

#include <QImage>

class CaptchaImageDownloader : public Downloader
{
    Q_OBJECT
public:
    explicit CaptchaImageDownloader(QNetworkAccessManager &manager, QObject *parent = 0);
    
    void download(QString captchaId);

private:
    void downloadFailure(QNetworkReply *reply, QNetworkReply::NetworkError error) const;
    void downloadSuccess(QNetworkReply *reply, QByteArray body) const;

signals:
    void failure(QString captchaId, QString errorMessage) const;
    void success(QString captchaId, QImage image) const;
};

#endif // CAPTCHAIMAGEDOWNLOADER_H
