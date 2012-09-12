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
        for (int i = 0; i < parentItem->childCount(); i++)
            if (parentItem->child(i) == this)
                return i;

    return 0;
}

TreeItem const *TreeItem::child(int index) const
{
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
