#ifndef TRANSLATIONDOWNLOADER_H
#define TRANSLATIONDOWNLOADER_H

#include "Downloader.h"
#include "Translation.h"

class TranslationDownloader : public Downloader
{
    Q_OBJECT

public:
    explicit TranslationDownloader(QNetworkAccessManager &manager, QObject *parent = 0);

    void download(QString sourceLanguage, QString targetLanguage, QString sourceText, QVariant userData);

private:
    void downloadFailure(QNetworkReply *reply, QNetworkReply::NetworkError error) const;
    void downloadSuccess(QNetworkReply *reply, QByteArray body) const;

signals:
    void failure(QString sourceLanguage, QString targetLanguage, QString sourceText, QVariant userData, QString errorMessage) const;
    void success(QString sourceLanguage, QString targetLanguage, QString sourceText, QVariant userData, Translation translation) const;
};

#endif // TRANSLATIONDOWNLOADER_H
