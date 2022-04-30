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
    ui->MeasureWindowPlot->graph(0)->setPen(QPen(Qt::red));
    ui->MeasureWindowPlot->xAxis->setRange(0, 50);
    ui->MeasureWindowPlot->yAxis->setRange(-50, 50);
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

void MeasureWindow::DrawDataPlot()
{
    ui->MeasureWindowPlot->graph(0)->setData(m_X, m_Y);
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
//        int index = line.lastIndexOf("\r");
//        SendToLogs(line.left(index));

        int index_ = line.lastIndexOf("\n") - 2;
        SendToLogs(line);
        if(line.at(0) == ":")
        {
            QList<QString> list = (line+1).left(index_).split(" ");
            m_L.push_back(list.first().toInt());
            m_X.push_back(list.at(1).toDouble());
            m_Y.push_back(list.at(2).toDouble());
        }
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
}


void MeasureWindow::on_SetLocationMeasureWindow_clicked()
{
    DrawDataPlot();
}

