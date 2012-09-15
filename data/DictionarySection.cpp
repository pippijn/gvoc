#include "DictionarySection.h"

#include <QDebug>

DictionarySection::DictionarySection()
{
}

DictionarySection::DictionarySection(QVariantList const &list)
{
    foreach (QVariant const &variant, list)
    {
        QVariantList const &element = variant.value<QVariantList>();

        QString word = element.at(0).value<QString>();
        QStringList options = element.at(1).toStringList();

        if (translations.contains(word))
        {
            qWarning() << "duplicate translation:" << word;
            QStringList &existing = translations[word];
            existing.append(options);
            existing.removeDuplicates();
            continue;
        }

        translations.insert(word, options);
    }
}


QDataStream &operator << (QDataStream &data, DictionarySection const &v)
{
    data << v.translations;
    return data;
}

QDataStream &operator >> (QDataStream &data, DictionarySection &v)
{
    data >> v.translations;
    return data;
}
