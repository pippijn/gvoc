#ifndef WORDLISTMODEL_H
#define WORDLISTMODEL_H

#include "AbstractTreeModel.h"
class Translation;

class WordListModel : public AbstractTreeModel
{
    Q_OBJECT

private:
    QStringList headers(QList<Translation> translations);
    QStringList data(QString sourceText, QString sourcePhonetic, QString translated, QString translatedPhonetic) const;

public:
    explicit WordListModel(QList<Translation> translations, QObject *parent = 0);
    
signals:
    
public slots:
    
private:
    bool hasSourcePhonetic;
    bool hasTranslatedPhonetic;
};

#endif // WORDLISTMODEL_H
