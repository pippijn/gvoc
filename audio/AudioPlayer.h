#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <QAudio>
#include <vector>

class AudioPlayerPrivate;

class AudioPlayer : public QObject
{
    Q_OBJECT

public:
    AudioPlayer(QObject *parent = 0);
    ~AudioPlayer();

    void playMP3(QByteArray const &body);

private:
    void play(std::vector<short> const &decoded, int channels, int sampleRate);

private slots:
    void stateChanged(QAudio::State state);

signals:
    void finished();

private:
    Q_DISABLE_COPY(AudioPlayer)
    Q_DECLARE_PRIVATE(AudioPlayer)
    AudioPlayerPrivate *d_ptr;
};

#endif // AUDIOPLAYER_H
