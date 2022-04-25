#include "measurewindow.h"
#include "ui_measurewindow.h"
#include "setportwindow.h"

SetPortWindow *ui_s;
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

void MeasureWindow::on_BackMeasureWindow_clicked()
{
    this->hide();
    ui_s = new SetPortWindow(this);
    ui_s->show();
}

