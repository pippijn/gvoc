#include "Translator.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QFile>
#include <QDir>
#include "parser.h"
#include "serializer.h"

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

    QString translationsFileName() const
    {
        return QDir::current().absolutePath()
                + QString("/translations/%0.%1.dat").arg(sourceLanguage, targetLanguage);
    }

    QString phoneticsFileName() const
    {
        return QDir::current().absolutePath()
                + QString("/phonetics/%1.dat").arg(targetLanguage);
    }


    QMap<QString, Translation> translations() const
    {
        QMap<QString, Translation> translations;
        QFile file(translationsFileName());
        if (file.open(QFile::ReadOnly))
        {
            QByteArray uncompressed = qUncompress(file.readAll());
            QDataStream data(&uncompressed, QIODevice::ReadOnly);
            data >> translations;
        }

        return translations;
    }

    void setTranslations(QMap<QString, Translation> translations)
    {
        QFile file(translationsFileName());
        if (!file.open(QFile::WriteOnly))
            qWarning() << "unable to open file for writing:" << file.fileName();
        else
        {
            QByteArray uncompressed;
            QDataStream data(&uncompressed, QIODevice::WriteOnly);
            data << translations;
            file.write(qCompress(uncompressed));
        }
    }

    void addTranslation(Translation const &translation)
    {
        QMap<QString, Translation> translations = this->translations();
        translations.insert(translation.primary.at(0).sourceText, translation);
        setTranslations(translations);
    }

    QList<Translation> wordList() const
    {
        QMap<QString, Translation> translations = this->translations();
        return translations.values();
    }

    void setWordList(QList<Translation> wordList)
    {
        QMap<QString, Translation> translations;
        foreach (Translation const &word, wordList)
            translations.insert(word.primary.at(0).sourceText, word);
        setTranslations(translations);
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


QList<Translation> Translator::wordList() const
{
    Q_D(const Translator);
    return d->wordList();
}

void Translator::setWordList(QList<Translation> wordList)
{
    Q_D(Translator);
    d->setWordList(wordList);
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


void Translator::translate(QString text)
{
    Q_D(Translator);

    QMap<QString, Translation> translations = d->translations();
    QMap<QString, Translation>::const_iterator found = translations.find(text);
    if (found != translations.end())
    {
        emit translateSuccess(found.value());
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
            << QueryItem("text", QUrl::toPercentEncoding(text))
            << QueryItem("sl", QUrl::toPercentEncoding(d->sourceLanguage))
            << QueryItem("tl", QUrl::toPercentEncoding(d->targetLanguage))
            << QueryItem("hl", "en")
    );

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.1 (KHTML, like Gecko) Chrome/22.0.1207.1 Safari/537.1");

    QNetworkReply *reply = d->http.get(request);
    reply->setProperty(xlatSourceText, text);
    reply->setProperty(xlatSourceLanguage, d->sourceLanguage);
    reply->setProperty(xlatTargetLanguage, d->targetLanguage);
    connect(reply, SIGNAL(finished()), SLOT(translateSuccess()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(translateError(QNetworkReply::NetworkError)));
}

void Translator::translateError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply != NULL);

    QString sourceText = reply->property(xlatSourceText).value<QString>();

    qWarning() << "error" << error << "while fetching translations for" << sourceText;
    emit phoneticError("error while fetching translations for " + sourceText);
}

void Translator::translateSuccess()
{
    Q_D(Translator);

    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply != NULL);

    QString sourceLanguage = reply->property(xlatSourceLanguage).value<QString>();
    QString targetLanguage = reply->property(xlatTargetLanguage).value<QString>();

    QByteArray body = QString::fromUtf8(reply->readAll())
            .replace(",,", ",null,")
            .replace(",,", ",null,")
            .toUtf8();

    QJson::Parser parser;
    bool ok;
    QVariant parsed = parser.parse(body, &ok);
    if (!ok)
    {
        qWarning("json error: %s", parser.errorString().toUtf8().constData());
        return;
    }

#if 0
    QJson::Serializer serializer;
    serializer.setIndentMode(QJson::IndentCompact);
    qDebug("%s", serializer.serialize(parsed).constData());
#endif

    if (targetLanguage != d->targetLanguage || sourceLanguage != d->sourceLanguage)
    {
        emit translateError("language switched while fetching translations");
        return;
    }

    Translation translation(qvariant_cast<QVariantList>(parsed));
    d->addTranslation(translation);

    emit translateSuccess(translation);
}


QString Translator::phoneticNoFetch(QString text) const
{
    Q_D(const Translator);
    return d->phonetics()[text];
}

void Translator::phonetic(QString text, QVariant userData)
{
    Q_D(Translator);

    QString phonetic = phoneticNoFetch(text);
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
