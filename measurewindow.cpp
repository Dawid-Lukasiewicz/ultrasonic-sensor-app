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
    ui->MeasureWindowPlot->xAxis->setRange(-50, 50);
    ui->MeasureWindowPlot->yAxis->setRange(0, 50);

    ui->MeasureWindowPlot->setInteractions(QCP::iRangeZoom | QCP::iRangeDrag | QCP::iSelectPlottables);
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
    for(int i = -50; i < 51; i++)
    {
        X.push_back(static_cast<double>(i));
        Y.push_back(static_cast<double>(sqrt(50*50 - i*i)));
    }

    DrawDataPlot(X, Y);
}

void MeasureWindow::ReadFromPort()
{
    while(Device->canReadLine())
    {
        QString line = Device->readLine();
        int index_ = line.lastIndexOf("\n") - 2;
        if(line.at(0) == ":")
        {
            QList<QString> list = (line+1).left(index_).split(" ");
            SendToLogs("L=" + list.first() + " X=" + list.at(1) + " Y=" + list.at(2));
            m_L.push_back(list.first().toInt());
            m_X.push_back(list.at(1).toDouble());
            m_Y.push_back(list.at(2).toDouble());
        }
    }
}

void MeasureWindow::SendToLogs(const QString &message)
{
    // NEED TO CHANGE THE WAY I SHOW THE DATA, NO DATETIME NEEDED I GUESS
    QString CurrentDate = QDateTime::currentDateTime().toString("hh:mm:ss -- ");
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
    GenerateAndDraw();

    QString Date;
    Date += QDateTime::currentDateTime().toString("hh:mm:ss__dd.MM.yyyy");
    QFile DataFile("/home/dawid/QT-workspace/Sensor_Project-WDS/datas/" + Date);

    if(!DataFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        SendToLogs("Not opened");
        return;
    }
    if(DataFile.exists())
        SendToLogs("It exists");

    QTextStream out(&DataFile);
    out << "L\tX\tY\n--------------------------\n";
    for(int i = 0; i < m_L.size(); i++)
    {
        out << m_L[i] << "\t" << m_X[i] << "\t" << m_Y[i] << "\n";
    }
    DataFile.close();
//    DrawDataPlot();
}

