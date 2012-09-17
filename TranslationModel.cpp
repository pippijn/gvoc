#include "TranslationModel.h"
#include "TranslationManager.h"
#include "PhoneticsManager.h"

#include <QDebug>
#include <QTimer>

TranslationModel::TranslationModel(TranslationManager &translationManager, PhoneticsManager &phoneticsManager, QString sourceLanguage, QString targetLanguage, QObject *parent)
    : AbstractTreeModel(QStringList() << "Word" << "Phonetic" << "Translations", parent)
    , translationManager(translationManager)
    , phoneticsManager(phoneticsManager)

    , sourceLanguageCode(sourceLanguage)
    , targetLanguageCode(targetLanguage)
{
    connect(&translationManager, SIGNAL(success(QString,QString,QString,QVariant,Translation)), SLOT(translateSuccess(QString,QString,QString,QVariant,Translation)));
    connect(&translationManager, SIGNAL(failure(QString,QString,QString,QVariant,QString)), SLOT(translateFailure(QString,QString,QString,QVariant,QString)));

    connect(&phoneticsManager, SIGNAL(success(QString,QString,QVariant,QString)), SLOT(phoneticSuccess(QString,QString,QVariant,QString)));
    connect(&phoneticsManager, SIGNAL(failure(QString,QString,QVariant,QString)), SLOT(phoneticFailure(QString,QString,QVariant,QString)));
}

TranslationModel::~TranslationModel()
{
}


QVariant TranslationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem const *item = getItem(index);

    if (index.column() == 0 && item->parent() && item->parent()->parent())
    {
        if (phoneticsQueue.isEmpty())
            QTimer::singleShot(0, const_cast<TranslationModel *>(this), SLOT(updatePhonetics()));
        phoneticsQueue.append(index);
    }

    return item->data(index.column());
}


QList<Translation> TranslationModel::wordList() const
{
    return translationManager.translations(sourceLanguage(), targetLanguage());
}

void TranslationModel::setWordList(QList<Translation> wordList)
{
    translationManager.setTranslations(sourceLanguage(), targetLanguage(), wordList);
}

QMap<QString, QString> TranslationModel::phonetics() const
{
    return phoneticsManager.phonetics(targetLanguage());
}

void TranslationModel::setPhonetics(QMap<QString, QString> phonetics)
{
    phoneticsManager.setPhonetics(targetLanguage(), phonetics);
}

QString TranslationModel::sourceLanguage() const
{
    return sourceLanguageCode;
}

void TranslationModel::setSourceLanguage(QString sourceLanguage)
{
    this->sourceLanguageCode = sourceLanguage;
}

QString TranslationModel::targetLanguage() const
{
    return targetLanguageCode;
}

void TranslationModel::setTargetLanguage(QString targetLanguage)
{
    this->targetLanguageCode = targetLanguage;
}


void TranslationModel::translate(QString text)
{
    translationManager.translation(sourceLanguage(), targetLanguage(), text);
}


void TranslationModel::translateFailure(QString sourceLanguage, QString targetLanguage, QString sourceText, QVariant userData, QString errorMessage)
{
    emit translateFailure(errorMessage);
}

void TranslationModel::translateSuccess(QString sourceLanguage, QString targetLanguage, QString sourceText, QVariant userData, Translation entry)
{
    TreeItem &root = *getItem(QModelIndex());
    root.clear();

    foreach (Dictionary::WordType wordType, entry.dictionary.sections.keys())
    {
        TreeItem &wordTypeItem = root.addChild(QStringList() << Dictionary::wordTypeName(wordType));
        DictionarySection const &section = entry.dictionary.sections[wordType];
        foreach (QString const &alternative, section.translations.keys())
        {
            wordTypeItem.addChild(
                QStringList()
                << alternative
                << ""
                << section.translations[alternative].join(", "));
        }
    }

    reset();

    emit translateSuccess(entry);
}


Q_DECLARE_METATYPE(QModelIndex)

void TranslationModel::updatePhonetics()
{
    std::reverse(phoneticsQueue.begin(), phoneticsQueue.end());
    foreach (QModelIndex index, phoneticsQueue)
    {
        TreeItem *item = getItem(index);
        phoneticsManager.phonetic(targetLanguage(), item->data(0), QVariant::fromValue(index));
    }
    phoneticsQueue.clear();
}

void TranslationModel::phoneticFailure(QString targetLanguage, QString targetText, QVariant userData, QString errorMessage)
{
}

void TranslationModel::phoneticSuccess(QString targetLanguage, QString targetText, QVariant userData, QString phonetic)
{
    QModelIndex translationIndex = userData.value<QModelIndex>();
    if (!translationIndex.isValid())
        return;

    TreeItem *item = static_cast<TreeItem *>(translationIndex.internalPointer());
    item->setData(1, phonetic);

    QModelIndex phoneticIndex = translationIndex.sibling(translationIndex.row(), 1);
    dataChanged(phoneticIndex, phoneticIndex);
}
