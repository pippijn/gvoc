#ifndef ABSTRACTTREEMODEL_H
#define ABSTRACTTREEMODEL_H

#include <QAbstractItemModel>

#include "TreeItem.h"

class AbstractTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit AbstractTreeModel(QStringList headers, QObject *parent = 0);
    
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool removeRows(int row, int count, QModelIndex const &parent);

protected:
    TreeItem &getItem(QModelIndex const &index);
    TreeItem const &getItem(QModelIndex const &index) const;

signals:
    
public slots:

private:
    TreeItem root;
};

#endif // ABSTRACTTREEMODEL_H
