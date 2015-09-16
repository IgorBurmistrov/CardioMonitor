#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QPushButton>
#include <QComboBox>
#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(this->ui->rescanButton, &QPushButton::clicked, this, &MainWindow::updateSerialPorts);
    connect(this->ui->startButton, &QPushButton::clicked, this, &MainWindow::startRecord);
    connect(this->ui->pauseButton, &QPushButton::clicked, this, &MainWindow::pauseRecord);
    connect(this->ui->stopButton, &QPushButton::clicked, this, &MainWindow::stopRecord);
    connect(this->ui->saveButton, &QPushButton::clicked, this, &MainWindow::save);
    connect(this->ui->serialportList, SIGNAL(currentIndexChanged(int)), this, SLOT(selectSerialPort()));

    this->m_time = new QTime();

}

MainWindow::~MainWindow()
{
    delete ui;
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
    dynamic_cast<QSerialPort*>(this->m_iodev)->setBaudRate(QSerialPort::Baud9600);
    ui->statusBar->showMessage("Serial port at " + dynamic_cast<QSerialPort*>(this->m_iodev)->portName() + " selected");
}

void MainWindow::startRecord() {
    if (!this->m_iodev->isOpen()) {
        if (!this->m_iodev->open(QIODevice::ReadOnly)) {
           ui->statusBar->showMessage("Error to open device");
           return;
        }
        this->m_time->restart();
        this->ui->cardioPlot->clear();
    }
    connect(this->m_iodev, &QIODevice::readyRead, this, &MainWindow::tryToRead);
    ui->statusBar->showMessage("Start recieving data");
}

void MainWindow::pauseRecord() {
    disconnect(this->m_iodev, &QIODevice::readyRead, this, &MainWindow::tryToRead);
    ui->statusBar->showMessage("Pause recieving data");
}

void MainWindow::stopRecord() {
    disconnect(this->m_iodev, &QIODevice::readyRead, this, &MainWindow::tryToRead);
    this->m_iodev->close();
    ui->statusBar->showMessage("Stop recieving data");
}

void MainWindow::tryToRead() {
    static double max = 0;
    static double min = 0;
    if (this->m_iodev->bytesAvailable() && this->m_iodev->canReadLine()) {
        QByteArray data = this->m_iodev->readLine();
        data.truncate(data.length()-2);
        if (data.size() == 0) {
            qDebug("Error occured!");
        } else {
            bool convStatus;
            double recievedData = data.toDouble(&convStatus);
            if (convStatus) {
                int time = this->m_time->elapsed();
                this->ui->cardioPlot->add(recievedData, time);
                if (recievedData > max) max = recievedData;
                if (recievedData < min) min = recievedData;
                ui->statusBar->showMessage("Value " + QString::number(recievedData) + " recived at " + QString::number(time) + " ms. (Max: " + QString::number(max) + " Min: " + QString::number(min) + ")");
            } else {
                qDebug("Error occured! Recieved data cannot be converted! Given input is: " + data);
            }
        }
    }
}

void MainWindow::save() {

    QString filename = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("CSV (*.csv)"));

    this->ui->cardioPlot->saveAsCSV(filename + ".csv");

    this->ui->statusBar->showMessage(tr("File saved"));

}
