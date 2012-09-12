#include "TranslationModel.h"

#include <QDebug>

TranslationModel::TranslationModel(QString sourceLanguage, QString targetLanguage, QObject *parent)
    : QAbstractItemModel(parent)
    , translator(sourceLanguage, targetLanguage, this)
    , root(QStringList() << "Word" << "Phonetic" << "Translations")
{
    connect(&translator, SIGNAL(translateSuccess(Translation)), SLOT(setTranslation(Translation)));
    connect(&translator, SIGNAL(phoneticSuccess(QString,QVariant)), SLOT(updatedPhonetic(QString,QVariant)));
    connect(&translator, SIGNAL(translateError(QString)), SIGNAL(translateError(QString)));
    connect(&translator, SIGNAL(phoneticError(QString)), SIGNAL(phoneticError(QString)));
}

TranslationModel::~TranslationModel()
{
}


QModelIndex TranslationModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    TreeItem const *parentItem = getItem(parent);

    TreeItem const *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, const_cast<TreeItem *>(childItem));
    else
        return QModelIndex();
}

QModelIndex TranslationModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    TreeItem const *childItem = getItem(child);
    TreeItem const *parentItem = childItem->parent();

    if (parentItem == &root)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, const_cast<TreeItem *>(parentItem));
}

int TranslationModel::rowCount(const QModelIndex &parent) const
{
    return getItem(parent)->childCount();
}

int TranslationModel::columnCount(const QModelIndex &parent) const
{
    return getItem(parent)->columnCount();
}

QVariant TranslationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem const *item = getItem(index);

    return item->data(index.column());
}

QVariant TranslationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return root.data(section);

    return QVariant();
}


QList<Translation> TranslationModel::wordList() const
{
    return translator.wordList();
}

void TranslationModel::setWordList(QList<Translation> wordList)
{
    translator.setWordList(wordList);
}

QMap<QString, QString> TranslationModel::phonetics() const
{
    return translator.phonetics();
}

void TranslationModel::setPhonetics(QMap<QString, QString> phonetics)
{
    translator.setPhonetics(phonetics);
}

QString TranslationModel::sourceLanguage() const
{
    return translator.sourceLanguage();
}

void TranslationModel::setSourceLanguage(QString sourceLanguage)
{
    translator.setSourceLanguage(sourceLanguage);
}

QString TranslationModel::targetLanguage() const
{
    return translator.targetLanguage();
}

void TranslationModel::setTargetLanguage(QString targetLanguage)
{
    translator.setTargetLanguage(targetLanguage);
}


TreeItem const *TranslationModel::getItem(const QModelIndex &index) const
{
    if (index.isValid())
        if (TreeItem const *item = static_cast<TreeItem const *>(index.internalPointer()))
            return item;
    return &root;
}

TreeItem *TranslationModel::getItem(const QModelIndex &index)
{
    if (index.isValid())
        if (TreeItem *item = static_cast<TreeItem *>(index.internalPointer()))
            return item;
    return &root;
}


void TranslationModel::translate(QString text)
{
    translator.translate(text);
}


Q_DECLARE_METATYPE(QModelIndex)

void TranslationModel::setTranslation(Translation translations)
{
    root.clear();

    foreach (Translation::Dictionary const &translation, translations.dictionary)
    {
        TreeItem &wordTypeItem = root.addChild(QStringList() << translation.wordTypeName());
        foreach (QString const &alternative, translation.alternatives)
        {
            wordTypeItem.addChild(
                QStringList()
                << alternative
                << translator.phoneticNoFetch(alternative)
                << translation.alternativeTranslations[alternative].join(", "));
            translator.phonetic(alternative, QVariant::fromValue(QModelIndex()));
        }
    }

    reset();

    emit updated(translations);
}


void TranslationModel::updatePhonetic(const QModelIndex &index)
{
    TreeItem *item = getItem(index);
    translator.phonetic(item->data(0), QVariant::fromValue(index));
}

void TranslationModel::updatedPhonetic(QString phonetic, QVariant index)
{
    QModelIndex translationIndex = qvariant_cast<QModelIndex>(index);
    if (!translationIndex.isValid())
        return;

    TreeItem *item = static_cast<TreeItem *>(translationIndex.internalPointer());
    item->setData(1, phonetic);

    QModelIndex phoneticIndex = translationIndex.sibling(translationIndex.row(), 1);
    dataChanged(phoneticIndex, phoneticIndex);
}
