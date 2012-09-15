#include "CaptchaImageDownloader.h"

#include <QDebug>
#include <QBuffer>
#include <QImageReader>

static char const *const xlatCaptchaId = "xlatCaptchaId";

CaptchaImageDownloader::CaptchaImageDownloader(QNetworkAccessManager &manager, QObject *parent)
    : Downloader(manager, parent)
{
}


void CaptchaImageDownloader::download(QString captchaId)
{
    // Start request
    QNetworkReply *reply = get(QString("http://www.google.com/sorry/image?id=%0").arg(captchaId));

    // Set original request properties
    reply->setProperty(xlatCaptchaId, captchaId);
}

void CaptchaImageDownloader::downloadFailure(QNetworkReply *reply, QNetworkReply::NetworkError error) const
{
    // Get original request properties
    QString captchaId = reply->property(xlatCaptchaId).value<QString>();

    emit failure(captchaId, reply->errorString());
}

void CaptchaImageDownloader::downloadSuccess(QNetworkReply *reply, QByteArray body) const
{
    // Get original request properties
    QString captchaId = reply->property(xlatCaptchaId).value<QString>();

    QBuffer buffer(&body);
    QImageReader reader(&buffer);
    QImage image;
    if (reader.read(&image))
        emit success(captchaId, image);
    else
        emit failure(captchaId, reader.errorString());
}
