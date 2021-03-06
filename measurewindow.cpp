#include "measurewindow.h"
#include "ui_measurewindow.h"

/**
 * @brief Constructor of measurement window class
 *
 * @details The constructor sets whole window and addittionaly
 * sets the first graph to display the range of sensor as a semicircle
 * draws ellipse indicating default localization of sensor
 * additionally sets range for slider and its default value
 *
 * @param[in, out] parent - Basic argument
 *
 * @param device[in, out] - Passed device connected in main window
 */
MeasureWindow::MeasureWindow(QWidget *parent, QSerialPort *device) :
    QDialog(parent),
    ui(new Ui::MeasureWindow),
    Device(new QSerialPort)
{
    ui->setupUi(this);

    /* Passing the selected serial port to new current window */
    Device = device;

    /* Setting QLCDNumber lcdNumber parameters and Qslider horizontalSlider */
    ui->lcdNumber->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdNumber->setBackgroundRole(QPalette::Light);
    ui->horizontalSlider->setMinimum(-SENSOR_RANGE);
    ui->horizontalSlider->setMaximum(SENSOR_RANGE);
    ui->horizontalSlider->setValue(0);
    ui->lcdNumber->display(ui->horizontalSlider->value());

    /* Setting first graph defining maximum sensor range */
    ui->MeasureWindowPlot->addGraph();
    ui->MeasureWindowPlot->graph(0)->setPen(QPen(Qt::red));
    ui->MeasureWindowPlot->xAxis->setRange(-SENSOR_RANGE - 10, SENSOR_RANGE + 10);
    ui->MeasureWindowPlot->yAxis->setRange(0, SENSOR_RANGE + 10);
//    ui->MeasureWindowPlot->setInteractions(QCP::iRangeZoom | QCP::iRangeDrag | QCP::iSelectPlottables);

    /* Draw sensor current position */
    m_positionValue = ui->horizontalSlider->value();
    m_sensorPosition = new QCPItemEllipse(ui->MeasureWindowPlot);
    m_sensorPosition->setPen(QPen(Qt::black));
    m_sensorPosition->setBrush(QBrush(Qt::red));
    m_sensorPosition->topLeft->setCoords(-SENSOR_ELLIPSE, SENSOR_ELLIPSE);
    m_sensorPosition->bottomRight->setCoords(SENSOR_ELLIPSE, -SENSOR_ELLIPSE);
    m_sensorPosition->setVisible(true);

    /* Generate range of sensor */
    QVector<double> X, Y;
    for(int i = -SENSOR_RANGE; i < SENSOR_RANGE + 1; ++i)
    {
        X.push_back(static_cast<double>(i));
        Y.push_back(static_cast<double>(sqrt(SENSOR_RANGE*SENSOR_RANGE - i*i)));
    }
    ui->MeasureWindowPlot->graph(0)->setData(X, Y, true);
    ui->MeasureWindowPlot->replot();
}

/**
 * @brief Deleting user interface class
 */
MeasureWindow::~MeasureWindow()
{
    delete ui;
}

/**
 * @brief Sends message to connected device
 *
 * @param[in] message - To be sent to device
 *
 * @retval None
 */
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

/**
 * @brief Drawing data on new graph
 *
 * @param[in] X - Vector of data
 *
 * @param[in] Y - Vector of data
 *
 * @retval None
 */
void MeasureWindow::DrawDataPlot(const QVector<double> &X, const QVector<double> &Y)
{
    ui->MeasureWindowPlot->addGraph();
    int GraphIndex = ui->MeasureWindowPlot->graphCount() - 1;
    ui->MeasureWindowPlot->graph(GraphIndex)->setData(X, Y);
    ui->MeasureWindowPlot->replot();
}

/**
 * @brief Draws plot on new graph from previously saved data in X and Y
 *
 * @retval None
 */
void MeasureWindow::DrawDataPlot()
{
    ui->MeasureWindowPlot->graph(m_GraphIndex)->setData(m_X, m_Y, true);
    ui->MeasureWindowPlot->replot();
}

/**
 * @brief Draws a semicircle marking the device range
 *
 * @retval None
 */
void MeasureWindow::GenerateAndDraw()
{
    QVector<double> X, Y;
    for(int i = -SENSOR_RANGE; i < SENSOR_RANGE + 1; i++)
    {
        X.push_back(static_cast<double>(i));
        Y.push_back(static_cast<double>(sqrt(SENSOR_RANGE*SENSOR_RANGE - i*i)));
    }
    DrawDataPlot(X, Y);
}

/**
 * @brief Reading data from port and saving it
 *
 * @details The method reads from port, partitions the data frame and saving each part to assigned member vector
 *
 * @retval None
 */
void MeasureWindow::ReadFromPort()
{
    while(Device->canReadLine())
    {
        QString line = Device->readLine();
        int __IndexRight = line.lastIndexOf("\n");
        int __IndexLeft = __IndexRight - 1;
        double radianValue = 0.0;
        if(line.at(0) == ":")
        {
            QString tmp = line.right(__IndexRight);
            QList<QString> list = tmp.left(__IndexLeft).split(" ");
            m_L.push_back(list.first().toDouble());
            radianValue = list.at(1).toDouble();
        }
        m_X.push_back( m_L.last() * cos(radianValue) + m_positionValue);
        m_Y.push_back( m_L.last() * sin(radianValue) );
        // This while loop is poorly designed
        while( sqrt( pow(m_X.last(), 2) + pow(m_Y.last(), 2) ) > SENSOR_RANGE)
        {
            if(m_L.last() < 0.5)
                m_L.last() = 0.0;
            else
                m_L.last() -= 0.5;
            m_X.last() =  m_L.last() * cos(radianValue) + m_positionValue;
            m_Y.last() =  m_L.last() * sin(radianValue);
        }
        SendToLogs("L=" + QString::number(m_L.last(), 'g', 3) + " X=" + QString::number(m_X.last(), 'g', 3) + " Y=" + QString::number(m_Y.last(), 'g', 3));
    }
    DrawDataPlot();
}

/**
 * @brief Sending message to logs
 *
 * @param[in] message -  Message to be sent
 *
 * @retval None
 */
void MeasureWindow::SendToLogs(const QString &message)
{
    QString CurrentDate = QDateTime::currentDateTime().toString("hh:mm:ss -- ");
    ui->DataMeasureWindow->append(CurrentDate + " " + message);
}

/**
 * @brief Closes the measure window so that main window comes back
 *
 * @retval None
 */
void MeasureWindow::on_BackMeasureWindow_clicked()
{
    this->close();
}

/**
 * @brief Start measurement and display it on plot
 *
 * @details At first clears vectors containing data from previous measurement.
 * Then checks if there are too many graphs created, if so delete them. Adding
 * a new graph with predefined color. Sending readyRead signal to ReadFromPort
 * methode. Send a message to device in order to start measurement
 *
 * @retval None
 */
void MeasureWindow::on_StartMeasureWindow_clicked()
{
    SendToLogs(tr("Start Measurement\n") + "-----------------------------------------------------------");

    /* Clear data from vectors */
    m_L.clear();
    m_X.clear();
    m_Y.clear();

    /* See if need to remove graph */
    m_GraphIndex = ui->MeasureWindowPlot->graphCount();
    if(m_GraphIndex >= GRAPH_COUNT)
    {
        for(int i = 4; i > 0; i--)
        {
            ui->MeasureWindowPlot->removeGraph(i);
        }
        m_GraphIndex = ui->MeasureWindowPlot->graphCount();
        ui->MeasureWindowPlot->replot();
    }

    /* Adding new graph */
    ui->MeasureWindowPlot->addGraph();
    ui->MeasureWindowPlot->graph(m_GraphIndex)->setPen(QPen(m_GraphColourIndex[m_GraphIndex - 1]));

    /* Send readyRead signal to ReadFromPort slot */
    connect(this->Device, SIGNAL(readyRead()), this, SLOT(ReadFromPort()));

    /* Send sensor a ready signal */
    SendToDevice("1");
}

/**
 * @brief Save measured data to file
 *
 * @note File name format is "hh-mm-ss__dd.MM.yyyy.txt
 *      Data format is Length[cm]   X_position[cm]  Y_position[cm]
 *
 * @retval None
 */
void MeasureWindow::on_SaveMeasureWindow_clicked()
{
    /* Getting current date and saving to variable */
    QString Date;
    Date += QDateTime::currentDateTime().toString("hh-mm-ss__dd.MM.yyyy");

    /* Getting current path, moving to parent directory and creating "/datas/ direcotry if not existing */
    QDir dataDirectory(QDir().path());
    dataDirectory.cdUp();
    dataDirectory.mkdir(dataDirectory.path() + "/datas/");

    /* Creating file with current date name */
    QFile DataFile(dataDirectory.path() + "/datas/" + Date + ".txt");

    /* Opening file and then saving data in it */
    if(!DataFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        SendToLogs(tr("File not opened"));
        return;
    }
    if(DataFile.exists())
        SendToLogs(tr("[INFO] Saving data to file:\n") + Date + ".txt");

    QTextStream out(&DataFile);
    out.setRealNumberPrecision(3);
    out << "L[cm]\tX[cm]\tY[cm]\n--------------------------\n";
    for(int i = 0; i < m_L.size(); i++)
    {
        out << m_L[i] << "\t" << m_X[i] << "\t" << m_Y[i] << "\n";
    }
    DataFile.close();
}

/**
 * @brief Changing sensor localization vizualization and applying the changed value to coords calculation
 *
 * @param[in] value - new value of sensor localization
 *
 * @retval None
 */
void MeasureWindow::on_horizontalSlider_valueChanged(int value)
{
    m_positionValue = value;
    ui->lcdNumber->display(value);
    m_sensorPosition->topLeft->setCoords(value - SENSOR_ELLIPSE, SENSOR_ELLIPSE);
    m_sensorPosition->bottomRight->setCoords(value + SENSOR_ELLIPSE, -SENSOR_ELLIPSE);
    ui->MeasureWindowPlot->replot();
}

