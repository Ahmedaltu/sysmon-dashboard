#include "dashboardwindow.h"
#include "./ui_dashboardwindow.h"

DashboardWindow::DashboardWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DashboardWindow)
{
    ui->setupUi(this);
}

DashboardWindow::~DashboardWindow()
{
    delete ui;
}
