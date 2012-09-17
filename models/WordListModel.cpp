#include "WordListModel.h"
#include "Translation.h"

WordListModel::WordListModel(QList<Translation> translations, QObject *parent)
    : AbstractTreeModel(headers(translations), parent)
{
    TreeItem &root = getItem(QModelIndex());

    foreach (Translation const &entry, translations)
    {
        TreeItem &entryItem = root.addChild(data(entry.primary.sourceText,
                                                 entry.primary.sourcePhonetic,
                                                 entry.primary.translated,
                                                 entry.primary.translatedPhonetic));

        foreach (Dictionary::WordType wordType, entry.dictionary.sections.keys())
        {
            TreeItem &wordTypeItem = entryItem.addChild(Dictionary::wordTypeName(wordType));
            DictionarySection const &section = entry.dictionary.sections[wordType];
            foreach (QString const &alternative, section.translations.keys())
            {
                wordTypeItem.addChild(data(alternative,
                                           "",
                                           section.translations[alternative].join(", "),
                                           ""));
            }
        }
    }
}


QStringList WordListModel::headers(QList<Translation> translations)
{
    hasSourcePhonetic = true;
    hasTranslatedPhonetic = true;

    foreach (Translation const &entry, translations)
    {
        if (!entry.primary.sourcePhonetic.isEmpty())
            hasSourcePhonetic = true;
        if (!entry.primary.translatedPhonetic.isEmpty())
            hasTranslatedPhonetic = true;

        if (hasSourcePhonetic && hasTranslatedPhonetic)
            break;
    }

    QStringList headers("Word");

    if (hasSourcePhonetic)
        headers << "Phonetic";
    headers << "Translation";
    if (hasTranslatedPhonetic)
        headers << "Phonetic";

    return headers;
}


QStringList WordListModel::data(QString sourceText, QString sourcePhonetic, QString translated, QString translatedPhonetic) const
{
    QStringList data(sourceText);

    if (hasSourcePhonetic)
        data << sourcePhonetic;
    data << translated;
    if (hasTranslatedPhonetic)
        data << translatedPhonetic;

    return data;
}
