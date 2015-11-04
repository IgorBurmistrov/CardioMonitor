#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QPushButton>
#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_last_time(0),
    m_time_diff(0),
    m_time_sync(false)
{
    ui->setupUi(this);

    ui->displayUnits->setChecked(true);

    connect(this->ui->rescanButton, &QPushButton::clicked, this, &MainWindow::updateSerialPorts);
    connect(this->ui->startButton, &QPushButton::clicked, this, &MainWindow::startRecord);
    connect(this->ui->pauseButton, &QPushButton::clicked, this, &MainWindow::pauseRecord);
    connect(this->ui->stopButton, &QPushButton::clicked, this, &MainWindow::stopRecord);
    connect(this->ui->saveButton, &QPushButton::clicked, this, &MainWindow::save);
    connect(this->ui->serialportList, SIGNAL(currentIndexChanged(int)), this, SLOT(selectSerialPort()));
    connect(this->ui->displayMode, SIGNAL(buttonClicked(int)), this, SLOT(changeDisplayMode(int)));
    connect(this->ui->timeSlider, SIGNAL(valueChanged(int)), this, SLOT(setTimeRange(int)));
    connect(this->ui->fromSpin, SIGNAL(valueChanged(int)), this, SLOT(setValueRange()));
    connect(this->ui->toSpin, SIGNAL(valueChanged(int)), this, SLOT(setValueRange()));

    this->ui->displayMode->setId(this->ui->displayUnits, DM_Units);
    this->ui->displayMode->setId(this->ui->displayVolts, DM_Volts);

    this->ui->toSpin->setMaximum(1024);
    this->ui->fromSpin->setMaximum(1024);

    this->ui->fromSpin->setValue(0);
    this->ui->toSpin->setValue(1024);

    this->m_iodev = NULL;

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::changeDisplayMode(int id) {
    this->ui->fromSpin->setValue(0);
    if (id == DM_Units) {
        this->ui->toSpin->setValue(1024);
    } else {
        this->ui->toSpin->setValue(5);
    }
    this->ui->cardioPlot->setValueDisplayMode(id);
}

void MainWindow::setTimeRange(int value) {
    this->ui->cardioPlot->setTimeRange(value*1000);
}

void MainWindow::setValueRange() {
    this->ui->cardioPlot->setValueRange(this->ui->fromSpin->value(), this->ui->toSpin->value());
}

void MainWindow::updateSerialPorts() {
    this->ui->serialportList->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QString itemName;
        if (info.description().isEmpty()) {
            itemName = info.portName();
        } else {
            itemName = info.portName() + " " + info.description();
        }
        QSerialPort* serialPort = new QSerialPort(info);
        this->ui->serialportList->addItem(itemName, QVariant::fromValue(serialPort));
    }
}

void MainWindow::selectSerialPort() {
    this->m_iodev = this->ui->serialportList->currentData().value<QSerialPort*>();
    dynamic_cast<QSerialPort*>(this->m_iodev)->setBaudRate(QSerialPort::Baud19200);
    ui->statusBar->showMessage("Serial port at " + dynamic_cast<QSerialPort*>(this->m_iodev)->portName() + " selected");
}

void MainWindow::startRecord() {
    if (m_iodev) {
        if (!this->m_iodev->isOpen()) {
            if (!this->m_iodev->open(QIODevice::ReadOnly)) {
               ui->statusBar->showMessage("Error to open device");
               return;
            }
            this->ui->cardioPlot->clear();
        }
        connect(this->m_iodev, &QIODevice::readyRead, this, &MainWindow::tryToRead);
        this->m_time_sync = false;
        ui->statusBar->showMessage("Start recieving data");
    }
}

void MainWindow::pauseRecord() {
    if (m_iodev) {
        disconnect(this->m_iodev, &QIODevice::readyRead, this, &MainWindow::tryToRead);
        ui->statusBar->showMessage("Pause recieving data");
    }
}

void MainWindow::stopRecord() {
    if (m_iodev) {
        disconnect(this->m_iodev, &QIODevice::readyRead, this, &MainWindow::tryToRead);
        this->m_iodev->close();
        this->m_last_time = 0;
        ui->statusBar->showMessage("Stop recieving data");
    }
}

void MainWindow::tryToRead() {
    if (m_iodev) {
        static double max = 0;
        static double min = 0;
        if (this->m_iodev->bytesAvailable() && this->m_iodev->canReadLine()) {
            QByteArray data = this->m_iodev->readLine();
            data.truncate(data.length()-2);
            if (data.size() == 0) {
                qDebug("Error occured!");
            } else {
                bool convStatus;
                int recievedData = data.toInt(&convStatus);
                if (convStatus) {
                    int time = recievedData & 0x003FFFFF;
                    int data = recievedData >> 22;
                    if (!this->m_time_sync) {
                        this->m_time_diff = this->m_last_time - time;
                        this->m_time_sync = true;
                    }
                    this->ui->cardioPlot->add(data, time + this->m_time_diff);
                    if (recievedData > max) max = data;
                    if (recievedData < min) min = data;
                    ui->statusBar->showMessage("Value " + QString::number(data) + " recived at " + QString::number(time) + " ms. (Max: " + QString::number(max) + " Min: " + QString::number(min) + ")");
                    this->m_last_time = time;
                } else {
                    qDebug("Error occured! Recieved data cannot be converted! Given input is: " + data);
                }
            }
        }
    }
}

void MainWindow::save() {

    QString filename = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("CSV (*.csv)"));

    this->ui->cardioPlot->saveAsCSV(filename + ".csv");

    this->ui->statusBar->showMessage(tr("File saved"));

}
