#include "WordListModel.h"

WordListModel::WordListModel(QObject *parent)
    : AbstractTreeModel(QStringList() << "Word" << "Phonetic" << "Translation" << "Dictionary", parent)
{
}


