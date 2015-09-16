#ifndef CARDIOPLOT_H
#define CARDIOPLOT_H

#include "qcustomplot.h"

class CardioPlot : public QCustomPlot
{
public:
    CardioPlot(QWidget* parent);
    ~CardioPlot();
    void add(double value, int time);
    void clear();
    void saveAsCSV(const QString & filaneme);
};

#endif // CARDIOPLOT_H
