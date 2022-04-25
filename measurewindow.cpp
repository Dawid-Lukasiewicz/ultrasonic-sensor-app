#include "measurewindow.h"
#include "ui_measurewindow.h"

MeasureWindow::MeasureWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasureWindow)
{
    ui->setupUi(this);
}

MeasureWindow::~MeasureWindow()
{
    delete ui;
}
