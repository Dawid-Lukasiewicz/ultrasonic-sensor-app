#ifndef MEASUREWINDOW_H
#define MEASUREWINDOW_H

#include <QDialog>
#include <QSerialPort>
#include <QDateTime>
#include <QColor>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include "qcustomplot.h"

#define GRAPH_COUNT 5
#define SENSOR_RANGE 50

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
    void DrawDataPlot(const QVector<double> &X, const QVector<double> &Y);
    void DrawDataPlot();
    void GenerateAndDraw();

private slots:
    void ReadFromPort();
    void SendToLogs(const QString &message);
    void on_BackMeasureWindow_clicked();
    void on_StartMeasureWindow_clicked();

    void on_SetLocationMeasureWindow_clicked();

    void on_SaveMeasureWindow_clicked();

    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::MeasureWindow *ui;
    QSerialPort *Device;
    int m_positionValue;
    QVector<double> m_L;
    QVector<double> m_X;
    QVector<double> m_Y;
    QColor m_GraphColourIndex[4] = {Qt::blue, Qt::green, Qt::yellow, Qt::cyan};
    int m_GraphIndex;
};

#endif // MEASUREWINDOW_H
