#include "PhoneticsDownloader.h"
#include "DictionaryPersister.h"
#include "Translation.h"

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
                               << QueryItem("text", targetText)
                               << QueryItem("sl", targetLanguage)
                               << QueryItem("tl", "en"));

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

    // Parse JSON body into Translation object
    DictionaryPersister persister;
    bool ok = true;
    Translation translation = persister.fromJSON(body, ok);
    if (!ok)
    {
        emit failure(targetLanguage, targetText, userData,
                     QString("JSON error: %0")
                     .arg(persister.errorString()));
        return;
    }

    emit success(targetLanguage, targetText, userData, translation.primary.sourcePhonetic);
}
