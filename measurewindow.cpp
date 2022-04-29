#include "measurewindow.h"
#include "ui_measurewindow.h"

MeasureWindow::MeasureWindow(QWidget *parent, QSerialPort *device) :
    QDialog(parent),
    ui(new Ui::MeasureWindow),
    Device(new QSerialPort)
{
    ui->setupUi(this);
    Device = device;
    ui->MeasureWindowPlot->addGraph();
//    ui->MeasureWindowPlot->graph(0)->setPen()
    ui->MeasureWindowPlot->xAxis->setRange(0, 100);
    ui->MeasureWindowPlot->yAxis->setRange(0, 50);
}

MeasureWindow::~MeasureWindow()
{
    delete ui;
}

void MeasureWindow::SendToDevice(const QString &message)
{
    if(Device->isOpen() && Device->isWritable())
    {
        Device->write(message.toStdString().c_str());
    }
    else
    {
        return;
    }
}

void MeasureWindow::DrawDataPlot(const QVector<double> &X, const QVector<double> &Y)
{
    ui->MeasureWindowPlot->graph(0)->setData(X, Y);
    ui->MeasureWindowPlot->replot();
}

void MeasureWindow::GenerateAndDraw()
{
    QVector<double> X, Y;
    for(int i = 0; i < 100; i++)
    {
        X.push_back(static_cast<double>(i));
        Y.push_back(static_cast<double>(i/10));
    }

    DrawDataPlot(X, Y);
}

void MeasureWindow::ReadFromPort()
{
    while(Device->canReadLine())
    {
        QString line = Device->readLine();
        int index = line.lastIndexOf("\r");
        SendToLogs(line.left(index));
    }
}

void MeasureWindow::SendToLogs(const QString &message)
{
    // NEED TO CHANGE THE WAY I SHOW THE DATA, NO DATETIME NEEDED I GUESS
    QString CurrentDate = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss -- ");
    ui->DataMeasureWindow->append(CurrentDate + " " + message);
}

void MeasureWindow::on_BackMeasureWindow_clicked()
{
    this->close();
}


void MeasureWindow::on_StartMeasureWindow_clicked()
{
    SendToLogs("Start");
    connect(this->Device, SIGNAL(readyRead()), this, SLOT(ReadFromPort()));
    SendToDevice("1");

    GenerateAndDraw();
}

