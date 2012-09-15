#include "TranslationDownloader.h"
#include "DictionaryPersister.h"

static char const *const xlatSourceText = "xlatSourceText";
static char const *const xlatTargetText = "xlatTargetText";
static char const *const xlatSourceLanguage = "xlatSourceLanguage";
static char const *const xlatTargetLanguage = "xlatTargetLanguage";
static char const *const xlatUserData = "xlatUserData";

TranslationDownloader::TranslationDownloader(QNetworkAccessManager &manager, QObject *parent)
    : Downloader(manager, parent)
{
}


void TranslationDownloader::download(QString sourceLanguage, QString targetLanguage, QString sourceText, QVariant userData)
{
    // Start request
    QNetworkReply *reply = get("/translate_a/t", Query()
                               << QueryItem("client", "t")
                               << QueryItem("text", sourceText)
                               << QueryItem("sl", sourceLanguage)
                               << QueryItem("tl", targetLanguage)
                               << QueryItem("hl", "en"));

    // Set original request properties
    reply->setProperty(xlatSourceLanguage, sourceLanguage);
    reply->setProperty(xlatTargetLanguage, targetLanguage);
    reply->setProperty(xlatSourceText, sourceText);
    reply->setProperty(xlatUserData, userData);
}

void TranslationDownloader::downloadFailure(QNetworkReply *reply, QNetworkReply::NetworkError error) const
{
    // Get original request properties
    QString sourceLanguage = reply->property(xlatSourceLanguage).value<QString>();
    QString targetLanguage = reply->property(xlatTargetLanguage).value<QString>();
    QString sourceText = reply->property(xlatSourceText).value<QString>();
    QVariant userData = reply->property(xlatUserData);

    // Emit error signal
    emit failure(sourceLanguage, targetLanguage, sourceText, userData,
                 QString("Network error %0 while downloading translation: %1")
                 .arg(error)
                 .arg(reply->errorString()));
}

void TranslationDownloader::downloadSuccess(QNetworkReply *reply, QByteArray body) const
{
    // Get original request properties
    QString sourceLanguage = reply->property(xlatSourceLanguage).value<QString>();
    QString targetLanguage = reply->property(xlatTargetLanguage).value<QString>();
    QString sourceText = reply->property(xlatSourceText).value<QString>();
    QVariant userData = reply->property(xlatUserData);

    // Parse JSON body into Translation object
    DictionaryPersister persister;
    bool ok = true;
    Translation translation = persister.fromJSON(body, ok);
    if (!ok)
    {
        emit failure(sourceLanguage, targetLanguage, sourceText, userData,
                     QString("JSON error: %0")
                     .arg(persister.errorString()));
        return;
    }

    emit success(sourceLanguage, targetLanguage, sourceText, userData, translation);
}
