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
    void changeDisplayMode(int);
    void setTimeRange(int);
    void setValueRange();
    void updateSerialPorts();
    void selectSerialPort();
    void startRecord();
    void pauseRecord();
    void stopRecord();
    void save();

private:
    Ui::MainWindow *ui;
    QIODevice* m_iodev;
    int m_last_time;
    int m_time_diff;
    bool m_time_sync;
};

#endif // MAINWINDOW_H
