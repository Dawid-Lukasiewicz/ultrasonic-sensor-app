#include "setportwindow.h"
#include "ui_setportwindow.h"

/**
 * @brief Basic constructor of main window
 *
 * @param parent - Basic argument
 */
SetPortWindow::SetPortWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SetPortWindow)
{
    ui->setupUi(this);
    Device = new QSerialPort;

    ui->SelectLanguage->addItem(tr("English"));
    ui->SelectLanguage->addItem(tr("Polish"));
}

/**
 * @brief Destructor addittionaly deletes the content of member Device pointer
 */
SetPortWindow::~SetPortWindow()
{
    delete ui;
    delete Device;
}

/**
 * @brief Sends message to connected device
 *
 * @param message - To be sent to device
 *
 * @retval None
 */
void SetPortWindow::SendToDevice(const QString &message)
{
    if(Device->isOpen() && Device->isWritable())
    {
        Device->write(message.toStdString().c_str());
    }
    else
    {
        SendToLogs(tr("[ERROR] Failed to send message to device"));
        return;
    }
}

/**
 * @brief Searches avalaible devices attached and saves it in list
 *
 * @retval None
 */
void SetPortWindow::on_Search_clicked()
{
    QList<QSerialPortInfo> devices;
    devices = QSerialPortInfo::availablePorts();
    ui->SelectedPort->clear();
    for(int i = 0; i < devices.count(); i++)
    {
        SendToLogs(tr("[INFO] ") + devices.at(i).portName() + "  " + devices.at(i).description()) ;
        ui->SelectedPort->addItem(devices.at(i).portName() + "  " + devices.at(i).description());
    }

}

/**
 * @brief Connect the chosen in list device
 *
 * @retval None
 */
void SetPortWindow::on_Connect_clicked()
{
    if(ui->SelectedPort->count() == 0)
    {
        SendToLogs(tr("[INFO] No device found"));
        return;
    }
    else if(Device->isOpen())
    {
        SendToLogs(tr("[ERROR] Port ") + Device->portName() + tr(" already opened"));
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

        SendToLogs(tr("[INFO] Serial port opened"));
    }
    else
    {
        SendToLogs(tr("[ERROR] Failed to open serial port"));
        return;
    }

}

/**
 * @brief Disconnect from device
 *
 * @retval None
 */
void SetPortWindow::on_Disconnect_clicked()
{
    if(Device->isOpen())
    {
        Device->close();
        SendToLogs(tr("[INFO] Port disconnected"));
    }
    else
    {
        SendToLogs(tr("[ERROR] No port connected"));
    }
}

/**
 * @brief Sending message to logs
 *
 * @details The message is preceded with current time in format yyyy.MM.dd hh:mm:ss --
 *
 * @param message - Message to be sent
 *
 * @retval None
 */
void SetPortWindow::SendToLogs(const QString &message)
{
    QString CurrentDate = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss -- ");
    ui->textEditLogs->append(CurrentDate + " " + message);
}

/**
 * @brief Reading data from port and saving it
 *
 * @retval None
 */
void SetPortWindow::ReadFromPort()
{
    while(Device->canReadLine())
    {
        QString line = Device->readLine();
        int index = line.lastIndexOf("\r");
        SendToLogs(line.left(index));
    }
}


/**
 * @brief Hide the main window and open measurement window
 *
 * @retval None
 */
void SetPortWindow::on_Measurement_clicked()
{
    this->hide();

    MeasureWindow ui_m(nullptr, this->Device);
    ui_m.setWindowTitle(tr("Measurement Window"));
    ui_m.setModal(true);
    ui_m.exec();

    this->show();
}

/**
 * @brief Close application
 *
 * @retval None
 */
void SetPortWindow::on_Exit_clicked()
{
    this->close();
}


void SetPortWindow::on_SelectLanguage_currentIndexChanged(int index)
{
    static QTranslator *translate = new QTranslator();

    switch(index)
    case 0:
    {
        translate->load("pro/SetPortWindow_en_150.qm", ".");

    }
}

