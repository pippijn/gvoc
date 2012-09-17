#include "WordListView.h"
#include "ui_WordListView.h"

#include <QDebug>

WordListView::WordListView(QWidget *parent)
    : QTreeView(parent)
    , ui(new Ui::WordListView)
{
    ui->setupUi(this);
}

WordListView::~WordListView()
{
    delete ui;
}


void WordListView::removeSelectedRows()
{
    QModelIndexList const &selection = selectionModel()->selectedRows();
    QModelIndex parent = selection.at(0).parent();

    QList<int> rows;
    foreach (QModelIndex const &index, selection)
        rows.append(index.row());
    qSort(rows);
    std::reverse(rows.begin(), rows.end());

    foreach (int row, rows)
        model()->removeRow(row, parent);
}


void WordListView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList indexes = selectedIndexes();
    QMap<QModelIndex, int> parents;
    foreach (QModelIndex index, indexes)
        parents[index.parent()]++;

    if (parents.size() > 1)
    {
        int largestSelectionCount = 0;
        QModelIndex largestSelectionParent;
        foreach (QModelIndex const &selectionParent, parents.keys())
        {
            int selectionCount = parents[selectionParent];
            if (largestSelectionCount < selectionCount)
            {
                largestSelectionCount = selectionCount;
                largestSelectionParent = selectionParent;
            }
        }

        QItemSelectionModel *selection = selectionModel();
        selection->clearSelection();
        foreach (QModelIndex const &index, indexes)
            if (index.parent() == largestSelectionParent)
                selection->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
    else
    {
        QTreeView::selectionChanged(selected, deselected);
    }
}
