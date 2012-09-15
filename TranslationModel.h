#ifndef TRANSLATIONMODEL_H
#define TRANSLATIONMODEL_H

#include <QAbstractItemModel>

#include "TreeItem.h"
#include "Translator.h"
class TranslationManager;
class PhoneticsManager;

class TranslationModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit TranslationModel(TranslationManager &translationManager, PhoneticsManager &phoneticsManager, QString sourceLanguage, QString targetLanguage, QObject *parent = 0);
    ~TranslationModel();

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void translate(QString text);

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
    void translateSuccess(Translation translation);
    void translateFailure(QString errorMessage);
    void phoneticError(QString message);

private slots:
    void updatePhonetics();

    void translateFailure(QString sourceLanguage, QString targetLanguage, QString sourceText, QVariant userData, QString errorMessage);
    void translateSuccess(QString sourceLanguage, QString targetLanguage, QString sourceText, QVariant userData, Translation entry);

    void phoneticFailure(QString targetLanguage, QString targetText, QVariant userData, QString errorMessage);
    void phoneticSuccess(QString targetLanguage, QString targetText, QVariant userData, QString phonetic);

private:
    TranslationManager &translationManager;
    PhoneticsManager &phoneticsManager;

    QString sourceLanguageCode;
    QString targetLanguageCode;

    TreeItem root;
    mutable QList<QModelIndex> phoneticsQueue;
};

#endif // TRANSLATIONMODEL_H
