#include "dashboardwindow.h"
#include "metriccard.h"
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QTimer>
#include <QWindow>

DashboardWindow::DashboardWindow(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setStyleSheet("DashboardWindow { background: #07070f; }");
    setMinimumSize(800, 400);

    auto first = m_mon.poll(); // prime delta state; first call returns 0 for rates

    m_cpuCard = new MetricCard("CPU", QColor("#ff4444"), this);
    m_ramCard = new MetricCard("RAM", QColor("#ffcc00"), this);
    m_netCard = new MetricCard("NET", QColor("#00ccff"), this);

    m_cpuCard->setBars(first.cpu.corePercent);
    m_ramCard->setBars({first.mem.usedGB}, first.mem.totalGB > 0 ? first.mem.totalGB : 1.f);
    m_netCard->setBars({first.net.downloadKBs, first.net.uploadKBs}, 100.f);

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);
    layout->addWidget(m_cpuCard);
    layout->addWidget(m_ramCard);
    layout->addWidget(m_netCard);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &DashboardWindow::onTick);
    m_timer->start(1000);

    // Install on every descendant so mouse events from any child are intercepted
    for (auto *w : findChildren<QWidget*>())
        w->installEventFilter(this);
}

bool DashboardWindow::eventFilter(QObject *obj, QEvent *e)
{
    Q_UNUSED(obj)
    if (e->type() == QEvent::MouseButtonPress) {
        auto *me = static_cast<QMouseEvent*>(e);
        if (me->button() == Qt::LeftButton) {
            if (windowHandle())
                windowHandle()->startSystemMove();
        }
    }
    return false;
}

void DashboardWindow::onTick()
{
    auto s = m_mon.poll();

    // CPU: per-core bars, head = average %, sparkline tracks average
    m_cpuCard->setBars(s.cpu.corePercent);
    if (!s.cpu.corePercent.isEmpty()) {
        float avg = 0;
        for (float v : s.cpu.corePercent) avg += v;
        avg /= s.cpu.corePercent.size();
        m_cpuCard->setHeadValue(QString("%1%").arg(int(avg)));
        m_cpuCard->pushSpark(avg);
    }

    // RAM: single bar, head = used/total, sparkline tracks used GB
    m_ramCard->setBars({s.mem.usedGB}, s.mem.totalGB > 0 ? s.mem.totalGB : 1.f);
    m_ramCard->setHeadValue(QString("%1/%2 GB")
        .arg(s.mem.usedGB,  0, 'f', 1)
        .arg(s.mem.totalGB, 0, 'f', 1));
    m_ramCard->pushSpark(s.mem.usedGB);

    // NET: two bars (down/up), auto-scaled, sparkline tracks download
    float netMax = qMax(100.f, qMax(s.net.downloadKBs, s.net.uploadKBs) * 1.2f);
    m_netCard->setBars({s.net.downloadKBs, s.net.uploadKBs}, netMax);
    m_netCard->setHeadValue(QString("↓%1 ↑%2")
        .arg(s.net.downloadKBs, 0, 'f', 0)
        .arg(s.net.uploadKBs,   0, 'f', 0));
    m_netCard->pushSpark(s.net.downloadKBs);
}
