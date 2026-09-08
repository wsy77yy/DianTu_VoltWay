#ifndef REVENUEWIDGET_H
#define REVENUEWIDGET_H

#include <QWidget>
#include <QMap>
#include <QVector>


#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

#include "../manager/OrderManager.h"
#include "../manager/StationManager.h"

class QLabel;
class QComboBox;
class QPushButton;
class QScrollArea;
class QFrame;

class RevenueWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RevenueWidget(QWidget *parent = nullptr);

private slots:

    void show7Days();

    void show30Days();

    void updateDistricts();

    void applyFilter();

    void resetFilter();

private:

    void createUI();

    void createChart();

    void updateStatistics();

    void updateChart(int days);

    bool orderMatchesRegion(
        const ChargingOrder &order
    ) const;

    QString selectedCity() const;

    QString selectedDistrict() const;

    QFrame* createStatCard(
        const QString &title,
        QLabel *&valueLabel
    );

private:

    // =========================
    // 筛选
    // =========================

    QComboBox *cityCombo;

    QComboBox *districtCombo;

    QPushButton *queryButton;

    QPushButton *resetButton;

    QPushButton *sevenDaysButton;

    QPushButton *thirtyDaysButton;


    // =========================
    // 核心统计
    // =========================

    QLabel *todayRevenueLabel;

    QLabel *monthRevenueLabel;

    QLabel *totalRevenueLabel;

    QLabel *totalOrderLabel;

    QLabel *averageOrderLabel;


    // =========================
    // 区域运营数据
    // =========================

    QLabel *stationCountLabel;

    QLabel *pileCountLabel;

    QLabel *availablePileLabel;

    QLabel *faultPileLabel;


    // =========================
    // 图表
    // =========================

    QChartView *chartView;

    QLineSeries *series;


    // =========================
    // 页面滚动
    // =========================

    QScrollArea *scrollArea;
};

#endif