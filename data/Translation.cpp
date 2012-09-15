#include "Translation.h"

#include <QDebug>


Translation::Translation()
    : level(0)
{
}

Translation::Translation(QVariantList const &list)
    : level(0)
    , primary(list.at(0).value<QVariantList>().at(0).value<QVariantList>())
    , dictionary(list.at(1).value<QVariantList>())
{
}



QDataStream &operator << (QDataStream &data, Translation const &v)
{
    data << v.level;
    data << v.primary;
    data << v.dictionary;
    return data;
}


QDataStream &operator >> (QDataStream &data, Translation &v)
{
    data >> v.level;
    data >> v.primary;
    data >> v.dictionary;
    return data;
}
