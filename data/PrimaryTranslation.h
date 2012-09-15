#ifndef PRIMARYTRANSLATION_H
#define PRIMARYTRANSLATION_H

#include <QVariant>

struct PrimaryTranslation
{
    QString translated;
    QString sourceText;
    QString translatedPhonetic;
    QString sourcePhonetic;

public:
    PrimaryTranslation();
    PrimaryTranslation(QVariantList const &list);
};

QDataStream &operator << (QDataStream &data, PrimaryTranslation const &v);
QDataStream &operator >> (QDataStream &data, PrimaryTranslation &v);

#endif // PRIMARYTRANSLATION_H
