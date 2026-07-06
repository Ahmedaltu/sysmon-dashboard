#pragma once
#include <QWidget>
#include "LinuxSystemMonitor.h"

class MetricCard;
class QTimer;

class DashboardWindow : public QWidget {
    Q_OBJECT
public:
    explicit DashboardWindow(QWidget *parent = nullptr);

private slots:
    void onTick();

protected:
    bool eventFilter(QObject *obj, QEvent *e) override;

private:
    LinuxSystemMonitor m_mon;
    QTimer            *m_timer;
    MetricCard        *m_cpuCard;
    MetricCard        *m_ramCard;
    MetricCard        *m_netCard;
};
