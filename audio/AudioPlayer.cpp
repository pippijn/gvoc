#include "AudioPlayer.h"
#include "mp3/minimp3.h"

#include <QByteArray>
#include <QDebug>

#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include <QBuffer>
#include <QFile>


struct AudioPlayerPrivate
{
    QAudioOutput *output;
    QBuffer buffer;

    AudioPlayerPrivate()
        : output(0)
    {
    }
};


AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent)
    , d_ptr(new AudioPlayerPrivate)
{
}

AudioPlayer::~AudioPlayer()
{
    delete d_ptr;
}


void AudioPlayer::play(std::vector<short> const &decoded, int channels, int sampleRate)
{
    Q_D(AudioPlayer);

    QAudioFormat format;
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setChannelCount(channels);
    format.setCodec("audio/pcm");
    format.setSampleRate(sampleRate);
    format.setSampleSize(16);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo device = QAudioDeviceInfo::defaultOutputDevice();
    if (!device.isFormatSupported(format))
    {
        qWarning() << "Default format not supported - trying to use nearest";
        format = device.nearestFormat(format);
    }

    delete d->output;
    d->output = new QAudioOutput(device, format, this);
    connect(d->output, SIGNAL(stateChanged(QAudio::State)), SLOT(stateChanged(QAudio::State)));

    d->buffer.setData(reinterpret_cast<char const *>(decoded.data()),
                      decoded.size() * sizeof decoded[0]);

    if (!d->buffer.open(QIODevice::ReadOnly))
    {
        qFatal("could not open buffer device");
        emit finished();
        return;
    }
    d->output->start(&d->buffer);
}


void AudioPlayer::stateChanged(QAudio::State state)
{
    Q_D(AudioPlayer);
    if (state == QAudio::IdleState)
    {
        d->buffer.close();
        emit finished();
    }
}


void AudioPlayer::playMP3(QByteArray const &body)
{
    mp3_info_t info;
    short sampleBuffer[MP3_MAX_SAMPLES_PER_FRAME];

    int size = body.size();
    char const *data = body.constData();

    mp3_context_t *decoder = mp3_create();
    int frameSize = mp3_decode(decoder, data, size, sampleBuffer, &info);
    if (frameSize == 0)
    {
        qWarning("invalid MP3 file");
        qDebug() << body.left(512).constData();
        emit finished();
        return;
    }

    std::vector<short> decoded;
    while (size >= 0 && frameSize > 0)
    {
        decoded.insert(decoded.end(),
                       sampleBuffer,
                       sampleBuffer + info.audio_bytes / sizeof *sampleBuffer);
        data += frameSize;
        size -= frameSize;
        frameSize = mp3_decode(decoder, data, size, sampleBuffer, NULL);
    }

    mp3_free(decoder);

    play(decoded, info.channels, info.sample_rate);
}
