#ifndef VOCABULARY_ZH_H
#define VOCABULARY_ZH_H

#include "Vocabulary.h"

class Vocabulary_en_zh : public Vocabulary::Derived<Vocabulary_en_zh>
{
public:
    static QString const className;

public:
    Vocabulary_en_zh();

private:
    QString processTargetText(QString targetText) const;
    QString processTargetPhonetic(QString targetPhonetic) const;
    QString processTranslation(QString translation) const;
    QString latinPhonetic(QString targetPhonetic) const;
};

#endif // VOCABULARY_ZH_H
