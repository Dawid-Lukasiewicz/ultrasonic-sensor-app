#include "setportwindow.h"
#include "ui_setportwindow.h"

SetPortWindow::SetPortWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SetPortWindow)
{
    ui->setupUi(this);
    Device = new QSerialPort;
}

SetPortWindow::~SetPortWindow()
{
    delete ui;
    delete Device;
}

void SetPortWindow::SendToDevice(const QString &message)
{
    if(Device->isOpen() && Device->isWritable())
    {
        SendToLogs(message);
        Device->write(message.toStdString().c_str());
    }
    else
    {
        SendToLogs("Failed to send message to device");
        return;
    }
}


void SetPortWindow::on_Search_clicked()
{
    QList<QSerialPortInfo> devices;
    devices = QSerialPortInfo::availablePorts();
    ui->SelectedPort->clear();
    for(int i = 0; i < devices.count(); i++)
    {
        SendToLogs("[FOUND] " + devices.at(i).portName() + "  " + devices.at(i).description());
        ui->SelectedPort->addItem(devices.at(i).portName() + "  " + devices.at(i).description());
    }

}

void SetPortWindow::on_Connect_clicked()
{
    if(ui->SelectedPort->count() == 0)
    {
        SendToLogs("No device found");
        return;
    }
    else if(Device->isOpen())
    {
        SendToLogs("Port " + Device->portName() + " already opened");
        return;
    }

    QString PortDeviceName = ui->SelectedPort->currentText().split("  ").first();
    Device->setPortName(PortDeviceName);
    if(Device->open(QSerialPort::ReadWrite))
    {
        Device->setBaudRate(QSerialPort::Baud9600);
        Device->setParity(QSerialPort::NoParity);
        Device->setDataBits(QSerialPort::Data8);
        Device->setStopBits(QSerialPort::OneStop);
        Device->setFlowControl(QSerialPort::NoFlowControl);

        SendToLogs("Serial port opened");
    }
    else
    {
        SendToLogs("Failed to open serial port");
        return;
    }

}

void SetPortWindow::on_Disconnect_clicked()
{
    if(Device->isOpen())
    {
        Device->close();
        SendToLogs("Port disconnected");
    }
    else
    {
        SendToLogs("No port connected");
    }
}

void SetPortWindow::SendToLogs(const QString &message)
{
    QString CurrentDate = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss -- ");
    ui->textEditLogs->append(CurrentDate + " " + message);
}

void SetPortWindow::ReadFromPort()
{
    while(Device->canReadLine())
    {
        QString line = Device->readLine();
        int index = line.lastIndexOf("\r");
        SendToLogs(line.left(index));
    }
}



void SetPortWindow::on_Measurement_clicked()
{
//    SendToDevice("1");
    this->hide();

    MeasureWindow ui_m(nullptr, this->Device);
    ui_m.setModal(true);
    ui_m.exec();


//    ui_m = new MeasureWindow(this, this->Device);
//    ui_m->show();
}


void SetPortWindow::on_Exit_clicked()
{
    this->close();
}

