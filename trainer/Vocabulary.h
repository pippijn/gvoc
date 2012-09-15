#ifndef VOCABULARY_H
#define VOCABULARY_H

#include <QMap>
#include <QStringList>

#include "ClassRegistry.h"
class Translation;

class Vocabulary : private ClassRegistry<Vocabulary>
{
private:
    // Words with translations
    struct Vocable;

    typedef QMap<QString, Vocable> VocableMap;
    VocableMap vocabulary;

    // Hints
    struct Hint;

    typedef QMap<QString, QList<Hint> > HintMap;
    HintMap hints;

public:
    static Vocabulary *create(QString sourceLanguage, QString targetLanguage);
    using ClassRegistry<Vocabulary>::Derived;

public:
    Vocabulary();
    virtual ~Vocabulary();

    void loadWordlist(QString fileName);
    void loadWordlist(QList<Translation> const &wordList);
    void loadHintList(QString fileName);

    QStringList wordList(int maxLevel) const;

    QString targetText(QString word) const;
    QString targetPhonetics(QString word) const;
    QString sourceText(QString word) const;
    QString sourcePhonetics(QString word) const;

    bool matchesSourceText(QString word, QString text) const;
    bool matchesTargetText(QString word, QString text) const;

    QString targetHint(QString word) const;
    void rotateTargetHints(QString word);

    QString sourceHint(QString word) const;
    void rotateSourceHints(QString word);

private:
    void addWord(Vocable const &voc);

    virtual QString processTargetText(QString targetText) const;
    virtual QString processTargetPhonetic(QString targetPhonetic) const;
    virtual QString processTranslation(QString translation) const;
    virtual QString latinPhonetic(QString targetPhonetic) const;
};

#endif // VOCABULARY_H
