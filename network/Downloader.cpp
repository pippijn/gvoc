#include "Downloader.h"

Downloader::Downloader(QNetworkAccessManager &manager, QObject *parent)
    : QObject(parent)
    , http(manager)
{
}


QNetworkReply *Downloader::get(QUrl url)
{
    // Set user agent
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent",
                         "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.1 (KHTML, like Gecko) Chrome/22.0.1207.1 Safari/537.1");

    // Start request
    QNetworkReply *reply = http.get(request);

    // Connect signals
    connect(reply, SIGNAL(finished()), SLOT(downloadFinished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(downloadError(QNetworkReply::NetworkError)));

    return reply;
}

QNetworkReply *Downloader::get(QString path, Query const &query)
{
    // Construct query URL
    QUrl url;
    url.setScheme("http");
    url.setHost("translate.google.com");
    url.setPath(path);
    url.setQueryItems(query);

    return get(url);
}

void Downloader::downloadError(QNetworkReply::NetworkError error) const
{
    // Get current network reply
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply != NULL);

    downloadFailure(reply, error);
}

void Downloader::downloadFinished() const
{
    // Get current network reply
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply != NULL);
    if (reply->error() != QNetworkReply::NoError)
        return;

    // Get reply body
    QByteArray body = reply->readAll();

    downloadSuccess(reply, body);
}
