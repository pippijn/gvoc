#ifndef PERSISTENTCACHE_H
#define PERSISTENTCACHE_H

#include <QDir>

#include "SerialVersion.h"

class PersistentCache
{
    Q_DISABLE_COPY(PersistentCache)

private:
    struct Persister
    {
        virtual void read(QDataStream &stream) = 0;
        virtual void write(QDataStream &stream) const = 0;
        virtual quint32 serialVersion() const = 0;
    };

    template<typename T>
    struct TypedPersister : Persister
    {
        void read(QDataStream &stream)
        {
            stream >> value;
        }

        void write(QDataStream &stream) const
        {
            stream << value;
        }

        quint32 serialVersion() const
        {
            return SerialVersion<T>::value;
        }

        TypedPersister(T const &value = T())
            : value(value)
        {
        }

        T value;
    };

protected:
    PersistentCache(QString name, QDir baseDir);

    template<typename T>
    T read(QStringList keys) const
    {
        TypedPersister<T> persister;
        fileRead(persister, keys);
        return persister.value;
    }

    template<typename T>
    void write(T const &value, QStringList keys) const
    {
        TypedPersister<T> persister(value);
        fileWrite(persister, keys);
    }

private:
    void fileRead(Persister &persister, QStringList keys) const;
    void fileWrite(Persister const &persister, QStringList keys) const;
    QString cacheFilePath(QStringList keys) const;

private:
    QString name;
    QDir dir;
};

#endif // PERSISTENTCACHE_H
