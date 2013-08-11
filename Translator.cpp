#include "Translator.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QFile>
#include <QDir>
#include <qjson/parser.h>
#include <qjson/serializer.h>

#include <QDebug>

static char const *const xlatSourceText = "xlatSourceText";
static char const *const xlatSourceLanguage = "xlatSourceLanguage";
static char const *const xlatTargetLanguage = "xlatTargetLanguage";
static char const *const xlatUserData = "xlatUserData";

struct TranslatorPrivate
{
    QNetworkAccessManager http;
    QString sourceLanguage;
    QString targetLanguage;

    QString phoneticsFileName() const
    {
        return QDir::current().absolutePath()
                + QString("/phonetics/%1.dat").arg(targetLanguage);
    }



    QMap<QString, QString> phonetics() const
    {
        QMap<QString, QString> phonetics;
        QFile file(phoneticsFileName());
        if (file.open(QFile::ReadOnly))
        {
            QByteArray uncompressed = qUncompress(file.readAll());
            QDataStream data(&uncompressed, QIODevice::ReadOnly);
            data >> phonetics;
        }

        return phonetics;
    }

    void setPhonetics(QMap<QString, QString> phonetics)
    {
        QFile file(phoneticsFileName());
        if (!file.open(QFile::WriteOnly))
            qWarning() << "unable to open file for writing:" << file.fileName();
        else
        {
            QByteArray uncompressed;
            QDataStream data(&uncompressed, QIODevice::WriteOnly);
            data << phonetics;
            file.write(qCompress(uncompressed));
        }
    }

    void addPhonetics(QString text, QString phonetic)
    {
        QMap<QString, QString> phonetics = this->phonetics();
        phonetics.insert(text, phonetic);
        setPhonetics(phonetics);
    }


    TranslatorPrivate(QString sourceLanguage, QString targetLanguage)
        : sourceLanguage(sourceLanguage)
        , targetLanguage(targetLanguage)
    {
        QDir::current().mkdir("translations");
        QDir::current().mkdir("phonetics");
    }
};

Translator::Translator(QString sourceLanguage, QString targetLanguage, QObject *parent)
    : QObject(parent)
    , d_ptr(new TranslatorPrivate(sourceLanguage, targetLanguage))
{
}

Translator::~Translator()
{
    delete d_ptr;
}


QMap<QString, QString> Translator::phonetics() const
{
    Q_D(const Translator);
    return d->phonetics();
}

void Translator::setPhonetics(QMap<QString, QString> phonetics)
{
    Q_D(Translator);
    d->setPhonetics(phonetics);
}


QString Translator::sourceLanguage() const
{
    Q_D(const Translator);
    return d->sourceLanguage;
}

void Translator::setSourceLanguage(QString sourceLanguage)
{
    Q_D(Translator);
    d->sourceLanguage = sourceLanguage;
}

QString Translator::targetLanguage() const
{
    Q_D(const Translator);
    return d->targetLanguage;
}

void Translator::setTargetLanguage(QString targetLanguage)
{
    Q_D(Translator);
    d->targetLanguage = targetLanguage;
}



void Translator::phonetic(QString text, QVariant userData)
{
    Q_D(Translator);

    QString phonetic = d->phonetics()[text];
    if (!phonetic.isEmpty())
    {
        emit phoneticSuccess(phonetic, userData);
        return;
    }

    typedef QPair<QByteArray, QByteArray> QueryItem;

    QUrl url;
    url.setScheme("http");
    url.setHost("translate.google.com");
    url.setPath("/translate_a/t");
    url.setEncodedQueryItems(
        QList<QueryItem>()
            << QueryItem("client", "t")
            << QueryItem("v", "1.0")
            << QueryItem("text", QUrl::toPercentEncoding(text))
            << QueryItem("sl", QUrl::toPercentEncoding(d->targetLanguage))
            << QueryItem("tl", "und-Latn")
            << QueryItem("hl", "en")
    );

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.1 (KHTML, like Gecko) Chrome/22.0.1207.1 Safari/537.1");

    QNetworkReply *reply = d->http.get(request);
    reply->setProperty(xlatSourceText, text);
    reply->setProperty(xlatTargetLanguage, d->targetLanguage);
    reply->setProperty(xlatUserData, userData);
    connect(reply, SIGNAL(finished()), SLOT(phoneticSuccess()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(phoneticError(QNetworkReply::NetworkError)));
}

void Translator::phoneticError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply != NULL);

    QString sourceText = reply->property(xlatSourceText).value<QString>();

    qWarning() << "error" << error << "while fetching phonetics for" << sourceText;
    emit phoneticError("error while fetching phonetics for " + sourceText);
}

void Translator::phoneticSuccess()
{
    Q_D(Translator);

    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply != NULL);

    QString sourceText = reply->property(xlatSourceText).value<QString>();
    QString targetLanguage = reply->property(xlatTargetLanguage).value<QString>();

    QByteArray body = reply->readAll();
    QString phonetic = body == "[\"\"]"
            ? "N/A"
            : QString::fromUtf8(body.constData() + 1, body.size() - 2);

    if (targetLanguage != d->targetLanguage)
    {
        emit phoneticError("language switched while fetching phonetics");
        return;
    }

    d->addPhonetics(sourceText, phonetic);

    emit phoneticSuccess(phonetic, reply->property(xlatUserData));
}
