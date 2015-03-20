#include "cardioplot.h"

CardioPlot::CardioPlot(QWidget* parent) : QCustomPlot(parent)
{

    this->axisRect()->axis(QCPAxis::atBottom)->setLabel("Time [ms]");
    this->axisRect()->axis(QCPAxis::atBottom)->setRange(0, 100000);

    this->axisRect()->axis(QCPAxis::atLeft)->setLabel("Voltage [V]");
    this->axisRect()->axis(QCPAxis::atLeft)->setRange(-5, 5);

    this->addGraph(xAxis, yAxis);

    this->clear();

}

CardioPlot::~CardioPlot()
{

}

void CardioPlot::add(double value, int time)
{
    this->graph(0)->addData(time, value);
    this->replot();
}

void CardioPlot::clear() {
    QVector<double> initial_data = QVector<double>();
    initial_data.append(0.0);
    this->graph(0)->setData(initial_data, initial_data);
    this->replot();
}

