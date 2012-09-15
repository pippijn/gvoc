#include "AudioDownloader.h"
#include "CaptchaHandler.h"

#include <QDebug>

static char const *const xlatSourceText = "xlatSourceText";
static char const *const xlatTargetText = "xlatTargetText";
static char const *const xlatSourceLanguage = "xlatSourceLanguage";
static char const *const xlatTargetLanguage = "xlatTargetLanguage";
static char const *const xlatUserData = "xlatUserData";

AudioDownloader::AudioDownloader(QNetworkAccessManager &manager, QObject *parent)
    : Downloader(manager, parent)
{
}


void AudioDownloader::download(QString targetLanguage, QString targetText, QVariant userData)
{
    // Start request
    QNetworkReply *reply = get("/translate_tts", Query()
                               << QueryItem("ie", "UTF-8")
                               << QueryItem("text", targetText)
                               << QueryItem("tl", targetLanguage));

    // Set original request properties
    reply->setProperty(xlatTargetLanguage, targetLanguage);
    reply->setProperty(xlatTargetText, targetText);
    reply->setProperty(xlatUserData, userData);
}

void AudioDownloader::downloadFailure(QNetworkReply *reply, QNetworkReply::NetworkError error) const
{
    // Get original request properties
    QString targetLanguage = reply->property(xlatTargetLanguage).value<QString>();
    QString targetText = reply->property(xlatTargetText).value<QString>();
    QVariant userData = reply->property(xlatUserData);

    // Emit error signal
    emit failure(targetLanguage, targetText, userData,
                 QString("Network error %0 while downloading audio: %1")
                 .arg(error)
                 .arg(reply->errorString()));
}

void AudioDownloader::downloadSuccess(QNetworkReply *reply, QByteArray body) const
{
    // Get original request properties
    QString targetLanguage = reply->property(xlatTargetLanguage).value<QString>();
    QString targetText = reply->property(xlatTargetText).value<QString>();
    QVariant userData = reply->property(xlatUserData);

    switch (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).value<int>())
    {
    case 200:
        emit success(targetLanguage, targetText, userData, body);
        break;

    case 302:
        // TODO: find a better place for this
        CaptchaHandler *handler = new CaptchaHandler(http, const_cast<AudioDownloader *>(this));
        handler->handle(reply->header(QNetworkRequest::LocationHeader).value<QUrl>());
        emit failure(targetLanguage, targetText, userData,
                     QString("Unable to download audio: rate limited"));
        break;
    }
}
