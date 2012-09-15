#include "CaptchaReplyUploader.h"

#include <QDebug>

static char const *const xlatCaptchaId = "xlatCaptchaId";
static char const *const xlatCaptchaText = "xlatCaptchaText";

CaptchaReplyUploader::CaptchaReplyUploader(QNetworkAccessManager &manager, QObject *parent)
    : Downloader(manager, parent)
{
}


void CaptchaReplyUploader::submit(QString captchaId, QString captchaText)
{
    // Start request
    QNetworkReply *reply = get(QString("http://www.google.com/sorry/Captcha?id=%0&captcha=%1")
                               .arg(captchaId, captchaText));

    // Set original request properties
    reply->setProperty(xlatCaptchaId, captchaId);
    reply->setProperty(xlatCaptchaText, captchaText);
}

void CaptchaReplyUploader::downloadFailure(QNetworkReply *reply, QNetworkReply::NetworkError error) const
{
    // Get original request properties
    QString captchaId = reply->property(xlatCaptchaId).value<QString>();
    QString captchaText = reply->property(xlatCaptchaText).value<QString>();

    emit failure(captchaId, captchaText, reply->errorString());
}

void CaptchaReplyUploader::downloadSuccess(QNetworkReply *reply, QByteArray body) const
{
    // Get original request properties
    QString captchaId = reply->property(xlatCaptchaId).value<QString>();
    QString captchaText = reply->property(xlatCaptchaText).value<QString>();

    emit success(captchaId, captchaText);
}
