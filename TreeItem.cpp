#include "TreeItem.h"

TreeItem::TreeItem(QStringList data, TreeItem *parent)
    : parentItem(parent)
    , itemData(data)
{
}


int TreeItem::columnCount() const
{
    return itemData.size();
}

QString TreeItem::data(int index) const
{
    return itemData.at(index);
}

void TreeItem::setData(int index, QString data)
{
    itemData[index] = data;
}


int TreeItem::childCount() const
{
    return childItems.size();
}

int TreeItem::childNumber() const
{
    if (parentItem)
        return parentItem->childNumber(this);

    return 0;
}

int TreeItem::childNumber(TreeItem const *child) const
{
    for (int i = 0; i < childCount(); i++)
        if (this->child(i) == child)
            return i;
    qWarning("did not find child %p in %p", child, this);
    return 0;
}

TreeItem const *TreeItem::child(int index) const
{
    if (childItems.size() <= index)
    {
        qWarning("index %d is greater than size %d", index, childItems.size());
        return NULL;
    }
    return &childItems.at(index);
}

TreeItem const *TreeItem::parent() const
{
    return parentItem;
}


TreeItem &TreeItem::addChild(QStringList data)
{
    TreeItem item(data, this);
    while (item.columnCount() != columnCount())
        item.itemData.append(QString());
    childItems.append(item);
    return childItems.back();
}

void TreeItem::clear()
{
    childItems.clear();
}
