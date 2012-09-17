#ifndef TREEITEM_H
#define TREEITEM_H

#include <QStringList>

class TreeItem
{
public:
    TreeItem(QStringList data = QStringList(), TreeItem *parent = 0);

    int columnCount() const;
    QString data(int index) const;
    void setData(int index, QString data);

    int childCount() const;
    int childNumber() const;
    int childNumber(TreeItem const *child) const;
    TreeItem const *child(int index) const;
    TreeItem const *parent() const;

    TreeItem &addChild(QString data);
    TreeItem &addChild(QStringList data);
    void removeChild(int index);
    void clear();

private:
    QList<TreeItem> childItems;
    TreeItem const *parentItem;
    QStringList itemData;
};

#endif // TREEITEM_H
