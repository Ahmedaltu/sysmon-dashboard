#include "metriccard.h"
#include "sparklinewidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>

MetricCard::MetricCard(const QString &title, const QColor &accent, QWidget *parent)
    : QFrame(parent)
{
    setStyleSheet(QString(
        "MetricCard { border: 2px solid %1; border-radius: 8px; background: #0d0d14; }"
    ).arg(accent.name()));

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(8);

    // Header: title (accent) + big value (white)
    auto *hdr = new QHBoxLayout;
    auto *titleLabel = new QLabel(title, this);
    titleLabel->setStyleSheet(QString(
        "color:%1; font-family:monospace; font-size:11px; font-weight:bold;"
        " border:none; background:transparent;"
    ).arg(accent.name()));

    m_valueLabel = new QLabel(QStringLiteral("—"), this);
    m_valueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_valueLabel->setStyleSheet(
        "color:#ffffff; font-family:monospace; font-size:22px; font-weight:bold;"
        " border:none; background:transparent;"
    );

    hdr->addWidget(titleLabel);
    hdr->addWidget(m_valueLabel);
    root->addLayout(hdr);

    // Bar area — vertical bars, fills available space
    auto *barWidget = new QWidget(this);
    barWidget->setStyleSheet("background:transparent;");
    m_barLayout = new QHBoxLayout(barWidget);
    m_barLayout->setContentsMargins(0, 0, 0, 0);
    m_barLayout->setSpacing(3);
    root->addWidget(barWidget, 1);

    m_spark = new SparklineWidget(accent, 60, this);
    root->addWidget(m_spark);

    m_barStyle = QString(
        "QProgressBar{background:#1a1a2e;border:1px solid #2a2a3e;border-radius:3px;}"
        "QProgressBar::chunk{background:%1;border-radius:2px;}"
    ).arg(accent.name());
}

void MetricCard::setHeadValue(const QString &text)
{
    m_valueLabel->setText(text);
}

void MetricCard::setBars(const QVector<float> &values, float max)
{
    if (m_bars.size() != values.size()) {
        qDeleteAll(m_bars);
        m_bars.clear();
        while (m_barLayout->count())
            delete m_barLayout->takeAt(0);

        for (int i = 0; i < values.size(); ++i) {
            auto *bar = new QProgressBar;
            bar->setOrientation(Qt::Vertical);
            bar->setRange(0, 1000); // 1000 steps for smooth movement
            bar->setTextVisible(false);
            bar->setStyleSheet(m_barStyle);
            m_barLayout->addWidget(bar);
            m_bars.append(bar);
        }
    }

    const float scale = max > 0 ? max : 1.f;
    for (int i = 0; i < m_bars.size(); ++i)
        m_bars[i]->setValue(int(values[i] / scale * 1000));
}

void MetricCard::pushSpark(float value)
{
    m_spark->push(value);
}
