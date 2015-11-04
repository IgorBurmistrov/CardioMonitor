#include "cardioplot.h"


CardioPlot::CardioPlot(QWidget* parent) : QCustomPlot(parent) {

    this->setInteraction(QCP::iRangeDrag, true);
    this->axisRect()->setRangeDrag(Qt::Horizontal);
    this->axisRect()->setRangeDragAxes(this->axisRect()->axis(QCPAxis::atBottom), this->axisRect()->axis(QCPAxis::atLeft));
    this->axisRect()->axis(QCPAxis::atBottom)->setAutoTickStep(false);
    this->axisRect()->axis(QCPAxis::atLeft)->setAutoTickStep(false);
    this->axisRect()->axis(QCPAxis::atBottom)->setRange(0, 5000);
    this->axisRect()->axis(QCPAxis::atBottom)->setTickStep(1000);

    this->addGraph(xAxis, yAxis);

    this->m_data = new(QCPDataMap);
    setValueDisplayMode(DM_Units);

    this->clear();

}

CardioPlot::~CardioPlot() {

}

void CardioPlot::setValueDisplayMode(int mode) {
        this->m_displayMode = mode;
        if (mode == DM_Units) {
            this->axisRect()->axis(QCPAxis::atLeft)->setLabel("Units");
            this->axisRect()->axis(QCPAxis::atLeft)->setRange(0, 1024);
            this->axisRect()->axis(QCPAxis::atLeft)->setTickStep(128);
            this->graph(0)->setData(this->m_data, true);
        } else {
            this->axisRect()->axis(QCPAxis::atLeft)->setLabel("Volts [V]");
            this->axisRect()->axis(QCPAxis::atLeft)->setRange(0, 5);
            this->axisRect()->axis(QCPAxis::atLeft)->setTickStep(0.5);

            this->graph(0)->clearData();


            for (QCPDataMap::iterator item = this->m_data->begin(); item != this->m_data->end(); item++)
                this->graph(0)->addData(convertData(item.value()));
        }
        this->replot();
}

void CardioPlot::setTimeRange(int range) {
    this->axisRect()->axis(QCPAxis::atBottom)->setRange(0, range);
    this->replot();
}

void CardioPlot::setValueRange(int from, int to) {
    this->axisRect()->axis(QCPAxis::atLeft)->setRange(from, to);
    this->replot();
}

void CardioPlot::add(double value, int time)
{
    //if (value < this->axisRect()->axis(QCPAxis::atLeft)->range().maxRange) {
    //    this->axisRect()->axis(QCPAxis::atLeft)->setRange(0, value);
    //}

    this->m_data->insert(time, QCPData(time, value));
    this->graph(0)->addData(convertData(QCPData(time, value)));

    if (time > this->axisRect()->axis(QCPAxis::atBottom)->range().upper) {
        int timeDiff = time - this->axisRect()->axis(QCPAxis::atBottom)->range().upper;
        this->axisRect()->axis(QCPAxis::atBottom)->setRangeLower(this->axisRect()->axis(QCPAxis::atBottom)->range().lower + timeDiff);
        this->axisRect()->axis(QCPAxis::atBottom)->setRangeUpper(this->axisRect()->axis(QCPAxis::atBottom)->range().upper + timeDiff);
    }

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

QCPData CardioPlot::convertData(const QCPData & src) {
    const double voltsPerUnit = 0.0049;
    return QCPData(src.key, this->m_displayMode == DM_Units ? src.value : (src.value * voltsPerUnit));
}
