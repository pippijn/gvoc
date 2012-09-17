#ifndef WORDLISTVIEW_H
#define WORDLISTVIEW_H

#include <QTreeView>

namespace Ui {
class WordListView;
}

class WordListView : public QTreeView
{
    Q_OBJECT
    
public:
    explicit WordListView(QWidget *parent = 0);
    ~WordListView();

    void removeSelectedRows();
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    
private:
    Ui::WordListView *ui;
};

#endif // WORDLISTVIEW_H
