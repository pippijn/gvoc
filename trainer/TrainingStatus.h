#ifndef TRAININGSTATUS_H
#define TRAININGSTATUS_H

#include <QString>

class TrainingStatus
{
public:
    TrainingStatus();

    QString statusMessage() const;
    QString finishMessage() const;

public:
    int correct;
    int mistakes;
    int skipped;
};

#endif // TRAININGSTATUS_H
