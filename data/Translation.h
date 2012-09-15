#ifndef TRANSLATION_OLD_H
#define TRANSLATION_OLD_H

#include <QVariant>
#include <QStringList>

#include "PrimaryTranslation.h"
#include "Dictionary.h"
#include "SerialVersion.h"

struct Translation
{
    int level;
    PrimaryTranslation primary;
    Dictionary dictionary;

public:
    Translation();
    Translation(QVariantList const &list);
};

template<>
struct SerialVersion<Translation>
{
    static quint32 const value = 0x00000003;
};

QDataStream &operator << (QDataStream &data, Translation const &v);
QDataStream &operator >> (QDataStream &data, Translation &v);

#endif // TRANSLATION_OLD_H
