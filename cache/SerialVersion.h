#ifndef SERIALVERSION_H
#define SERIALVERSION_H

#include <QMap>

template<typename T>
struct SerialVersion;

// partial specialisation for string maps
template<typename T>
struct SerialVersion<QMap<QString, T> >
{
    static quint32 const value = SerialVersion<T>::value;
};

// full specialisations for Qt types
template<>
struct SerialVersion<QString>
{
    static quint32 const value = 0x52545351; // "QSTR"
};

template<>
struct SerialVersion<QByteArray>
{
    static quint32 const value = 0x52414251; // "QBAR"
};

#endif // SERIALVERSION_H
