#include "cardioplot.h"

CardioPlot::CardioPlot(QWidget* parent) : QCustomPlot(parent)
{

    this->setInteraction(QCP::iRangeDrag, true);
    this->axisRect()->setRangeDrag(Qt::Horizontal);
    this->axisRect()->setRangeDragAxes(this->axisRect()->axis(QCPAxis::atBottom), this->axisRect()->axis(QCPAxis::atLeft));

    this->axisRect()->axis(QCPAxis::atBottom)->setLabel("Time [ms]");
    this->axisRect()->axis(QCPAxis::atBottom)->setRange(0, 5000);

    this->axisRect()->axis(QCPAxis::atLeft)->setLabel("Voltage [V]");
    this->axisRect()->axis(QCPAxis::atLeft)->setRange(0, 1024);

    this->addGraph(xAxis, yAxis);

    this->clear();

}

CardioPlot::~CardioPlot()
{

}

void CardioPlot::add(double value, int time)
{
    //if (value < this->axisRect()->axis(QCPAxis::atLeft)->range().maxRange) {
    //    this->axisRect()->axis(QCPAxis::atLeft)->setRange(0, value);
    //}

    if (time > this->axisRect()->axis(QCPAxis::atBottom)->range().upper) {
        int timeDiff = time - this->axisRect()->axis(QCPAxis::atBottom)->range().upper;
        this->axisRect()->axis(QCPAxis::atBottom)->setRangeLower(this->axisRect()->axis(QCPAxis::atBottom)->range().lower + timeDiff);
        this->axisRect()->axis(QCPAxis::atBottom)->setRangeUpper(this->axisRect()->axis(QCPAxis::atBottom)->range().upper + timeDiff);
    }

    this->graph(0)->addData(time, value);
    this->replot();
}

void CardioPlot::clear() {
    QVector<double> initial_data = QVector<double>();
    initial_data.append(0.0);
    this->graph(0)->setData(initial_data, initial_data);
    this->replot();
}

void CardioPlot::saveAsCSV(const QString &filename) {
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QTextStream stream( &file );
    stream << "Time,Value" << endl;
    for(QMap< double, QCPData >::const_iterator i = this->graph(0)->data()->constBegin(); i != this->graph(0)->data()->constEnd(); i++) {
        QString s = QString::number(i.value().key) + ',' + QString::number(i.value().value);
        stream << s << endl;
    }
    file.close();
}

