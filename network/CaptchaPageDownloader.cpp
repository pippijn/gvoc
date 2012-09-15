#include "CaptchaPageDownloader.h"

#include <QSgml.h>
#include <QDebug>

static char const *const xlatLocation = "xlatLocation";

CaptchaPageDownloader::CaptchaPageDownloader(QNetworkAccessManager &manager, QObject *parent)
    : Downloader(manager, parent)
{
}


void CaptchaPageDownloader::download(QUrl location)
{
    // Start request
    QNetworkReply *reply = get(location);

    // Set original request properties
    reply->setProperty(xlatLocation, location);
}

void CaptchaPageDownloader::downloadFailure(QNetworkReply *reply, QNetworkReply::NetworkError error) const
{
    Q_UNUSED(error);

    // Get original request properties
    QUrl location = reply->property(xlatLocation).value<QUrl>();

    // Get reply body
    QByteArray body = reply->readAll();

    QSgml sgml(QString::fromUtf8(body.constData(), body.size()));
    QList<QSgmlTag*> inputs = sgml.getElementsByName("input", "name", "id");
    Q_ASSERT(inputs.size() == 1);

    QSgmlTag *input = inputs.front();
    QString captchaId = input->attributes["value"];

    // The captcha page sends 503, but this is the page we are interested in.
    emit success(location, captchaId);
}

void CaptchaPageDownloader::downloadSuccess(QNetworkReply *reply, QByteArray body) const
{
    Q_UNUSED(body);

    // Get original request properties
    QUrl location = reply->property(xlatLocation).value<QUrl>();

    emit failure(location, "Unexpected success from captcha page");
}
