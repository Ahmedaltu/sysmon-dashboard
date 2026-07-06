#pragma once
#include <QColor>
#include <QFrame>
#include <QVector>

class QLabel;
class QProgressBar;
class QHBoxLayout;
class SparklineWidget;

class MetricCard : public QFrame {
    Q_OBJECT
public:
    MetricCard(const QString &title, const QColor &accent, QWidget *parent = nullptr);
    void setHeadValue(const QString &text);
    void setBars(const QVector<float> &values, float max = 100.f);
    void pushSpark(float value);

private:
    QLabel             *m_valueLabel;
    QHBoxLayout        *m_barLayout;
    QVector<QProgressBar*> m_bars;
    SparklineWidget    *m_spark;
    QString             m_barStyle;
};
