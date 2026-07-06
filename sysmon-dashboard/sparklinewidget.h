#pragma once
#include <QColor>
#include <QVector>
#include <QWidget>

class SparklineWidget : public QWidget {
    Q_OBJECT
public:
    SparklineWidget(const QColor &color, int capacity = 60, QWidget *parent = nullptr);
    void push(float value);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QColor         m_color;
    QVector<float> m_buf;
    int            m_capacity;
};
