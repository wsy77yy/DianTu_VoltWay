#include "RevenueWidget.h"

#include "../manager/PileManager.h"

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QValueAxis>

#include <QComboBox>
#include <QDate>
#include <QDateTime>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QPainter>


// ======================================================
// 构造函数
// ======================================================

RevenueWidget::RevenueWidget(QWidget *parent)
    : QWidget(parent)
{
    cityCombo = nullptr;
    districtCombo = nullptr;

    queryButton = nullptr;
    resetButton = nullptr;

    sevenDaysButton = nullptr;
    thirtyDaysButton = nullptr;

    todayRevenueLabel = nullptr;
    monthRevenueLabel = nullptr;
    totalRevenueLabel = nullptr;
    totalOrderLabel = nullptr;
    averageOrderLabel = nullptr;

    stationCountLabel = nullptr;
    pileCountLabel = nullptr;
    availablePileLabel = nullptr;
    faultPileLabel = nullptr;

    chartView = nullptr;
    series = nullptr;

    scrollArea = nullptr;

    createUI();

    createChart();

    updateStatistics();

    updateChart(7);
}


// ======================================================
// 创建页面 UI
// ======================================================

void RevenueWidget::createUI()
{
    // ==================================================
    // 外层滚动区域
    // ==================================================

    scrollArea = new QScrollArea(this);

    scrollArea->setWidgetResizable(true);

    scrollArea->setFrameShape(QFrame::NoFrame);

    scrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );


    QWidget *content = new QWidget();

    QVBoxLayout *mainLayout =
        new QVBoxLayout(content);

    mainLayout->setContentsMargins(
        30, 25, 30, 30
    );

    mainLayout->setSpacing(20);


    // ==================================================
    // 标题
    // ==================================================

    QLabel *title =
        new QLabel("收入统计");

    title->setStyleSheet(
        "font-size:28px;"
        "font-weight:bold;"
        "color:#1f2937;"
    );

    mainLayout->addWidget(title);


    QLabel *subtitle =
        new QLabel(
            "查看平台营收、订单及区域运营情况"
        );

    subtitle->setStyleSheet(
        "font-size:14px;"
        "color:#6b7280;"
    );

    mainLayout->addWidget(subtitle);


    // ==================================================
    // 区域筛选
    // ==================================================

    QFrame *filterFrame =
        new QFrame();

    filterFrame->setStyleSheet(
        "QFrame {"
        "background:white;"
        "border:1px solid #e5e7eb;"
        "border-radius:10px;"
        "}"
    );


    QHBoxLayout *filterLayout =
        new QHBoxLayout(filterFrame);

    filterLayout->setContentsMargins(
        20, 15, 20, 15
    );

    filterLayout->setSpacing(12);


    QLabel *cityLabel =
        new QLabel("城市");

    cityLabel->setStyleSheet(
        "color:#374151;"
        "font-weight:bold;"
    );


    cityCombo =
        new QComboBox();

    cityCombo->setMinimumWidth(150);

    cityCombo->addItem("全部城市");


    QLabel *districtLabel =
        new QLabel("区县");

    districtLabel->setStyleSheet(
        "color:#374151;"
        "font-weight:bold;"
    );


    districtCombo =
        new QComboBox();

    districtCombo->setMinimumWidth(150);

    districtCombo->addItem("全部区县");


    queryButton =
        new QPushButton("查询");

    queryButton->setMinimumWidth(80);


    resetButton =
        new QPushButton("重置");

    resetButton->setMinimumWidth(80);


    filterLayout->addWidget(cityLabel);
    filterLayout->addWidget(cityCombo);

    filterLayout->addWidget(districtLabel);
    filterLayout->addWidget(districtCombo);

    filterLayout->addStretch();

    filterLayout->addWidget(queryButton);
    filterLayout->addWidget(resetButton);


    mainLayout->addWidget(filterFrame);


    // ==================================================
    // 填充城市
    // ==================================================

    QList<ManagedStation> stations =
        StationManager::instance().getStations();

    QStringList cities;

    for (const ManagedStation &station : stations)
    {
        if (!station.city.isEmpty() &&
            !cities.contains(station.city))
        {
            cities.append(station.city);
        }
    }

    cityCombo->addItems(cities);


    // ==================================================
    // 核心统计卡片
    // ==================================================

    QGridLayout *statsLayout =
        new QGridLayout();

    statsLayout->setSpacing(15);


    QLabel *dummy = nullptr;

    QFrame *card1 =
        createStatCard(
            "今日营收",
            todayRevenueLabel
        );

    QFrame *card2 =
        createStatCard(
            "本月营收",
            monthRevenueLabel
        );

    QFrame *card3 =
        createStatCard(
            "总营收",
            totalRevenueLabel
        );

    QFrame *card4 =
        createStatCard(
            "总订单数",
            totalOrderLabel
        );

    QFrame *card5 =
        createStatCard(
            "平均订单金额",
            averageOrderLabel
        );


    Q_UNUSED(dummy);


    statsLayout->addWidget(card1, 0, 0);
    statsLayout->addWidget(card2, 0, 1);
    statsLayout->addWidget(card3, 0, 2);
    statsLayout->addWidget(card4, 0, 3);
    statsLayout->addWidget(card5, 0, 4);


    mainLayout->addLayout(statsLayout);


    // ==================================================
    // 区域运营数据
    // ==================================================

    QLabel *operationTitle =
        new QLabel("区域运营数据");

    operationTitle->setStyleSheet(
        "font-size:20px;"
        "font-weight:bold;"
        "color:#1f2937;"
    );

    mainLayout->addWidget(operationTitle);


    QGridLayout *operationLayout =
        new QGridLayout();

    operationLayout->setSpacing(15);


    operationLayout->setContentsMargins(
        0, 0, 0, 0
    );


    // =========================
    // 区域电站
    // =========================

    QFrame *stationCard =
        createStatCard(
            "区域电站数",
            stationCountLabel
        );


    // =========================
    // 区域电桩
    // =========================

    QFrame *pileCard =
        createStatCard(
            "区域电桩数",
            pileCountLabel
        );


    // =========================    
    // 可用电桩
    // =========================

    QFrame *availableCard =
        createStatCard(
            "可用电桩",
            availablePileLabel
        );


    // =========================
    // 故障电桩
    // =========================

    QFrame *faultCard =
        createStatCard(
            "故障电桩",
            faultPileLabel
        );


    operationLayout->addWidget(
        stationCard,
        0,
        0
    );

    operationLayout->addWidget(
        pileCard,
        0,
        1
    );

    operationLayout->addWidget(
        availableCard,
        0,
        2
    );

    operationLayout->addWidget(
        faultCard,
        0,
        3   
    );


    mainLayout->addLayout(
        operationLayout
    );


    // ==================================================
    // 营收趋势
    // ==================================================

    QLabel *chartTitle =
        new QLabel("营收趋势");

    chartTitle->setStyleSheet(
        "font-size:20px;"
        "font-weight:bold;"
        "color:#1f2937;"
    );

    mainLayout->addWidget(chartTitle);


    // ==================================================
    // 7 / 30 天切换
    // ==================================================

    QHBoxLayout *periodLayout =
        new QHBoxLayout();

    sevenDaysButton =
        new QPushButton("近7天");

    thirtyDaysButton =
        new QPushButton("近30天");


    sevenDaysButton->setCheckable(true);
    thirtyDaysButton->setCheckable(true);

    sevenDaysButton->setChecked(true);


    periodLayout->addWidget(
        sevenDaysButton
    );

    periodLayout->addWidget(
        thirtyDaysButton
    );

    periodLayout->addStretch();


    mainLayout->addLayout(periodLayout);


    // ==================================================
    // 图表占位
    // ==================================================

    chartView =
        new QChartView();

    chartView->setMinimumHeight(380);

    chartView->setRenderHint(
        QPainter::Antialiasing
    );

    mainLayout->addWidget(chartView);


    mainLayout->addStretch();


    // ==================================================
    // 设置滚动区域
    // ==================================================

    scrollArea->setWidget(content);

    QVBoxLayout *outerLayout =
        new QVBoxLayout(this);

    outerLayout->setContentsMargins(
        0, 0, 0, 0
    );

    outerLayout->addWidget(scrollArea);


    // ==================================================
    // 信号
    // ==================================================

    connect(
        cityCombo,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &RevenueWidget::updateDistricts
    );


    connect(
        queryButton,
        &QPushButton::clicked,
        this,
        &RevenueWidget::applyFilter
    );


    connect(
        resetButton,
        &QPushButton::clicked,
        this,
        &RevenueWidget::resetFilter
    );


    connect(
        sevenDaysButton,
        &QPushButton::clicked,
        this,
        &RevenueWidget::show7Days
    );


    connect(
        thirtyDaysButton,
        &QPushButton::clicked,
        this,
        &RevenueWidget::show30Days
    );
}


// ======================================================
// 创建统计卡片
// ======================================================

QFrame* RevenueWidget::createStatCard(
    const QString &title,
    QLabel *&valueLabel
)
{
    QFrame *frame = new QFrame();

    frame->setMinimumHeight(120);

    frame->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Preferred
    );

    frame->setStyleSheet(
        "QFrame {"
        "background-color: #ffffff;"
        "border: 1px solid #e5e7eb;"
        "border-radius: 12px;"
        "}"
    );


    QVBoxLayout *layout =
        new QVBoxLayout(frame);

    layout->setContentsMargins(
        20, 16, 20, 16
    );

    layout->setSpacing(8);


    // =========================
    // 数据名称
    // =========================

    QLabel *titleLabel =
        new QLabel(title);

    titleLabel->setStyleSheet(
        "font-size: 14px;"
        "font-weight: bold;"
        "color: #6b7280;"
        "border: none;"
        "background: transparent;"
    );


    // =========================
    // 数据数值
    // =========================

    valueLabel =
        new QLabel("0");

    valueLabel->setStyleSheet(
        "font-size: 26px;"
        "font-weight: bold;"
        "color: #111827;"
        "border: none;"
        "background: transparent;"
    );


    valueLabel->setAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );


    layout->addWidget(titleLabel);

    layout->addWidget(valueLabel);

    layout->addStretch();


    return frame;
}


// ======================================================
// 创建图表
// ======================================================

void RevenueWidget::createChart()
{
    series =
        new QLineSeries();


    QChart *chart =
        new QChart();

    chart->setTitle(
        "近7天营收趋势"
    );

    chart->legend()->hide();

    chart->addSeries(series);


    chart->setAnimationOptions(
        QChart::SeriesAnimations
    );


    chartView->setChart(chart);
}


// ======================================================
// 更新区县
// ======================================================

void RevenueWidget::updateDistricts()
{
    QString city =
        cityCombo->currentText();


    districtCombo->blockSignals(true);

    districtCombo->clear();

    districtCombo->addItem("全部区县");


    if (city == "全部城市")
    {
        districtCombo->setEnabled(false);

        districtCombo->blockSignals(false);

        return;
    }


    districtCombo->setEnabled(true);


    QList<ManagedStation> stations =
        StationManager::instance().getStations();


    QStringList districts;


    for (const ManagedStation &station : stations)
    {
        if (station.city == city &&
            !station.district.isEmpty() &&
            !districts.contains(
                station.district
            ))
        {
            districts.append(
                station.district
            );
        }
    }


    districtCombo->addItems(
        districts
    );


    districtCombo->blockSignals(false);
}


// ======================================================
// 当前城市
// ======================================================

QString RevenueWidget::selectedCity() const
{
    if (!cityCombo)
        return QString();


    if (cityCombo->currentText() ==
        "全部城市")
    {
        return QString();
    }


    return cityCombo->currentText();
}


// ======================================================
// 当前区县
// ======================================================

QString RevenueWidget::selectedDistrict() const
{
    if (!districtCombo)
        return QString();


    if (districtCombo->currentText() ==
        "全部区县")
    {
        return QString();
    }


    return districtCombo->currentText();
}


// ======================================================
// 判断订单是否属于当前区域
// ======================================================

bool RevenueWidget::orderMatchesRegion(
    const ChargingOrder &order
) const
{
    QString city =
        selectedCity();

    QString district =
        selectedDistrict();


    // 全部区域
    if (city.isEmpty() &&
        district.isEmpty())
    {
        return true;
    }


    QList<ManagedStation> stations =
        StationManager::instance().getStations();


    for (const ManagedStation &station :
         stations)
    {
        if (station.code !=
            order.stationCode)
        {
            continue;
        }


        if (!city.isEmpty() &&
            station.city != city)
        {
            return false;
        }


        if (!district.isEmpty() &&
            station.district != district)
        {
            return false;
        }


        return true;
    }


    return false;
}


// ======================================================
// 更新统计数据
// ======================================================

void RevenueWidget::updateStatistics()
{
    QList<ChargingOrder> orders =
        OrderManager::instance().getOrders();


    QDate today =
        QDate::currentDate();

    QDateTime now =
        QDateTime::currentDateTime();


    double todayRevenue = 0.0;

    double monthRevenue = 0.0;

    double totalRevenue = 0.0;

    int totalOrders = 0;

    int completedOrders = 0;


    // ==================================================
    // 订单统计
    // ==================================================

    for (const ChargingOrder &order :
         orders)
    {
        if (!orderMatchesRegion(order))
        {
            continue;
        }


        // 区域内全部订单
        totalOrders++;


        // 只计算已完成订单的营收
        if (order.status != "已完成")
        {
            continue;
        }


        completedOrders++;

        totalRevenue += order.amount;


        if (order.finishTime.isValid())
        {
            QDate finishDate =
                order.finishTime.date();


            // 今日营收
            if (finishDate == today)
            {
                todayRevenue +=
                    order.amount;
            }


            // 本月营收
            if (finishDate.year() ==
                    today.year() &&
                finishDate.month() ==
                    today.month())
            {
                monthRevenue +=
                    order.amount;
            }
        }
    }


    double averageOrder = 0.0;


    if (completedOrders > 0)
    {
        averageOrder =
            totalRevenue /
            completedOrders;
    }


    todayRevenueLabel->setText(
        QString("¥ %1")
            .arg(
                todayRevenue,
                0,
                'f',
                2
            )
    );


    monthRevenueLabel->setText(
        QString("¥ %1")
            .arg(
                monthRevenue,
                0,
                'f',
                2
            )
    );


    totalRevenueLabel->setText(
        QString("¥ %1")
            .arg(
                totalRevenue,
                0,
                'f',
                2
            )
    );


    totalOrderLabel->setText(
        QString::number(
            totalOrders
        )
    );


    averageOrderLabel->setText(
        QString("¥ %1")
            .arg(
                averageOrder,
                0,
                'f',
                2
            )
    );


    // ==================================================
    // 区域电站
    // ==================================================

    QList<ManagedStation> stations =
        StationManager::instance().getStations();


    int stationCount = 0;


    QString city =
        selectedCity();

    QString district =
        selectedDistrict();


    for (const ManagedStation &station :
         stations)
    {
        if (!city.isEmpty() &&
            station.city != city)
        {
            continue;
        }


        if (!district.isEmpty() &&
            station.district != district)
        {
            continue;
        }


        stationCount++;
    }


    // ==================================================
    // 区域电桩
    // ==================================================

    QList<ChargingPile> piles =
        PileManager::instance().getPiles();


    int pileCount = 0;

    int availablePileCount = 0;

    int faultPileCount = 0;


    for (const ChargingPile &pile :
         piles)
    {
        bool matched = false;


        for (const ManagedStation &station :
             stations)
        {
            if (station.code !=
                pile.stationCode)
            {
                continue;
            }


            if (!city.isEmpty() &&
                station.city != city)
            {
                break;
            }


            if (!district.isEmpty() &&
                station.district != district)
            {
                break;
            }


            matched = true;

            break;
        }


        if (!matched)
        {
            continue;
        }


        pileCount++;


        if (pile.status == "空闲" ||
            pile.status == "可用")
        {
            availablePileCount++;
        }


        if (pile.status == "故障")
        {
            faultPileCount++;
        }
    }


    stationCountLabel->setText(
        QString::number(stationCount)
    );


    pileCountLabel->setText(
        QString::number(pileCount)
    );


    availablePileLabel->setText(
        QString::number(
            availablePileCount
        )
    );


    faultPileLabel->setText(
        QString::number(
            faultPileCount
        )
    );


    Q_UNUSED(now);
}


// ======================================================
// 更新营收折线图
// ======================================================

void RevenueWidget::updateChart(
    int days
)
{
    if (!chartView)
        return;


    QChart *chart =
        chartView->chart();

    if (!chart)
        return;


    // ==================================================
    // 1. 清除旧曲线
    // ==================================================

    chart->removeAllSeries();


    // ==================================================
    // 2. 清除旧坐标轴
    //
    // removeAllSeries() 不会删除 Axis
    // 必须单独处理
    // ==================================================

    const QList<QAbstractAxis*> oldAxes =
        chart->axes();

    for (QAbstractAxis *axis : oldAxes)
    {
        chart->removeAxis(axis);

        delete axis;
    }


    // ==================================================
    // 3. 创建新的曲线
    // ==================================================

    series =
        new QLineSeries();


    QList<ChargingOrder> orders =
        OrderManager::instance().getOrders();


    QDate today =
        QDate::currentDate();


    // ==================================================
    // 4. 每天营收
    // ==================================================

    QVector<double> dailyRevenue;

    dailyRevenue.resize(days);


    for (int i = 0; i < days; ++i)
    {
        dailyRevenue[i] = 0.0;
    }


    for (const ChargingOrder &order : orders)
    {
        // 只统计完成订单
        if (order.status != "已完成")
            continue;


        // 区域过滤
        if (!orderMatchesRegion(order))
            continue;


        // 没有完成时间
        if (!order.finishTime.isValid())
            continue;


        QDate finishDate =
            order.finishTime.date();


        // ==================================================
        // 当前日期距离今天的偏移
        // ==================================================

        int dayOffset =
            today.daysTo(finishDate);


        int index =
            dayOffset + days - 1;


        if (index >= 0 &&
            index < days)
        {
            dailyRevenue[index] +=
                order.amount;
        }
    }


    // ==================================================
    // 5. 添加数据点
    // ==================================================

    for (int i = 0; i < days; ++i)
    {
        series->append(
            i,
            dailyRevenue[i]
        );
    }


    chart->addSeries(series);


    // ==================================================
    // 6. 创建全新的 X 轴
    // ==================================================

    QCategoryAxis *axisX =
        new QCategoryAxis();


    for (int i = 0; i < days; ++i)
    {
        QDate date =
            today.addDays(
                i - days + 1
            );


        QString label;


        if (days == 7)
        {
            // 7天全部显示
            label =
                date.toString("MM-dd");
        }
        else
        {
            // 30天避免太拥挤
            if (i % 5 == 0 ||
                i == days - 1)
            {
                label =
                    date.toString("MM-dd");
            }
            else
            {
                label.clear();
            }
        }


        if (!label.isEmpty())
        {
            axisX->append(
                label,
                i
            );
        }
    }


    // ==================================================
    // 7. 创建全新的 Y 轴
    // ==================================================

    QValueAxis *axisY =
        new QValueAxis();


    double maxRevenue = 0.0;


    for (double value : dailyRevenue)
    {
        if (value > maxRevenue)
        {
            maxRevenue = value;
        }
    }


    // 没有数据时也保持一个合理坐标范围
    if (maxRevenue <= 0)
    {
        maxRevenue = 100;
    }


    axisY->setRange(
        0,
        maxRevenue * 1.2
    );


    axisY->setLabelFormat(
        "¥%.0f"
    );


    // ==================================================
    // 8. 加入坐标轴
    // ==================================================

    chart->addAxis(
        axisX,
        Qt::AlignBottom
    );


    chart->addAxis(
        axisY,
        Qt::AlignLeft
    );


    // ==================================================
    // 9. 曲线绑定新坐标轴
    // ==================================================

    series->attachAxis(axisX);

    series->attachAxis(axisY);


    // ==================================================
    // 10. 更新标题
    // ==================================================

    if (days == 7)
    {
        chart->setTitle(
            "近7天营收趋势"
        );
    }
    else
    {
        chart->setTitle(
            "近30天营收趋势"
        );
    }


    // ==================================================
    // 11. 强制刷新
    // ==================================================

    chartView->update();
}


// ======================================================
// 7天
// ======================================================

void RevenueWidget::show7Days()
{
    sevenDaysButton->setChecked(true);

    thirtyDaysButton->setChecked(false);

    updateChart(7);
}


// ======================================================
// 30天
// ======================================================

void RevenueWidget::show30Days()
{
    sevenDaysButton->setChecked(false);

    thirtyDaysButton->setChecked(true);

    updateChart(30);
}


// ======================================================
// 查询
// ======================================================

void RevenueWidget::applyFilter()
{
    updateStatistics();

    if (sevenDaysButton->isChecked())
    {
        updateChart(7);
    }
    else
    {
        updateChart(30);
    }
}


// ======================================================
// 重置
// ======================================================

void RevenueWidget::resetFilter()
{
    cityCombo->setCurrentIndex(0);

    districtCombo->clear();

    districtCombo->addItem("全部区县");

    districtCombo->setEnabled(false);


    sevenDaysButton->setChecked(true);

    thirtyDaysButton->setChecked(false);


    updateStatistics();

    updateChart(7);
}