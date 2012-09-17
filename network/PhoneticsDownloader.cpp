#include "PhoneticsDownloader.h"

#include <QDebug>

static char const *const xlatSourceText = "xlatSourceText";
static char const *const xlatTargetText = "xlatTargetText";
static char const *const xlatSourceLanguage = "xlatSourceLanguage";
static char const *const xlatTargetLanguage = "xlatTargetLanguage";
static char const *const xlatUserData = "xlatUserData";

PhoneticsDownloader::PhoneticsDownloader(QNetworkAccessManager &manager, QObject *parent)
    : Downloader(manager, parent)
{
}


void PhoneticsDownloader::download(QString targetLanguage, QString targetText, QVariant userData)
{
    // Start request
    QNetworkReply *reply = get("/translate_a/t", Query()
                               << QueryItem("client", "t")
                               << QueryItem("v", "1.0")
                               << QueryItem("text", targetText)
                               << QueryItem("sl", targetLanguage)
                               << QueryItem("tl", "und-Latn")
                               << QueryItem("hl", "en"));

    // Set original request properties
    reply->setProperty(xlatTargetLanguage, targetLanguage);
    reply->setProperty(xlatTargetText, targetText);
    reply->setProperty(xlatUserData, userData);
}

void PhoneticsDownloader::downloadFailure(QNetworkReply *reply, QNetworkReply::NetworkError error) const
{
    // Get original request properties
    QString targetLanguage = reply->property(xlatTargetLanguage).value<QString>();
    QString targetText = reply->property(xlatTargetText).value<QString>();
    QVariant userData = reply->property(xlatUserData);

    qDebug() << reply->readAll();

    // Emit error signal
    emit failure(targetLanguage, targetText, userData,
                 QString("Network error %0 while downloading phonetics: %1")
                 .arg(error)
                 .arg(reply->errorString()));
}

void PhoneticsDownloader::downloadSuccess(QNetworkReply *reply, QByteArray body) const
{
    // Get original request properties
    QString targetLanguage = reply->property(xlatTargetLanguage).value<QString>();
    QString targetText = reply->property(xlatTargetText).value<QString>();
    QVariant userData = reply->property(xlatUserData);

    // Parse QVariant into Translation object
    QString phonetics = body == "[\"\"]"
            ? "N/A"
            : QString::fromUtf8(body.constData() + 1, body.size() - 2);

    emit success(targetLanguage, targetText, userData, phonetics);
}
