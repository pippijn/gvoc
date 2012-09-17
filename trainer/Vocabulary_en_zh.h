#ifndef VOCABULARY_ZH_H
#define VOCABULARY_ZH_H

#include "Vocabulary.h"

class Vocabulary_en_zh : public Vocabulary::Derived<Vocabulary_en_zh>
{
    typedef Vocabulary::Derived<Vocabulary_en_zh> Base;

public:
    static QString const className;

public:
    Vocabulary_en_zh();

private:
    QString normalisePhonetic(QString targetPhonetic) const;
    QString phoneticToLatin(QString targetPhonetic) const;
    bool matchTranslation(QString actual, QString expected) const;
};

#endif // VOCABULARY_ZH_H
