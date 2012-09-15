#include "PrimaryTranslation.h"

PrimaryTranslation::PrimaryTranslation()
{
}

PrimaryTranslation::PrimaryTranslation(QVariantList const &list)
    : translated(list.at(0).value<QString>())
    , sourceText(list.at(1).value<QString>())
    , translatedPhonetic(list.at(2).value<QString>())
    , sourcePhonetic(list.at(3).value<QString>())
{
}


QDataStream &operator << (QDataStream &data, PrimaryTranslation const &v)
{
    data << v.translated;
    data << v.sourceText;
    data << v.translatedPhonetic;
    data << v.sourcePhonetic;
    return data;
}

QDataStream &operator >> (QDataStream &data, PrimaryTranslation &v)
{
    data >> v.translated;
    data >> v.sourceText;
    data >> v.translatedPhonetic;
    data >> v.sourcePhonetic;
    return data;
}
