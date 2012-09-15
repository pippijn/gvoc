#ifndef CAPTCHAREPLYUPLOADER_H
#define CAPTCHAREPLYUPLOADER_H

#include "Downloader.h"

class CaptchaReplyUploader : public Downloader
{
    Q_OBJECT

public:
    explicit CaptchaReplyUploader(QNetworkAccessManager &manager, QObject *parent = 0);
    
    void submit(QString captchaId, QString captchaText);

private:
    void downloadFailure(QNetworkReply *reply, QNetworkReply::NetworkError error) const;
    void downloadSuccess(QNetworkReply *reply, QByteArray body) const;

signals:
    void failure(QString captchaId, QString captchaText, QString errorMessage) const;
    void success(QString captchaId, QString captchaText) const;
};

#endif // CAPTCHAREPLYUPLOADER_H
