#include "AbstractTreeModel.h"

AbstractTreeModel::AbstractTreeModel(QStringList headers, QObject *parent)
    : QAbstractItemModel(parent)
    , root(headers)
{
}


QModelIndex AbstractTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    TreeItem const &parentItem = getItem(parent);

    TreeItem const *childItem = parentItem.child(row);
    if (childItem)
        return createIndex(row, column, const_cast<TreeItem *>(childItem));
    else
        return QModelIndex();
}

QModelIndex AbstractTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    TreeItem const &childItem = getItem(child);
    TreeItem const *parentItem = childItem.parent();

    if (parentItem == &root)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, const_cast<TreeItem *>(parentItem));
}

int AbstractTreeModel::rowCount(const QModelIndex &parent) const
{
    return getItem(parent).childCount();
}

int AbstractTreeModel::columnCount(const QModelIndex &parent) const
{
    return getItem(parent).columnCount();
}

QVariant AbstractTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem const &item = getItem(index);

    return item.data(index.column());
}

QVariant AbstractTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return root.data(section);

    return QVariant();
}


TreeItem &AbstractTreeModel::getItem(QModelIndex const &index)
{
    if (index.isValid())
        if (TreeItem *item = static_cast<TreeItem *>(index.internalPointer()))
            return *item;
    return root;
}

TreeItem const &AbstractTreeModel::getItem(QModelIndex const &index) const
{
    if (index.isValid())
        if (TreeItem const *item = static_cast<TreeItem const *>(index.internalPointer()))
            return *item;
    return root;
}
