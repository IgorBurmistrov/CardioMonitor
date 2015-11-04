#ifndef CARDIOPLOT_H
#define CARDIOPLOT_H

#include <QVector>

#include "qcustomplot.h"

const int DM_Units = 0;
const int DM_Volts = 1;

class CardioPlot : public QCustomPlot
{
public:
    CardioPlot(QWidget* parent);
    ~CardioPlot();
    void add(double value, int time);
    void clear();
    void setValueDisplayMode(int mode);
    void setValueRange(int from, int to);
    void setTimeRange(int range);
    void saveAsCSV(const QString & filaneme);
private:
    QCPData convertData(const QCPData & src);
private:
    int m_displayMode;
    QCPDataMap * m_data;
};

#endif // CARDIOPLOT_H
