#ifndef VOCABULARY_H
#define VOCABULARY_H

#include <QMap>
#include <QStringList>

#include "ClassRegistry.h"
class Translation;

class Vocabulary : private ClassRegistry<Vocabulary>
{
public:
    // Words with translations
    struct Vocable;

    // Hints
    struct Hint;

private:
    typedef QMap<QString, Vocable> VocableMap;
    VocableMap vocabulary;

    typedef QList<Hint> HintList;
    HintList hints;

    typedef QMap<QString, HintList> HintMap;
    mutable HintMap hintIndex;
    mutable int hintIndexLevel;

public:
    static Vocabulary *create(QString sourceLanguage, QString targetLanguage);
    using ClassRegistry<Vocabulary>::Derived;

public:
    Vocabulary();
    virtual ~Vocabulary();

    void loadWordList(QString fileName);
    void loadWordList(QList<Translation> const &wordList);
    void loadHintList(QString fileName);

    QStringList wordList(int minLevel, int maxLevel) const;

    QStringList targetOptions(QString word) const;
    QString targetText(QString word) const;
    QString targetPhonetics(QString word) const;
    QStringList sourceOptions(QString word) const;
    QString sourceText(QString word) const;
    QString sourcePhonetics(QString word) const;

    bool matchesSourceText(QString word, QString text) const;
    bool matchesTargetText(QString word, QString text) const;

    QString hintText(Hint const &hint) const;
    QString hintPhonetic(Hint const &hint) const;
    QString hintTranslation(Hint const &hint) const;

    HintList &hintList(QString word, int maxLevel) const;
    Hint const *hint(QString word, int maxLevel) const;
    void rotateHints(QString word, int maxLevel);

private:
    void addWord(Vocable const &voc);

protected:
    virtual QString normalisePhonetic(QString phonetic) const;
    virtual QString phoneticToLatin(QString phonetic) const;
    virtual bool matchTranslation(QString actual, QString expected) const;
};

#endif // VOCABULARY_H
