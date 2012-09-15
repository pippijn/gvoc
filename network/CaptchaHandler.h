#ifndef CAPTCHAPARSER_H
#define CAPTCHAPARSER_H

#include "CaptchaPageDownloader.h"
#include "CaptchaImageDownloader.h"
#include "CaptchaReplyUploader.h"

class CaptchaHandler : public QObject
{
    Q_OBJECT

public:
    CaptchaHandler(QNetworkAccessManager &manager, QObject *parent = 0);

    void handle(QUrl location);

private slots:
    void pageFailure(QUrl location, QString errorMessage);
    void pageSuccess(QUrl location, QString captchaId);

    void imageFailure(QString captchaId, QString errorMessage);
    void imageSuccess(QString captchaId, QImage image);

    void replyFailure(QString captchaId, QString captchaText, QString errorMessage);
    void replySuccess(QString captchaId, QString captchaText);

private:
    CaptchaPageDownloader pageDownloader;
    CaptchaImageDownloader imageDownloader;
    CaptchaReplyUploader replyUploader;
};

#endif // CAPTCHAPARSER_H
