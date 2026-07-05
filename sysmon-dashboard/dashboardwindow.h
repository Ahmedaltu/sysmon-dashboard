#ifndef DASHBOARDWINDOW_H
#define DASHBOARDWINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class DashboardWindow;
}
QT_END_NAMESPACE

class DashboardWindow : public QWidget
{
    Q_OBJECT

public:
    DashboardWindow(QWidget *parent = nullptr);
    ~DashboardWindow();

private:
    Ui::DashboardWindow *ui;
};
#endif // DASHBOARDWINDOW_H
