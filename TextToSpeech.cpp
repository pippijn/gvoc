#include "TextToSpeech.h"

#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QDir>

typedef QMap<QString, QByteArray> CacheMap;

struct TextToSpeechPrivate
{
    QNetworkAccessManager http;
    AudioPlayer player;

    QString cachePath(QString language) const
    {
        return QDir::current().absolutePath() + "/tts/" + language + ".dat";
    }

    CacheMap cache(QString language) const
    {
        CacheMap cache;

        QFile cacheFile(cachePath(language));
        if (cacheFile.open(QFile::ReadOnly))
        {
            QDataStream stream(&cacheFile);
            stream >> cache;
        }

        return cache;
    }

    QByteArray cached(QString language, QString text) const
    {
        return cache(language)[text];
    }

    void setCache(QString language, CacheMap cache)
    {
        QFile cacheFile(cachePath(language));
        if (cacheFile.open(QFile::WriteOnly))
        {
            QDataStream stream(&cacheFile);
            stream << cache;
        }
    }

    void cache(QString language, QString text, QByteArray data)
    {
        CacheMap cache = this->cache(language);
        cache.insert(text, data);
        setCache(language, cache);
    }


    TextToSpeechPrivate()
    {
        QDir::current().mkdir("tts");
    }
};


TextToSpeech::TextToSpeech(QObject *parent)
    : QObject(parent)
    , d_ptr(new TextToSpeechPrivate)
{
    Q_D(TextToSpeech);
    connect(&d->player, SIGNAL(finished()), SIGNAL(synthesisFinished()));
}

TextToSpeech::~TextToSpeech()
{
    Q_D(TextToSpeech);
    delete d;
}


QMap<QString, QByteArray> TextToSpeech::soundFiles(QString language) const
{
    Q_D(const TextToSpeech);
    QMap<QString, QByteArray> cache = d->cache(language);
    return cache;
}

void TextToSpeech::setSoundFiles(QString language, QMap<QString, QByteArray> soundFiles)
{
    Q_D(TextToSpeech);
    d->setCache(language, soundFiles);
}


void TextToSpeech::prepare(QString language, QString text)
{
    synthesise(language, text, true);
}

void TextToSpeech::synthesise(QString language, QString text)
{
    synthesise(language, text, false);
}

void TextToSpeech::synthesise(QString language, QString text, bool downloadOnly)
{
    Q_D(TextToSpeech);

    QByteArray cached = d->cached(language, text);
    if (!cached.isEmpty())
    {
        if (!downloadOnly)
            play(cached);
        return;
    }

    QNetworkRequest request(QUrl(QString("http://translate.google.cn/translate_tts?ie=UTF-8&q=%0&tl=%1&total=1&idx=0&textlen=2")
                                 .arg(text)
                                 .arg(language)));

    QNetworkReply *reply = d->http.get(request);
    reply->setProperty("ttsLanguage", language);
    reply->setProperty("ttsText", text);
    reply->setProperty("ttsDownloadOnly", downloadOnly);
    connect(reply, SIGNAL(finished()), SLOT(soundFileDownloaded()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(soundFileError(QNetworkReply::NetworkError)));
}

void TextToSpeech::soundFileError(QNetworkReply::NetworkError error)
{
    qWarning() << "sound file download error" << error;
    emit synthesisError("sound file download error");
}

void TextToSpeech::soundFileDownloaded()
{
    Q_D(TextToSpeech);

    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply != NULL);

    QByteArray data = reply->readAll();

    QString language = reply->property("ttsLanguage").value<QString>();
    QString text = reply->property("ttsText").value<QString>();
    bool downloadOnly = reply->property("ttsDownloadOnly").value<bool>();

    d->cache(language, text, data);

    if (!downloadOnly)
        play(data);
}

void TextToSpeech::play(QByteArray data)
{
    Q_D(TextToSpeech);
    d->player.playMP3(data);
}
