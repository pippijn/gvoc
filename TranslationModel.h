#ifndef TRANSLATIONMODEL_H
#define TRANSLATIONMODEL_H

#include <QAbstractItemModel>

#include "TreeItem.h"
#include "Translator.h"

class TranslationModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit TranslationModel(QString sourceLanguage, QString targetLanguage, QObject *parent = 0);
    ~TranslationModel();

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void translate(QString text);
    void updatePhonetic(QModelIndex const &index);

    QList<Translation> wordList() const;
    void setWordList(QList<Translation> wordList);

    QMap<QString, QString> phonetics() const;
    void setPhonetics(QMap<QString, QString> phonetics);

    QString sourceLanguage() const;
    void setSourceLanguage(QString sourceLanguage);
    QString targetLanguage() const;
    void setTargetLanguage(QString targetLanguage);

private:
    TreeItem const *getItem(const QModelIndex &index) const;
    TreeItem *getItem(const QModelIndex &index);

signals:
    void updated(Translation translation);
    void translateError(QString message);
    void phoneticError(QString message);

public slots:
    void setTranslation(Translation translations);
    void updatedPhonetic(QString phonetic, QVariant index);

private:
    Translator translator;
    TreeItem root;
};

#endif // TRANSLATIONMODEL_H
