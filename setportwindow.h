#ifndef SETPORTWINDOW_H
#define SETPORTWINDOW_H

#include <QDebug>
#include <QList>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QDateTime>

#include <QMainWindow>

#include "measurewindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SetPortWindow; }
QT_END_NAMESPACE

class SetPortWindow : public QMainWindow
{
    Q_OBJECT

public:
    SetPortWindow(QWidget *parent = nullptr);
    ~SetPortWindow();
    void SendToDevice(const QString &message);

private slots:
    void on_Search_clicked();
    void on_Connect_clicked();
    void on_Disconnect_clicked();
    void SendToLogs(const QString &message);
    void ReadFromPort();
    void on_Measurement_clicked();

    void on_Exit_clicked();

private:
    Ui::SetPortWindow *ui;
    MeasureWindow *ui_m;
    QSerialPort *Device;
};
#endif // SETPORTWINDOW_H
