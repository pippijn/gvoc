#include "PersistentCache.h"

PersistentCache::PersistentCache(QString name, QDir baseDir)
    : name(name)
    , dir(baseDir)
{
    if (!dir.exists(name) && !dir.mkdir(name))
        qWarning("could not create %s cache directory", name.toUtf8().constData());
    if (!dir.cd(name))
        qWarning("could not enter %s cache directory", name.toUtf8().constData());
}


void PersistentCache::fileRead(Persister &persister, QStringList keys) const
{
    QFile file(cacheFilePath(keys));
    if (file.open(QFile::ReadOnly))
    {
        QByteArray uncompressed = qUncompress(file.readAll());
        QDataStream stream(&uncompressed, QIODevice::ReadOnly);

        quint32 serialVersion;
        stream >> serialVersion;
        if (serialVersion != persister.serialVersion())
        {
            qWarning("file format version 0x%x does not match current version 0x%x; not loading", serialVersion, persister.serialVersion());
            return;
        }

        persister.read(stream);
    }
}

void PersistentCache::fileWrite(Persister const &persister, QStringList keys) const
{
    QFile file(cacheFilePath(keys));
    if (file.open(QFile::WriteOnly))
    {
        QByteArray uncompressed;
        QDataStream stream(&uncompressed, QIODevice::WriteOnly);
        stream << persister.serialVersion();

        persister.write(stream);
        file.write(qCompress(uncompressed));
    }
}


QString PersistentCache::cacheFilePath(QStringList keys) const
{
    return dir.filePath(keys.join(".") + ".dat");
}
