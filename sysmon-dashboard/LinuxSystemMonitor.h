#pragma once

#include <QVector>

struct CpuStats {
    QVector<float> corePercent; // per-core, 0-100
};

struct MemStats {
    float usedGB  = 0;
    float totalGB = 0;
};

struct NetStats {
    float   uploadKBs   = 0;
    float   downloadKBs = 0;
    quint64 totalSent   = 0;
    quint64 totalRecv   = 0;
};

struct SystemStats {
    CpuStats cpu;
    MemStats mem;
    NetStats net;
};

class LinuxSystemMonitor {
public:
    LinuxSystemMonitor() = default;
    SystemStats poll(); // call once per second; first call returns 0 for deltas

private:
    struct CoreTick { quint64 active = 0, total = 0; };
    struct NetTick  { quint64 recv   = 0, sent  = 0; };

    QVector<CoreTick> m_prevCpu;
    NetTick           m_prevNet;

    CpuStats readCpu();
    MemStats readMem();
    NetStats readNet();
};
