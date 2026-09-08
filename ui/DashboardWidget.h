#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include <QWidget>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPieSeries>

#include "../manager/OrderManager.h"

class QPushButton;
class QLabel;
class QScrollArea;
class QFrame;

class DashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardWidget(
        QWidget *parent = nullptr
    );

public slots:
    void refreshData();

signals:
    void revenueClicked();
    void stationClicked();
    void pileClicked();
    void userClicked();

    // 首页 AI 预警卡片点击
    void aiWarningClicked();

private:
    void createUI();
    void createCharts();

    // AI 首页简洁预警卡片
    void createAIWarningCard();
    void updateAIWarningCard();

    // 数据更新
    void updateCards();
    void updateRevenueChart();
    void updateStationChart();
    void updatePileChart();

    // 创建统计卡片
    QFrame *createCard(
        const QString &title,
        QLabel *&valueLabel
    );

private:

    // ========================================================
    // 页面按钮
    // ========================================================

    QPushButton *revenueBtn;
    QPushButton *stationBtn;
    QPushButton *pileBtn;
    QPushButton *userBtn;


    // ========================================================
    // 收入
    // ========================================================

    QLabel *todayRevenueLabel;
    QLabel *monthRevenueLabel;
    QLabel *totalRevenueLabel;


    // ========================================================
    // 电站 / 电桩
    // ========================================================

    QLabel *totalStationLabel;
    QLabel *stationCountLabel;

    QLabel *pileCountLabel;
    QLabel *onlinePileLabel;
    QLabel *faultPileLabel;


    // ========================================================
    // 用户
    // ========================================================

    QLabel *totalUserLabel;
    QLabel *todayNewUserLabel;

    QLabel *onlineUserLabel;


    // ========================================================
    // 订单
    // ========================================================

    QLabel *totalOrderLabel;
    QLabel *todayOrderLabel;
    QLabel *chargingOrderLabel;
    QLabel *finishedOrderLabel;
    QLabel *cancelledOrderLabel;


    // ========================================================
    // 图表
    // ========================================================

    QChartView *revenueView;
    QChartView *stationView;
    QChartView *pileView;

    QLineSeries *revenueSeries;

    QPieSeries *stationSeries;
    QPieSeries *pileSeries;


    // ========================================================
    // AI 首页简洁预警卡片
    // ========================================================

    QPushButton *aiWarningFrame;

    QLabel *aiWarningIconLabel;
    QLabel *aiWarningTitleLabel;
    QLabel *aiWarningLevelLabel;
    QLabel *aiWarningDescriptionLabel;
    QLabel *aiWarningActionLabel;


    // ========================================================
    // 页面滚动区域
    // ========================================================

    QScrollArea *scrollArea;
};

#endif // DASHBOARDWIDGET_H
