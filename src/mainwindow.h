#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QIODevice>
#include <QTime>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void tryToRead();

private slots:
    void updateSerialPorts();
    void selectSerialPort();
    void startRecord();
    void pauseRecord();
    void stopRecord();

private:
    Ui::MainWindow *ui;
    QIODevice* m_iodev;
    QTime* m_time;
    QTimer* m_timer;
};

#endif // MAINWINDOW_H
