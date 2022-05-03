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

//    Generate range of sensor
    QVector<double> X, Y;
    for(int i = -50; i < 51; i++)
    {
        X.push_back(static_cast<double>(i));
        Y.push_back(static_cast<double>(sqrt(50*50 - i*i)));
    }
    ui->MeasureWindowPlot->graph(0)->setData(X, Y);
    ui->MeasureWindowPlot->replot();
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
    ui->MeasureWindowPlot->addGraph();
    int GraphIndex = ui->MeasureWindowPlot->graphCount() - 1;
    ui->MeasureWindowPlot->graph(GraphIndex)->setData(X, Y);
    ui->MeasureWindowPlot->replot();
}

void MeasureWindow::DrawDataPlot()
{
    ui->MeasureWindowPlot->addGraph();
    int GraphIndex = ui->MeasureWindowPlot->graphCount() - 1;
    ui->MeasureWindowPlot->graph(GraphIndex)->setData(m_X, m_Y, true);
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
        int __IndexRight = line.lastIndexOf("\n");
        int __IndexLeft = __IndexRight - 1;
        if(line.at(0) == ":")
        {
            QString tmp = line.right(__IndexRight);
            QList<QString> list = tmp.left(__IndexLeft).split(" ");
            SendToLogs("L=" + list.first() + " X=" + list.at(1) + " Y=" + list.at(2));
            m_L.push_back(list.first().toDouble());
            m_X.push_back(list.at(1).toDouble());
            m_Y.push_back(list.at(2).toDouble());
        }
    }
    DrawDataPlot();
}

void MeasureWindow::SendToLogs(const QString &message)
{
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
    QString Date;
    Date += QDateTime::currentDateTime().toString("hh-mm-ss__dd.MM.yyyy");
//    Probably gonna need to set some functions to find current directory
    QFile DataFile("/home/dawid/QT-workspace/Sensor_Project-WDS/datas/" + Date + ".txt");

    if(!DataFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        SendToLogs("Not opened");
        return;
    }
    if(DataFile.exists())
        SendToLogs("[INFO] Saving data to file\n" + Date + ".txt");

    QTextStream out(&DataFile);
    out << "L\tX\tY\n--------------------------\n";
    for(int i = 0; i < m_L.size(); i++)
    {
        out << m_L[i] << "\t" << m_X[i] << "\t" << m_Y[i] << "\n";
    }
    DataFile.close();
}

