#include "LinuxSystemMonitor.h"
#include <QFile>
#include <QTextStream>

SystemStats LinuxSystemMonitor::poll()
{
    return { readCpu(), readMem(), readNet() };
}

CpuStats LinuxSystemMonitor::readCpu()
{
    QFile f("/proc/stat");
    if (!f.open(QIODevice::ReadOnly)) return {};
    QTextStream in(&f);

    QVector<CoreTick> cur;
    QString line;
    while (in.readLineInto(&line)) {
        // skip aggregate "cpu " line, keep "cpu0", "cpu1", ...
        if (!line.startsWith("cpu") || line.startsWith("cpu ")) continue;

        auto p = line.split(' ', Qt::SkipEmptyParts);
        if (p.size() < 8) continue;

        quint64 user    = p[1].toULongLong();
        quint64 nice    = p[2].toULongLong();
        quint64 system  = p[3].toULongLong();
        quint64 idle    = p[4].toULongLong();
        quint64 iowait  = p[5].toULongLong();
        quint64 irq     = p[6].toULongLong();
        quint64 softirq = p[7].toULongLong();
        quint64 steal   = p.size() > 8 ? p[8].toULongLong() : 0;

        CoreTick t;
        t.active = user + nice + system + irq + softirq + steal;
        t.total  = t.active + idle + iowait;
        cur.append(t);
    }

    CpuStats stats;
    stats.corePercent.resize(cur.size(), 0.0f);

    if (m_prevCpu.size() == cur.size()) {
        for (int i = 0; i < cur.size(); ++i) {
            quint64 dActive = cur[i].active - m_prevCpu[i].active;
            quint64 dTotal  = cur[i].total  - m_prevCpu[i].total;
            stats.corePercent[i] = dTotal ? float(dActive) * 100.f / float(dTotal) : 0.f;
        }
    }
    m_prevCpu = cur;
    return stats;
}

MemStats LinuxSystemMonitor::readMem()
{
    QFile f("/proc/meminfo");
    if (!f.open(QIODevice::ReadOnly)) return {};
    QTextStream in(&f);

    quint64 total = 0, available = 0;
    QString line;
    while (in.readLineInto(&line)) {
        if      (line.startsWith("MemTotal:"))
            total     = line.split(' ', Qt::SkipEmptyParts)[1].toULongLong();
        else if (line.startsWith("MemAvailable:"))
            available = line.split(' ', Qt::SkipEmptyParts)[1].toULongLong();
        if (total && available) break;
    }

    return { float(total - available) / (1024.f * 1024.f),
             float(total)             / (1024.f * 1024.f) };
}

NetStats LinuxSystemMonitor::readNet()
{
    QFile f("/proc/net/dev");
    if (!f.open(QIODevice::ReadOnly)) return {};
    QTextStream in(&f);

    quint64 totalRecv = 0, totalSent = 0;
    QString line;
    while (in.readLineInto(&line)) {
        line = line.trimmed();
        if (line.startsWith("Inter") || line.startsWith("face") || line.startsWith("lo:"))
            continue;

        // columns: iface rx_bytes [7 more rx cols] tx_bytes ...
        auto p = line.split(' ', Qt::SkipEmptyParts);
        if (p.size() < 10) continue;
        totalRecv += p[1].toULongLong();
        totalSent += p[9].toULongLong();
    }

    NetStats s;
    s.totalRecv   = totalRecv;
    s.totalSent   = totalSent;
    s.downloadKBs = m_prevNet.recv ? float(totalRecv - m_prevNet.recv) / 1024.f : 0.f;
    s.uploadKBs   = m_prevNet.sent ? float(totalSent - m_prevNet.sent) / 1024.f : 0.f;
    m_prevNet     = { totalRecv, totalSent };
    return s;
}

#ifdef SYSMON_TEST
#include <QCoreApplication>
#include <QTimer>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    LinuxSystemMonitor mon;

    QTimer t;
    QObject::connect(&t, &QTimer::timeout, [&]() {
        auto s = mon.poll();
        qDebug() << "CPU cores:" << s.cpu.corePercent;
        qDebug() << "RAM:" << s.mem.usedGB << "/" << s.mem.totalGB << "GB";
        qDebug() << "NET up:" << s.net.uploadKBs << "down:" << s.net.downloadKBs << "KB/s";
    });
    t.start(1000);
    return app.exec();
}
#endif
