#include "DashboardWidget.h"

#include "../manager/StationManager.h"
#include "../manager/PileManager.h"
#include "../manager/UserManager.h"
#include "../manager/RevenueManager.h"
#include "../manager/OrderManager.h"

#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPieSlice>

#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QDate>
#include <QFont>
#include <QPainter>
#include <QPen>
#include <QColor>
#include <QSizePolicy>


// ============================================================
// 创建统计卡片
// ============================================================

QFrame *DashboardWidget::createCard(
    const QString &title,
    QLabel *&valueLabel
)
{
    QFrame *card =
        new QFrame;

    card->setMinimumHeight(
        105
    );

    card->setStyleSheet(
        "QFrame {"
        "background: white;"
        "border: 1px solid #e6eaf0;"
        "border-radius: 12px;"
        "}"
    );

    QVBoxLayout *layout =
        new QVBoxLayout(card);

    layout->setContentsMargins(
        18,
        14,
        18,
        14
    );

    layout->setSpacing(
        8
    );


    QLabel *titleLabel =
        new QLabel(title);

    titleLabel->setStyleSheet(
        "color: #7b8494;"
        "font-size: 14px;"
        "border: none;"
        "background: transparent;"
    );


    valueLabel =
        new QLabel("0");

    valueLabel->setStyleSheet(
        "color: #202938;"
        "font-size: 25px;"
        "font-weight: 700;"
        "border: none;"
        "background: transparent;"
    );


    layout->addWidget(
        titleLabel
    );

    layout->addWidget(
        valueLabel
    );

    layout->addStretch();


    return card;
}


// ============================================================
// 构造函数
// ============================================================

DashboardWidget::DashboardWidget(
    QWidget *parent
)
    : QWidget(parent),

      revenueBtn(nullptr),
      stationBtn(nullptr),
      pileBtn(nullptr),
      userBtn(nullptr),

      todayRevenueLabel(nullptr),
      monthRevenueLabel(nullptr),
      totalRevenueLabel(nullptr),

      totalStationLabel(nullptr),
      stationCountLabel(nullptr),

      pileCountLabel(nullptr),
      onlinePileLabel(nullptr),
      faultPileLabel(nullptr),

      totalUserLabel(nullptr),
      todayNewUserLabel(nullptr),
      onlineUserLabel(nullptr),

      totalOrderLabel(nullptr),
      todayOrderLabel(nullptr),
      chargingOrderLabel(nullptr),
      finishedOrderLabel(nullptr),
      cancelledOrderLabel(nullptr),

      revenueView(nullptr),
      stationView(nullptr),
      pileView(nullptr),

      revenueSeries(nullptr),
      stationSeries(nullptr),
      pileSeries(nullptr),

      aiWarningFrame(nullptr),
      aiWarningIconLabel(nullptr),
      aiWarningTitleLabel(nullptr),
      aiWarningLevelLabel(nullptr),
      aiWarningDescriptionLabel(nullptr),
      aiWarningActionLabel(nullptr),

      scrollArea(nullptr)
{
    createUI();

    createCharts();

    refreshData();
}


// ============================================================
// 创建 UI
// ============================================================

void DashboardWidget::createUI()
{
    // ========================================================
    // 外层滚动区域
    // ========================================================

    scrollArea =
        new QScrollArea(this);

    scrollArea->setWidgetResizable(
        true
    );

    scrollArea->setFrameShape(
        QFrame::NoFrame
    );

    scrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    scrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
    );


    QWidget *contentWidget =
        new QWidget;


    contentWidget->setStyleSheet(
        "background: #f5f7fb;"
    );


    QVBoxLayout *rootLayout =
        new QVBoxLayout(
            contentWidget
        );

    rootLayout->setContentsMargins(
        24,
        22,
        24,
        30
    );

    rootLayout->setSpacing(
        18
    );


    // ========================================================
    // 页面标题
    // ========================================================

    QLabel *titleLabel =
        new QLabel(
            "运营数据总览"
        );

    titleLabel->setStyleSheet(
        "font-size: 27px;"
        "font-weight: 700;"
        "color: #202938;"
    );


    QLabel *subtitleLabel =
        new QLabel(
            "实时查看电站、电桩、用户及订单运营情况"
        );

    subtitleLabel->setStyleSheet(
        "font-size: 14px;"
        "color: #7b8494;"
    );


    rootLayout->addWidget(
        titleLabel
    );

    rootLayout->addWidget(
        subtitleLabel
    );


    // ========================================================
    // 第一行：收入
    //
    // 3 张卡片
    // ========================================================

    QHBoxLayout *revenueCardLayout =
        new QHBoxLayout;

    revenueCardLayout->setSpacing(
        14
    );

    revenueCardLayout->setContentsMargins(
        0,
        0,
        0,
        0
    );


    revenueCardLayout->addWidget(
        createCard(
            "今日收入",
            todayRevenueLabel
        ),
        1
    );

    revenueCardLayout->addWidget(
        createCard(
            "本月累计收入",
            monthRevenueLabel
        ),
        1
    );

    revenueCardLayout->addWidget(
        createCard(
            "总收入",
            totalRevenueLabel
        ),
        1
    );


    rootLayout->addLayout(
        revenueCardLayout
    );


    // ========================================================
    // 第二行：电站 / 电桩
    //
    // 5 张卡片
    // ========================================================

    QHBoxLayout *stationCardLayout =
        new QHBoxLayout;

    stationCardLayout->setSpacing(
        14
    );

    stationCardLayout->setContentsMargins(
        0,
        0,
        0,
        0
    );


    stationCardLayout->addWidget(
        createCard(
            "总电站",
            totalStationLabel
        ),
        1
    );


    stationCardLayout->addWidget(
        createCard(
            "运行电站",
            stationCountLabel
        ),
        1
    );


    stationCardLayout->addWidget(
        createCard(
            "电桩总数",
            pileCountLabel
        ),
        1
    );


    stationCardLayout->addWidget(
        createCard(
            "空闲电桩",
            onlinePileLabel
        ),
        1
    );


    stationCardLayout->addWidget(
        createCard(
            "故障电桩",
            faultPileLabel
        ),
        1
    );


    rootLayout->addLayout(
        stationCardLayout
    );


    // ========================================================
    // 第三行：用户
    //
    // 3 张卡片
    // ========================================================

    QHBoxLayout *userCardLayout =
        new QHBoxLayout;

    userCardLayout->setSpacing(
        14
    );

    userCardLayout->setContentsMargins(
        0,
        0,
        0,
        0
    );


    userCardLayout->addWidget(
        createCard(
            "用户总数",
            totalUserLabel
        ),
        1
    );


    userCardLayout->addWidget(
        createCard(
            "今日新增用户",
            todayNewUserLabel
        ),
        1
    );


    userCardLayout->addWidget(
        createCard(
            "当前在线用户",
            onlineUserLabel
        ),
        1
    );


    rootLayout->addLayout(
        userCardLayout
    );


    // ========================================================
    // 第四行：订单
    //
    // 5 张卡片
    // ========================================================

    QHBoxLayout *orderCardLayout =
        new QHBoxLayout;

    orderCardLayout->setSpacing(
        14
    );

    orderCardLayout->setContentsMargins(
        0,
        0,
        0,
        0
    );


    orderCardLayout->addWidget(
        createCard(
            "订单总数",
            totalOrderLabel
        ),
        1
    );


    orderCardLayout->addWidget(
        createCard(
            "今日订单",
            todayOrderLabel
        ),
        1
    );


    orderCardLayout->addWidget(
        createCard(
            "进行中",
            chargingOrderLabel
        ),
        1
    );


    orderCardLayout->addWidget(
        createCard(
            "已完成",
            finishedOrderLabel
        ),
        1
    );


    orderCardLayout->addWidget(
        createCard(
            "已取消",
            cancelledOrderLabel
        ),
        1
    );


    rootLayout->addLayout(
        orderCardLayout
    );


    // ========================================================
    // 图表区域
    // ========================================================

    QLabel *chartTitle =
        new QLabel(
            "运营趋势与状态"
        );

    chartTitle->setStyleSheet(
        "font-size: 20px;"
        "font-weight: 700;"
        "color: #202938;"
        "margin-top: 5px;"
    );


    rootLayout->addWidget(
        chartTitle
    );


    // ========================================================
    // 收入折线图
    // ========================================================

    revenueView =
        new QChartView;

    revenueView->setMinimumHeight(
        330
    );

    revenueView->setRenderHint(
        QPainter::Antialiasing
    );


    // ========================================================
    // 电站 / 电桩状态图
    // ========================================================

    QHBoxLayout *pieLayout =
        new QHBoxLayout;

    pieLayout->setSpacing(
        16
    );

    pieLayout->setContentsMargins(
        0,
        0,
        0,
        0
    );


    stationView =
        new QChartView;

    pileView =
        new QChartView;


    stationView->setMinimumHeight(
        360
    );

    pileView->setMinimumHeight(
        360
    );


    stationView->setRenderHint(
        QPainter::Antialiasing
    );

    pileView->setRenderHint(
        QPainter::Antialiasing
    );


    pieLayout->addWidget(
        stationView,
        1
    );

    pieLayout->addWidget(
        pileView,
        1
    );


    rootLayout->addWidget(
        revenueView
    );

    rootLayout->addLayout(
        pieLayout
    );


    // ========================================================
    // 首页 AI 智能预警卡片
    //
    // 注意：
    // 这里不再显示完整 AI 分析。
    //
    // 首页只显示：
    // 当前预警级别
    // 简短说明
    // 查看详细预警入口
    // ========================================================

    createAIWarningCard();


    rootLayout->addStretch();


    // ========================================================
    // 设置滚动区域
    // ========================================================

    scrollArea->setWidget(
        contentWidget
    );


    QVBoxLayout *pageLayout =
        new QVBoxLayout(this);

    pageLayout->setContentsMargins(
        0,
        0,
        0,
        0
    );


    pageLayout->addWidget(
        scrollArea
    );
}


// ============================================================
// 创建图表
// ============================================================

void DashboardWidget::createCharts()
{
    // ========================================================
    // 收入折线图
    // ========================================================

    revenueSeries =
        new QLineSeries;

    revenueSeries->setName(
        "收入"
    );

    revenueSeries->setPointsVisible(
        true
    );


    QPen revenuePen =
        revenueSeries->pen();

    revenuePen.setWidth(
        3
    );

    revenuePen.setColor(
        QColor("#4c8bf5")
    );

    revenueSeries->setPen(
        revenuePen
    );


    QChart *revenueChart =
        new QChart;

    revenueChart->addSeries(
        revenueSeries
    );

    revenueChart->setTitle(
        "最近7天收入趋势"
    );

    revenueChart->setBackgroundBrush(
        Qt::white
    );

    revenueChart->setPlotAreaBackgroundBrush(
        QColor("#f8fbff")
    );

    revenueChart->setPlotAreaBackgroundVisible(
        true
    );

    revenueChart->legend()->hide();


    revenueView->setChart(
        revenueChart
    );


    // ========================================================
    // 电站状态饼图
    // ========================================================

    stationSeries =
        new QPieSeries;


    QChart *stationChart =
        new QChart;

    stationChart->addSeries(
        stationSeries
    );

    stationChart->setTitle(
        "电站状态"
    );

    stationChart->setBackgroundBrush(
        Qt::white
    );

    stationChart->legend()->setVisible(
        true
    );

    stationChart->legend()->setAlignment(
        Qt::AlignBottom
    );


    stationView->setChart(
        stationChart
    );


    // ========================================================
    // 电桩状态饼图
    // ========================================================

    pileSeries =
        new QPieSeries;


    QChart *pileChart =
        new QChart;

    pileChart->addSeries(
        pileSeries
    );

    pileChart->setTitle(
        "电桩状态"
    );

    pileChart->setBackgroundBrush(
        Qt::white
    );

    pileChart->legend()->setVisible(
        true
    );

    pileChart->legend()->setAlignment(
        Qt::AlignBottom
    );


    pileView->setChart(
        pileChart
    );
}


// ============================================================
// 更新统计卡片
// ============================================================

void DashboardWidget::updateCards()
{
    // ========================================================
    // 电站
    // ========================================================

    const auto stations =
        StationManager::instance()
            .getStations();


    int totalStations =
        stations.size();

    int runningStations = 0;

    int totalPiles = 0;

    int availablePiles = 0;

    int faultPiles = 0;


    for (const auto &station : stations)
    {
        if (station.status == "启用")
        {
            ++runningStations;
        }


        totalPiles += station.total;

        availablePiles += station.available;

        faultPiles += station.fault;
    }


    // ========================================================
    // 总电站
    // ========================================================

    totalStationLabel->setText(
        QString::number(
            totalStations
        )
    );


    // ========================================================
    // 运行电站
    // ========================================================

    stationCountLabel->setText(
        QString::number(
            runningStations
        )
    );


    // ========================================================
    // 电桩总数
    // ========================================================

    pileCountLabel->setText(
        QString::number(
            totalPiles
        )
    );


    // ========================================================
    // 空闲电桩
    // ========================================================

    onlinePileLabel->setText(
        QString::number(
            availablePiles
        )
    );


    // ========================================================
    // 故障电桩
    // ========================================================

    faultPileLabel->setText(
        QString::number(
            faultPiles
        )
    );


    // ========================================================
    // 收入
    // ========================================================

    double todayRevenue =
        RevenueManager::instance()
            .getTodayRevenue();


    double monthRevenue =
        RevenueManager::instance()
            .getMonthRevenue();


    // ========================================================
    // 总收入
    //
    // 从已完成订单重新计算
    // ========================================================

    double totalRevenue =
        0.0;


    const QList<ChargingOrder> orders =
        OrderManager::instance()
            .getOrders();


    for (const ChargingOrder &order : orders)
    {
        if (order.status == "已完成")
        {
            totalRevenue += order.amount;
        }
    }


    todayRevenueLabel->setText(
        QString("¥%1")
            .arg(
                todayRevenue,
                0,
                'f',
                2
            )
    );


    monthRevenueLabel->setText(
        QString("¥%1")
            .arg(
                monthRevenue,
                0,
                'f',
                2
            )
    );


    totalRevenueLabel->setText(
        QString("¥%1")
            .arg(
                totalRevenue,
                0,
                'f',
                2
            )
    );


    // ========================================================
    // 用户
    // ========================================================

    const auto users =
        UserManager::instance()
            .getUsers();


    int totalUsers =
        users.size();


    int todayNewUsers =
        0;


    const QDate today =
        QDate::currentDate();


    for (const auto &user : users)
    {
        if (
            user.registerTime.isValid() &&
            user.registerTime.date() == today
        )
        {
            ++todayNewUsers;
        }
    }


    totalUserLabel->setText(
        QString::number(
            totalUsers
        )
    );


    todayNewUserLabel->setText(
        QString::number(
            todayNewUsers
        )
    );


    // ========================================================
    // 当前在线用户
    //
    // 当前项目暂时没有普通用户在线状态数据。
    // ========================================================

    onlineUserLabel->setText(
        "—"
    );


    onlineUserLabel->setToolTip(
        "当前项目暂未接入普通用户在线状态统计"
    );


    // ========================================================
    // 订单
    // ========================================================

    int totalOrders =
        0;

    int todayOrders =
        0;

    int chargingOrders =
        0;

    int finishedOrders =
        0;

    int cancelledOrders =
        0;


    for (const ChargingOrder &order : orders)
    {
        ++totalOrders;


        if (order.status == "进行中")
        {
            ++chargingOrders;
        }
        else if (order.status == "已完成")
        {
            ++finishedOrders;
        }
        else if (order.status == "已取消")
        {
            ++cancelledOrders;
        }


        if (
            order.createTime.isValid() &&
            order.createTime.date() == today
        )
        {
            ++todayOrders;
        }
    }


    totalOrderLabel->setText(
        QString::number(
            totalOrders
        )
    );


    todayOrderLabel->setText(
        QString::number(
            todayOrders
        )
    );


    chargingOrderLabel->setText(
        QString::number(
            chargingOrders
        )
    );


    finishedOrderLabel->setText(
        QString::number(
            finishedOrders
        )
    );


    cancelledOrderLabel->setText(
        QString::number(
            cancelledOrders
        )
    );
}


// ============================================================
// 更新收入趋势
// ============================================================

void DashboardWidget::updateRevenueChart()
{
    if (!revenueSeries ||
        !revenueView)
    {
        return;
    }


    revenueSeries->clear();


    const QList<ChargingOrder> orders =
        OrderManager::instance()
            .getOrders();


    const QDate today =
        QDate::currentDate();


    // ========================================================
    // 最近7天
    // ========================================================

    for (int i = 6; i >= 0; --i)
    {
        QDate date =
            today.addDays(-i);


        double revenue =
            0.0;


        for (const ChargingOrder &order : orders)
        {
            if (order.status != "已完成")
            {
                continue;
            }


            if (!order.finishTime.isValid())
            {
                continue;
            }


            if (order.finishTime.date() == date)
            {
                revenue += order.amount;
            }
        }


        int x =
            6 - i;


        revenueSeries->append(
            x,
            revenue
        );
    }


    QChart *chart =
        revenueView->chart();


    if (!chart)
    {
        return;
    }


    // ========================================================
    // X 轴
    // ========================================================

    QValueAxis *axisX =
        qobject_cast<QValueAxis *>(
            chart->axes(
                Qt::Horizontal
            ).value(0)
        );


    if (!axisX)
    {
        axisX =
            new QValueAxis;


        chart->addAxis(
            axisX,
            Qt::AlignBottom
        );


        revenueSeries->attachAxis(
            axisX
        );
    }


    axisX->setRange(
        0,
        6
    );


    axisX->setLabelFormat(
        "%d"
    );


    axisX->setTickCount(
        7
    );


    axisX->setTitleText(
        "日期"
    );


    // ========================================================
    // Y 轴
    // ========================================================

    QValueAxis *axisY =
        qobject_cast<QValueAxis *>(
            chart->axes(
                Qt::Vertical
            ).value(0)
        );


    if (!axisY)
    {
        axisY =
            new QValueAxis;


        chart->addAxis(
            axisY,
            Qt::AlignLeft
        );


        revenueSeries->attachAxis(
            axisY
        );
    }


    double maxRevenue =
        0.0;


    for (
        const QPointF &point :
        revenueSeries->points()
    )
    {
        maxRevenue =
            qMax(
                maxRevenue,
                point.y()
            );
    }


    if (maxRevenue <= 0)
    {
        maxRevenue =
            100;
    }


    axisY->setRange(
        0,
        maxRevenue * 1.25
    );


    axisY->setLabelFormat(
        "¥%.0f"
    );


    axisY->setTitleText(
        "收入"
    );
}


// ============================================================
// 更新电站状态
// ============================================================

void DashboardWidget::updateStationChart()
{
    if (!stationSeries)
    {
        return;
    }


    stationSeries->clear();


    const auto stations =
        StationManager::instance()
            .getStations();


    int enabled =
        0;

    int paused =
        0;

    int fault =
        0;


    for (const auto &station : stations)
    {
        if (station.status == "启用")
        {
            ++enabled;
        }
        else if (station.status == "暂停")
        {
            ++paused;
        }
        else if (station.status == "故障")
        {
            ++fault;
        }
    }


    int total =
        enabled +
        paused +
        fault;


    if (total == 0)
    {
        stationSeries->append(
            "暂无数据",
            1
        );

        return;
    }


    QPieSlice *enabledSlice =
        stationSeries->append(
            QString("启用 %1%")
                .arg(
                    enabled * 100.0 / total,
                    0,
                    'f',
                    1
                ),
            enabled
        );


    QPieSlice *pausedSlice =
        stationSeries->append(
            QString("暂停 %1%")
                .arg(
                    paused * 100.0 / total,
                    0,
                    'f',
                    1
                ),
            paused
        );


    QPieSlice *faultSlice =
        stationSeries->append(
            QString("故障 %1%")
                .arg(
                    fault * 100.0 / total,
                    0,
                    'f',
                    1
                ),
            fault
        );


    enabledSlice->setLabelVisible(
        true
    );

    pausedSlice->setLabelVisible(
        true
    );

    faultSlice->setLabelVisible(
        true
    );
}


// ============================================================
// 更新电桩状态
// ============================================================

void DashboardWidget::updatePileChart()
{
    if (!pileSeries)
    {
        return;
    }


    pileSeries->clear();


    const auto piles =
        PileManager::instance()
            .getPiles();


    int idle =
        0;

    int usingCount =
        0;

    int fault =
        0;


    for (const auto &pile : piles)
    {
        if (pile.status == "闲置")
        {
            ++idle;
        }
        else if (
            pile.status == "在用" ||
            pile.status == "使用中"
        )
        {
            ++usingCount;
        }
        else if (pile.status == "故障")
        {
            ++fault;
        }
    }


    int total =
        idle +
        usingCount +
        fault;


    if (total == 0)
    {
        pileSeries->append(
            "暂无数据",
            1
        );

        return;
    }


    QPieSlice *idleSlice =
        pileSeries->append(
            QString("闲置 %1%")
                .arg(
                    idle * 100.0 / total,
                    0,
                    'f',
                    1
                ),
            idle
        );


    QPieSlice *usingSlice =
        pileSeries->append(
            QString("使用中 %1%")
                .arg(
                    usingCount * 100.0 / total,
                    0,
                    'f',
                    1
                ),
            usingCount
        );


    QPieSlice *faultSlice =
        pileSeries->append(
            QString("故障 %1%")
                .arg(
                    fault * 100.0 / total,
                    0,
                    'f',
                    1
                ),
            fault
        );


    idleSlice->setLabelVisible(
        true
    );

    usingSlice->setLabelVisible(
        true
    );

    faultSlice->setLabelVisible(
        true
    );
}


// ============================================================
// 创建首页 AI 预警卡片
//
// 首页只显示简洁的当前预警级别。
// 点击整个卡片进入独立 AI 预警页面。
// ============================================================

void DashboardWidget::createAIWarningCard()
{
    if (!scrollArea)
    {
        return;
    }


    QWidget *contentWidget =
        scrollArea->widget();


    if (!contentWidget)
    {
        return;
    }


    QVBoxLayout *layout =
        qobject_cast<QVBoxLayout *>(
            contentWidget->layout()
        );


    if (!layout)
    {
        return;
    }


    // ========================================================
    // 使用 QPushButton 作为整个卡片
    //
    // 这样可以直接获得 clicked() 信号，
    // 不需要重写 QFrame::mousePressEvent。
    // ========================================================

    aiWarningFrame =
        new QPushButton(
            contentWidget
        );


    aiWarningFrame->setObjectName(
        "aiWarningFrame"
    );


    aiWarningFrame->setCursor(
        Qt::PointingHandCursor
    );


    aiWarningFrame->setMinimumHeight(
        150
    );


    aiWarningFrame->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Fixed
    );


    aiWarningFrame->setStyleSheet(
        "QPushButton#aiWarningFrame {"
        "background: white;"
        "border: 1px solid #e6eaf0;"
        "border-radius: 12px;"
        "text-align: left;"
        "}"
        "QPushButton#aiWarningFrame:hover {"
        "border: 1px solid #4c8bf5;"
        "background: #fbfdff;"
        "}"
        "QPushButton#aiWarningFrame:pressed {"
        "background: #f2f7ff;"
        "border: 1px solid #4c8bf5;"
        "}"
    );


    QVBoxLayout *mainLayout =
        new QVBoxLayout(
            aiWarningFrame
        );


    mainLayout->setContentsMargins(
        20,
        16,
        20,
        16
    );


    mainLayout->setSpacing(
        8
    );


    // ========================================================
    // 标题行
    // ========================================================

    QHBoxLayout *titleLayout =
        new QHBoxLayout;


    titleLayout->setContentsMargins(
        0,
        0,
        0,
        0
    );


    aiWarningTitleLabel =
        new QLabel(
            "🤖  AI智能预警"
        );


    aiWarningTitleLabel->setStyleSheet(
        "font-size: 18px;"
        "font-weight: 700;"
        "color: #202938;"
        "border: none;"
        "background: transparent;"
    );


    aiWarningActionLabel =
        new QLabel(
            "查看详细预警  →"
        );


    aiWarningActionLabel->setAlignment(
        Qt::AlignRight |
        Qt::AlignVCenter
    );


    aiWarningActionLabel->setStyleSheet(
        "font-size: 13px;"
        "color: #4c8bf5;"
        "border: none;"
        "background: transparent;"
    );


    titleLayout->addWidget(
        aiWarningTitleLabel
    );


    titleLayout->addStretch();


    titleLayout->addWidget(
        aiWarningActionLabel
    );


    mainLayout->addLayout(
        titleLayout
    );


    // ========================================================
    // 当前预警区域
    // ========================================================

    QHBoxLayout *warningLayout =
        new QHBoxLayout;


    warningLayout->setContentsMargins(
        0,
        4,
        0,
        0
    );


    warningLayout->setSpacing(
        12
    );


    // ========================================================
    // 预警图标
    // ========================================================

    aiWarningIconLabel =
        new QLabel(
            "⚠"
        );


    aiWarningIconLabel->setFixedSize(
        42,
        42
    );


    aiWarningIconLabel->setAlignment(
        Qt::AlignCenter
    );


    aiWarningIconLabel->setStyleSheet(
        "font-size: 30px;"
        "font-weight: 700;"
        "color: #f0a000;"
        "background: #fff6df;"
        "border-radius: 21px;"
    );


    // ========================================================
    // 预警文字
    // ========================================================

    QVBoxLayout *warningTextLayout =
        new QVBoxLayout;


    warningTextLayout->setContentsMargins(
        0,
        0,
        0,
        0
    );


    warningTextLayout->setSpacing(
        2
    );


    QLabel *currentLevelTitle =
        new QLabel(
            "当前预警级别"
        );


    currentLevelTitle->setStyleSheet(
        "font-size: 13px;"
        "color: #7b8494;"
        "border: none;"
        "background: transparent;"
    );


    aiWarningLevelLabel =
        new QLabel(
            "待关注"
        );


    aiWarningLevelLabel->setStyleSheet(
        "font-size: 24px;"
        "font-weight: 700;"
        "color: #f0a000;"
        "border: none;"
        "background: transparent;"
    );


    aiWarningDescriptionLabel =
        new QLabel(
            "当前有部分电站需要重点关注"
        );


    aiWarningDescriptionLabel->setStyleSheet(
        "font-size: 13px;"
        "color: #7b8494;"
        "border: none;"
        "background: transparent;"
    );


    warningTextLayout->addWidget(
        currentLevelTitle
    );


    warningTextLayout->addWidget(
        aiWarningLevelLabel
    );


    warningTextLayout->addWidget(
        aiWarningDescriptionLabel
    );


    warningLayout->addWidget(
        aiWarningIconLabel
    );


    warningLayout->addLayout(
        warningTextLayout
    );


    warningLayout->addStretch();


    mainLayout->addLayout(
        warningLayout
    );


    // ========================================================
    // 点击事件
    // ========================================================

    connect(
        aiWarningFrame,
        &QPushButton::clicked,
        this,
        [this](bool) {
            emit aiWarningClicked();
        }
    );


    // ========================================================
    // 加入 Dashboard
    // ========================================================

    layout->addWidget(
        aiWarningFrame
    );


    // ========================================================
    // 初始化状态
    // ========================================================

    updateAIWarningCard();
}


// ============================================================
// 更新首页 AI 预警卡片
//
// 当前 Linux 版本暂未接入真正 ML。
// 这里先使用统一的默认预警状态。
// 后续接入 AIManager / PyBind11 时，
// 只需要修改这里的数据来源即可。
// ============================================================

void DashboardWidget::updateAIWarningCard()
{
    if (!aiWarningIconLabel ||
        !aiWarningLevelLabel ||
        !aiWarningDescriptionLabel)
    {
        return;
    }


    // ========================================================
    // 当前默认状态
    // ========================================================

    aiWarningIconLabel->setText(
        "⚠"
    );


    aiWarningLevelLabel->setText(
        "待关注"
    );


    aiWarningDescriptionLabel->setText(
        "当前有部分电站需要重点关注"
    );


    // ========================================================
    // 图标样式
    // ========================================================

    aiWarningIconLabel->setStyleSheet(
        "font-size: 30px;"
        "font-weight: 700;"
        "color: #f0a000;"
        "background: #fff6df;"
        "border-radius: 21px;"
    );


    // ========================================================
    // 预警级别样式
    // ========================================================

    aiWarningLevelLabel->setStyleSheet(
        "font-size: 24px;"
        "font-weight: 700;"
        "color: #f0a000;"
        "border: none;"
        "background: transparent;"
    );
}


// ============================================================
// 刷新 Dashboard
// ============================================================

void DashboardWidget::refreshData()
{
    updateCards();

    updateRevenueChart();

    updateStationChart();

    updatePileChart();

    updateAIWarningCard();
}
