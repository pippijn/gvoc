#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>

class Downloader : public QObject
{
    Q_OBJECT

protected:
    typedef QPair<QString, QString> QueryItem;
    typedef QList<QueryItem> Query;

protected:
    explicit Downloader(QNetworkAccessManager &manager, QObject *parent = 0);

    QNetworkReply *get(QUrl url);
    QNetworkReply *get(QString path, Query const &query);

private:
    virtual void downloadFailure(QNetworkReply *reply, QNetworkReply::NetworkError error) const = 0;
    virtual void downloadSuccess(QNetworkReply *reply, QByteArray body) const = 0;

private slots:
    void downloadError(QNetworkReply::NetworkError error) const;
    void downloadFinished() const;

protected:
    QNetworkAccessManager &http;
};

#endif // DOWNLOADER_H
