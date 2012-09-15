#include "CaptchaHandler.h"
#include "CaptchaDialog.h"

#include <QDebug>
#include <QMessageBox>

CaptchaHandler::CaptchaHandler(QNetworkAccessManager &manager, QObject *parent)
    : QObject(parent)
    , pageDownloader(manager, this)
    , imageDownloader(manager, this)
    , replyUploader(manager, this)
{
    connect(&pageDownloader, SIGNAL(failure(QUrl,QString)), SLOT(pageFailure(QUrl,QString)));
    connect(&pageDownloader, SIGNAL(success(QUrl,QString)), SLOT(pageSuccess(QUrl,QString)));

    connect(&imageDownloader, SIGNAL(failure(QString,QString)), SLOT(imageFailure(QString,QString)));
    connect(&imageDownloader, SIGNAL(success(QString,QImage)), SLOT(imageSuccess(QString,QImage)));

    connect(&replyUploader, SIGNAL(failure(QString,QString,QString)), SLOT(replyFailure(QString,QString,QString)));
    connect(&replyUploader, SIGNAL(success(QString,QString)), SLOT(replySuccess(QString,QString)));
}


void CaptchaHandler::handle(QUrl location)
{
    qDebug(__func__);
    pageDownloader.download(location);
}

void CaptchaHandler::pageFailure(QUrl location, QString errorMessage)
{
    qDebug(__func__);
}

void CaptchaHandler::pageSuccess(QUrl location, QString captchaId)
{
    qDebug(__func__);
    imageDownloader.download(captchaId);
}

void CaptchaHandler::imageFailure(QString captchaId, QString errorMessage)
{
    qDebug() << __func__ << errorMessage;
}

void CaptchaHandler::imageSuccess(QString captchaId, QImage image)
{
    CaptchaDialog dialog(image);
    if (dialog.exec() == QDialog::Accepted)
        replyUploader.submit(captchaId, dialog.text());
}

void CaptchaHandler::replyFailure(QString captchaId, QString captchaText, QString errorMessage)
{
}

void CaptchaHandler::replySuccess(QString captchaId, QString captchaText)
{
}
