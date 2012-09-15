#include "TrainingStatus.h"

static int percent(int x, int y)
{
    return y ? x * 100 / y : 100;
}


TrainingStatus::TrainingStatus()
    : correct(0)
    , mistakes(0)
    , skipped(0)
{
}


QString TrainingStatus::statusMessage() const
{
    QString message
            = QString(" Correct: <font color='green'><strong>%0</strong></font>").arg(correct)
            + QString(" Mistakes: <font color='red'><strong>%0</strong></font>").arg(mistakes)
            + QString(" Skipped: <font color='yellow'><strong>%0</strong></font>").arg(skipped)
            ;
    return message;
}

QString TrainingStatus::finishMessage() const
{
    int total = correct + skipped;
    QString message
            = QString("Correct: <font color='green'><strong>%0</strong></font><br/>").arg(correct)
            + QString("Mistakes: <font color='red'><strong>%0</strong></font><br/>").arg(mistakes)
            + QString("Skipped: <font color='yellow'><strong>%0</strong></font><br/>").arg(skipped)
            + "<br/>"
            + QString("Knowledge: %0%").arg(percent(correct, total))
            ;
    return message;
}
