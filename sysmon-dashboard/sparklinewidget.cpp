#include "sparklinewidget.h"
#include <QPainter>
#include <QPainterPath>
#include <algorithm>

SparklineWidget::SparklineWidget(const QColor &color, int capacity, QWidget *parent)
    : QWidget(parent), m_color(color), m_capacity(capacity)
{
    setMinimumHeight(40);
    setFixedHeight(40);
}

void SparklineWidget::push(float value)
{
    if (m_buf.size() >= m_capacity)
        m_buf.removeFirst();
    m_buf.append(value);
    update();
}

void SparklineWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Background + border
    p.fillRect(rect(), QColor("#1a1a2e"));
    QColor border = m_color;
    border.setAlpha(80);
    p.setPen(QPen(border, 1));
    p.drawRect(rect().adjusted(0, 0, -1, -1));

    if (m_buf.size() < 2) return;

    float maxVal = *std::max_element(m_buf.begin(), m_buf.end());
    if (maxVal <= 0) return;

    const float w = width(), h = height();
    const float pad = 3.f;

    QVector<QPointF> pts;
    pts.reserve(m_buf.size());
    for (int i = 0; i < m_buf.size(); ++i) {
        float x = float(i) / float(m_buf.size() - 1) * w;
        float y = h - pad - (m_buf[i] / maxVal) * (h - pad * 2);
        pts.append({x, y});
    }

    // Translucent fill under the line
    QPainterPath path;
    path.moveTo(pts.first().x(), h);
    for (const auto &pt : pts) path.lineTo(pt);
    path.lineTo(pts.last().x(), h);
    path.closeSubpath();
    QColor fill = m_color;
    fill.setAlpha(35);
    p.fillPath(path, fill);

    // Line
    p.setPen(QPen(m_color, 1.5));
    p.drawPolyline(pts.constData(), pts.size());
}
