#ifndef MEASUREWINDOW_H
#define MEASUREWINDOW_H

#include <QDialog>

namespace Ui {
class MeasureWindow;
}

class MeasureWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MeasureWindow(QWidget *parent = nullptr);
    ~MeasureWindow();

private:
    Ui::MeasureWindow *ui;
};

#endif // MEASUREWINDOW_H
