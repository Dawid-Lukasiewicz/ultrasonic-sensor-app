#ifndef MEASUREWINDOW_H
#define MEASUREWINDOW_H

#include <QDialog>
#include <QSerialPort>
#include <QDateTime>

namespace Ui {
class MeasureWindow;
}

class MeasureWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MeasureWindow(QWidget *parent = nullptr, QSerialPort *device = nullptr);
    ~MeasureWindow();
    void SendToDevice(const QString &message);

private slots:
    void ReadFromPort();
    void SendToLogs(const QString &message);
    void on_BackMeasureWindow_clicked();
    void on_StartMeasureWindow_clicked();

private:
    Ui::MeasureWindow *ui;
    QSerialPort *Device;
};

#endif // MEASUREWINDOW_H
