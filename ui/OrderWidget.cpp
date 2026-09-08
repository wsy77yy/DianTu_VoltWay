#include "OrderWidget.h"

#include "../manager/OrderManager.h"
#include "../manager/StationManager.h"
#include "../manager/RevenueManager.h"

#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <algorithm>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QGroupBox>
#include <QMessageBox>
#include <QDateTime>
#include <QFont>
#include <QFrame>
#include <QAbstractItemView>
#include <QScrollArea>
#include <QEvent>
#include <QMouseEvent>
#include <QMap>
#include <QList>
#include <QPainter>


// ============================================================
// 构造函数
// ============================================================

OrderWidget::OrderWidget(QWidget *parent)
    : QWidget(parent)
{
    createUI();

    refreshTable();
}


// ============================================================
// 创建界面
// ============================================================

void OrderWidget::createUI()
{
    // ========================================================
    // 外层滚动区域
    // ========================================================

    scrollArea =
        new QScrollArea(this);

    scrollArea->setWidgetResizable(true);

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
        "background:#f5f7fb;"
    );


    QVBoxLayout *mainLayout =
        new QVBoxLayout(contentWidget);

    mainLayout->setContentsMargins(
        20,
        20,
        20,
        30
    );

    mainLayout->setSpacing(14);


    // ========================================================
    // 页面标题
    // ========================================================

    QLabel *pageTitle =
        new QLabel(
            "订单管理",
            contentWidget
        );


    QFont titleFont;
    titleFont.setPointSize(21);
    titleFont.setBold(true);

    pageTitle->setFont(
        titleFont
    );

    pageTitle->setStyleSheet(
        "color:#303846;"
    );


    mainLayout->addWidget(
        pageTitle
    );


    QLabel *subtitleLabel =
        new QLabel(
            "查看订单运营数据、收入趋势及电站收入排名",
            contentWidget
        );


    subtitleLabel->setStyleSheet(
        "color:#7b8494;"
        "font-size:13px;"
    );


    mainLayout->addWidget(
        subtitleLabel
    );


    // ========================================================
    // 统计卡片
    // ========================================================

    QGridLayout *cardLayout =
        new QGridLayout();


    cardLayout->setSpacing(12);


    QFont numberFont;
    numberFont.setPointSize(20);
    numberFont.setBold(true);


    auto createCard =
        [&](const QString &titleText,
            QLabel *&valueLabel,
            const QString &valueColor)
        -> QFrame*
    {
        QFrame *card =
            new QFrame(contentWidget);


        card->setMinimumHeight(
            92
        );


        card->setStyleSheet(
            "QFrame {"
            "background:#ffffff;"
            "border:1px solid #e2eaf3;"
            "border-radius:10px;"
            "}"
        );


        QVBoxLayout *layout =
            new QVBoxLayout(card);


        layout->setContentsMargins(
            16,
            12,
            16,
            12
        );


        layout->setSpacing(5);


        QLabel *titleLabel =
            new QLabel(
                titleText,
                card
            );


        titleLabel->setStyleSheet(
            "font-size:13px;"
            "color:#777d88;"
            "border:none;"
            "background:transparent;"
        );


        valueLabel =
            new QLabel(
                "0",
                card
            );


        valueLabel->setFont(
            numberFont
        );


        valueLabel->setStyleSheet(
            QString(
                "font-size:20px;"
                "font-weight:bold;"
                "color:%1;"
                "border:none;"
                "background:transparent;"
            )
            .arg(valueColor)
        );


        layout->addWidget(
            titleLabel
        );

        layout->addWidget(
            valueLabel
        );


        return card;
    };


    // 第一行

    cardLayout->addWidget(
        createCard(
            "订单总数",
            totalOrderLabel,
            "#2f83c5"
        ),
        0,
        0
    );


    cardLayout->addWidget(
        createCard(
            "进行中",
            chargingOrderLabel,
            "#b97a2a"
        ),
        0,
        1
    );


    cardLayout->addWidget(
        createCard(
            "已完成",
            finishedOrderLabel,
            "#2d8b68"
        ),
        0,
        2
    );


    cardLayout->addWidget(
        createCard(
            "已取消",
            cancelledOrderLabel,
            "#c84658"
        ),
        0,
        3
    );


    // 第二行

    cardLayout->addWidget(
        createCard(
            "今日收入",
            todayRevenueLabel,
            "#d94f79"
        ),
        1,
        0
    );


    cardLayout->addWidget(
        createCard(
            "本月收入",
            monthRevenueLabel,
            "#7a63d8"
        ),
        1,
        1
    );


    cardLayout->addWidget(
        createCard(
            "总收入",
            totalRevenueLabel,
            "#198754"
        ),
        1,
        2
    );


    cardLayout->addWidget(
        createCard(
            "今日订单",
            todayOrderLabel,
            "#e08a28"
        ),
        1,
        3
    );


    for (int i = 0; i < 4; ++i)
    {
        cardLayout->setColumnStretch(
            i,
            1
        );
    }


    mainLayout->addLayout(
        cardLayout
    );


    // ========================================================
    // 收入趋势
    // ========================================================

    QGroupBox *trendGroup =
        new QGroupBox(
            "收入趋势",
            contentWidget
        );


    QVBoxLayout *trendLayout =
        new QVBoxLayout(
            trendGroup
        );


    QHBoxLayout *trendHeader =
        new QHBoxLayout();


    QLabel *trendDescription =
        new QLabel(
            "按订单完成时间统计实际收入",
            trendGroup
        );


    trendDescription->setStyleSheet(
        "color:#7b8494;"
        "font-size:13px;"
    );


    revenueRangeCombo =
        new QComboBox(
            trendGroup
        );


    revenueRangeCombo->addItem(
        "最近7天",
        7
    );


    revenueRangeCombo->addItem(
        "最近30天",
        30
    );


    revenueRangeCombo->setFixedWidth(
        120
    );


    trendHeader->addWidget(
        trendDescription
    );


    trendHeader->addStretch();


    trendHeader->addWidget(
        new QLabel(
            "统计范围：",
            trendGroup
        )
    );


    trendHeader->addWidget(
        revenueRangeCombo
    );


    trendLayout->addLayout(
        trendHeader
    );


    revenueChartView =
        new QChartView(
            trendGroup
        );


    revenueChartView->setMinimumHeight(
        330
    );


    revenueChartView->setRenderHint(
        QPainter::Antialiasing
    );


    QChart *chart =
        new QChart();


    revenueSeries =
        new QLineSeries();


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


    chart->addSeries(
        revenueSeries
    );


    chart->setTitle(
        "收入趋势"
    );


    chart->setBackgroundBrush(
        Qt::white
    );


    chart->setPlotAreaBackgroundBrush(
        QColor("#f8fbff")
    );


    chart->setPlotAreaBackgroundVisible(
        true
    );


    chart->legend()->hide();


    revenueChartView->setChart(
        chart
    );


    trendLayout->addWidget(
        revenueChartView
    );


    mainLayout->addWidget(
        trendGroup
    );


    connect(
        revenueRangeCombo,
        QOverload<int>::of(
            &QComboBox::currentIndexChanged
        ),
        this,
        &OrderWidget::updateRevenueChart
    );


    // ========================================================
    // 电站月累计收入排名
    // ========================================================

    QGroupBox *rankingGroup =
        new QGroupBox(
            "电站月累计收入排名",
            contentWidget
        );


    QVBoxLayout *rankingLayout =
        new QVBoxLayout(
            rankingGroup
        );


    QLabel *rankingDescription =
        new QLabel(
            "按照本月已完成订单收入从高到低排序",
            rankingGroup
        );


    rankingDescription->setStyleSheet(
        "color:#7b8494;"
        "font-size:13px;"
    );


    rankingLayout->addWidget(
        rankingDescription
    );


    revenueRankingTable =
        new QTableWidget(
            rankingGroup
        );


    revenueRankingTable->setColumnCount(
        4
    );


    revenueRankingTable->setHorizontalHeaderLabels(
        QStringList()
            << "排名"
            << "电站"
            << "本月收入"
            << "订单数"
    );


    revenueRankingTable->verticalHeader()
        ->setVisible(false);


    revenueRankingTable->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );


    revenueRankingTable->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );


    revenueRankingTable->setSelectionMode(
        QAbstractItemView::SingleSelection
    );


    revenueRankingTable->setAlternatingRowColors(
        true
    );


    revenueRankingTable->setMinimumHeight(
        180
    );


    QHeaderView *rankingHeader =
        revenueRankingTable->horizontalHeader();


    rankingHeader->setSectionResizeMode(
        0,
        QHeaderView::Fixed
    );


    rankingHeader->setSectionResizeMode(
        1,
        QHeaderView::Stretch
    );


    rankingHeader->setSectionResizeMode(
        2,
        QHeaderView::Fixed
    );


    rankingHeader->setSectionResizeMode(
        3,
        QHeaderView::Fixed
    );


    revenueRankingTable->setColumnWidth(
        0,
        80
    );


    revenueRankingTable->setColumnWidth(
        2,
        150
    );


    revenueRankingTable->setColumnWidth(
        3,
        100
    );


    revenueRankingTable->setMouseTracking(
        true
    );


    revenueRankingTable->viewport()
        ->installEventFilter(this);


    rankingLayout->addWidget(
        revenueRankingTable
    );


    mainLayout->addWidget(
        rankingGroup
    );


    // ========================================================
    // 查询区域
    // ========================================================

    QGroupBox *searchGroup =
        new QGroupBox(
            "订单查询",
            contentWidget
        );


    QHBoxLayout *searchLayout =
        new QHBoxLayout(
            searchGroup
        );


    searchLayout->setContentsMargins(
        12,
        12,
        12,
        12
    );


    searchLayout->setSpacing(
        8
    );


    QLabel *orderIdLabel =
        new QLabel(
            "订单号：",
            searchGroup
        );


    orderIdEdit =
        new QLineEdit(
            searchGroup
        );


    orderIdEdit->setPlaceholderText(
        "请输入订单号"
    );


    orderIdEdit->setMinimumWidth(
        160
    );


    QLabel *stationLabel =
        new QLabel(
            "电站：",
            searchGroup
        );


    stationCombo =
        new QComboBox(
            searchGroup
        );


    stationCombo->setMinimumWidth(
        150
    );


    QLabel *statusLabel =
        new QLabel(
            "状态：",
            searchGroup
        );


    statusCombo =
        new QComboBox(
            searchGroup
        );


    statusCombo->addItem(
        "全部"
    );

    statusCombo->addItem(
        "进行中"
    );

    statusCombo->addItem(
        "已完成"
    );

    statusCombo->addItem(
        "已取消"
    );


    searchButton =
        new QPushButton(
            "查询",
            searchGroup
        );


    refreshButton =
        new QPushButton(
            "刷新",
            searchGroup
        );


    cancelOrderButton =
        new QPushButton(
            "取消超时订单",
            searchGroup
        );


    searchLayout->addWidget(
        orderIdLabel
    );

    searchLayout->addWidget(
        orderIdEdit
    );

    searchLayout->addWidget(
        stationLabel
    );

    searchLayout->addWidget(
        stationCombo
    );

    searchLayout->addWidget(
        statusLabel
    );

    searchLayout->addWidget(
        statusCombo
    );

    searchLayout->addWidget(
        searchButton
    );

    searchLayout->addWidget(
        refreshButton
    );

    searchLayout->addWidget(
        cancelOrderButton
    );

    searchLayout->addStretch();


    mainLayout->addWidget(
        searchGroup
    );


    // ========================================================
    // 订单表格
    // ========================================================

    table =
        new QTableWidget(
            contentWidget
        );


    // 多一个选择列

    table->setColumnCount(
        9
    );


    table->setHorizontalHeaderLabels(
        QStringList()
            << "选择"
            << "订单号"
            << "电站"
            << "电桩"
            << "金额"
            << "状态"
            << "开始时间"
            << "结束时间"
            << "持续时间"
    );


    QHeaderView *orderHeader =
        table->horizontalHeader();


    orderHeader->setSectionResizeMode(
        0,
        QHeaderView::Fixed
    );


    orderHeader->setSectionResizeMode(
        1,
        QHeaderView::Fixed
    );


    orderHeader->setSectionResizeMode(
        2,
        QHeaderView::Fixed
    );


    orderHeader->setSectionResizeMode(
        3,
        QHeaderView::Fixed
    );


    orderHeader->setSectionResizeMode(
        4,
        QHeaderView::Fixed
    );


    orderHeader->setSectionResizeMode(
        5,
        QHeaderView::Fixed
    );


    orderHeader->setSectionResizeMode(
        6,
        QHeaderView::Stretch
    );


    orderHeader->setSectionResizeMode(
        7,
        QHeaderView::Stretch
    );


    orderHeader->setSectionResizeMode(
        8,
        QHeaderView::Fixed
    );


    table->setColumnWidth(
        0,
        55
    );


    table->setColumnWidth(
        1,
        145
    );


    table->setColumnWidth(
        2,
        90
    );


    table->setColumnWidth(
        3,
        90
    );


    table->setColumnWidth(
        4,
        90
    );


    table->setColumnWidth(
        5,
        90
    );


    table->setColumnWidth(
        8,
        110
    );


    table->verticalHeader()
        ->setVisible(false);


    table->verticalHeader()
        ->setDefaultSectionSize(
            40
        );


    table->horizontalHeader()
        ->setDefaultAlignment(
            Qt::AlignCenter
        );


    table->setAlternatingRowColors(
        true
    );


    table->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );


    table->setSelectionMode(
        QAbstractItemView::SingleSelection
    );


    table->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );


    table->setMouseTracking(
        true
    );


    table->viewport()
        ->installEventFilter(this);


    mainLayout->addWidget(
        table
    );


    // ========================================================
    // 用户取消申请
    // ========================================================

    QGroupBox *cancelGroup =
        new QGroupBox(
            "用户取消申请",
            contentWidget
        );


    QVBoxLayout *cancelLayout =
        new QVBoxLayout(
            cancelGroup
        );


    cancelRequestTable =
        new QTableWidget(
            cancelGroup
        );


    cancelRequestTable->setColumnCount(
        6
    );


    cancelRequestTable->setHorizontalHeaderLabels(
        QStringList()
            << "选择"
            << "申请编号"
            << "订单号"
            << "申请原因"
            << "申请时间"
            << "状态"
    );


    QHeaderView *requestHeader =
        cancelRequestTable->horizontalHeader();


    requestHeader->setSectionResizeMode(
        0,
        QHeaderView::Fixed
    );


    requestHeader->setSectionResizeMode(
        1,
        QHeaderView::Fixed
    );


    requestHeader->setSectionResizeMode(
        2,
        QHeaderView::Fixed
    );


    requestHeader->setSectionResizeMode(
        3,
        QHeaderView::Stretch
    );


    requestHeader->setSectionResizeMode(
        4,
        QHeaderView::Fixed
    );


    requestHeader->setSectionResizeMode(
        5,
        QHeaderView::Fixed
    );


    cancelRequestTable->setColumnWidth(
        0,
        55
    );


    cancelRequestTable->setColumnWidth(
        1,
        120
    );


    cancelRequestTable->setColumnWidth(
        2,
        140
    );


    cancelRequestTable->setColumnWidth(
        4,
        180
    );


    cancelRequestTable->setColumnWidth(
        5,
        110
    );


    cancelRequestTable->verticalHeader()
        ->setVisible(false);


    cancelRequestTable->verticalHeader()
        ->setDefaultSectionSize(
            40
        );


    cancelRequestTable->horizontalHeader()
        ->setDefaultAlignment(
            Qt::AlignCenter
        );


    cancelRequestTable->setAlternatingRowColors(
        true
    );


    cancelRequestTable->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );


    cancelRequestTable->setSelectionMode(
        QAbstractItemView::SingleSelection
    );


    cancelRequestTable->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );


    cancelRequestTable->setMouseTracking(
        true
    );


    cancelRequestTable->viewport()
        ->installEventFilter(this);


    cancelLayout->addWidget(
        cancelRequestTable
    );


    // ========================================================
    // 取消申请按钮
    // ========================================================

    QHBoxLayout *cancelButtonLayout =
        new QHBoxLayout();


    approveCancelButton =
        new QPushButton(
            "同意取消",
            cancelGroup
        );


    rejectCancelButton =
        new QPushButton(
            "拒绝取消",
            cancelGroup
        );


    cancelButtonLayout->addStretch();


    cancelButtonLayout->addWidget(
        approveCancelButton
    );


    cancelButtonLayout->addWidget(
        rejectCancelButton
    );


    cancelLayout->addLayout(
        cancelButtonLayout
    );


    mainLayout->addWidget(
        cancelGroup
    );


    // ========================================================
    // 底部
    // ========================================================

    QHBoxLayout *bottomLayout =
        new QHBoxLayout();


    backButton =
        new QPushButton(
            "返回首页",
            contentWidget
        );


    bottomLayout->addStretch();


    bottomLayout->addWidget(
        backButton
    );


    mainLayout->addLayout(
        bottomLayout
    );


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


    // ========================================================
    // 信号
    // ========================================================

    connect(
        searchButton,
        &QPushButton::clicked,
        this,
        &OrderWidget::searchOrders
    );


    connect(
        refreshButton,
        &QPushButton::clicked,
        this,
        &OrderWidget::refreshTable
    );


    connect(
        cancelOrderButton,
        &QPushButton::clicked,
        this,
        &OrderWidget::adminCancelOrder
    );


    connect(
        table,
        &QTableWidget::cellDoubleClicked,
        this,
        &OrderWidget::showOrderDetail
    );


    connect(
        table,
        &QTableWidget::itemChanged,
        this,
        &OrderWidget::onOrderItemChanged
    );


    connect(
        cancelRequestTable,
        &QTableWidget::itemChanged,
        this,
        &OrderWidget::onCancelRequestItemChanged
    );


    connect(
        approveCancelButton,
        &QPushButton::clicked,
        this,
        &OrderWidget::approveCancelRequest
    );


    connect(
        rejectCancelButton,
        &QPushButton::clicked,
        this,
        &OrderWidget::rejectCancelRequest
    );


    connect(
        backButton,
        &QPushButton::clicked,
        this,
        &OrderWidget::backHome
    );
}


// ============================================================
// 刷新全部数据
// ============================================================

void OrderWidget::refreshTable()
{
    // ========================================================
    // 电站下拉框
    // ========================================================

    QString currentStation =
        stationCombo->currentText();


    stationCombo->clear();


    stationCombo->addItem(
        "全部"
    );


    QList<ManagedStation> stations =
        StationManager::instance()
            .getStations();


    for (const ManagedStation &station
         : stations)
    {
        stationCombo->addItem(
            station.code
        );
    }


    int stationIndex =
        stationCombo->findText(
            currentStation
        );


    if (stationIndex >= 0)
    {
        stationCombo->setCurrentIndex(
            stationIndex
        );
    }
    else
    {
        stationCombo->setCurrentIndex(
            0
        );
    }


    // ========================================================
    // 订单
    // ========================================================

    searchOrders();


    // ========================================================
    // 取消申请
    // ========================================================

    QList<OrderCancelRequest> requests =
        OrderManager::instance()
            .getCancelRequests();


    updatingRequestItems = true;


    cancelRequestTable->setRowCount(
        0
    );


    for (const OrderCancelRequest &request
         : requests)
    {
        int row =
            cancelRequestTable->rowCount();


        cancelRequestTable->insertRow(
            row
        );


        // 选择框

        QTableWidgetItem *checkItem =
            new QTableWidgetItem();


        checkItem->setCheckState(
            Qt::Unchecked
        );


        checkItem->setTextAlignment(
            Qt::AlignCenter
        );


        cancelRequestTable->setItem(
            row,
            0,
            checkItem
        );


        cancelRequestTable->setItem(
            row,
            1,
            new QTableWidgetItem(
                request.requestId
            )
        );


        cancelRequestTable->setItem(
            row,
            2,
            new QTableWidgetItem(
                request.orderId
            )
        );


        cancelRequestTable->setItem(
            row,
            3,
            new QTableWidgetItem(
                request.reason
            )
        );


        cancelRequestTable->setItem(
            row,
            4,
            new QTableWidgetItem(
                request.requestTime.isValid()
                ? request.requestTime.toString(
                      "yyyy-MM-dd HH:mm:ss"
                  )
                : "-"
            )
        );


        cancelRequestTable->setItem(
            row,
            5,
            new QTableWidgetItem(
                request.status
            )
        );


        for (int column = 1;
             column < 6;
             ++column)
        {
            QTableWidgetItem *item =
                cancelRequestTable->item(
                    row,
                    column
                );


            if (item)
            {
                item->setTextAlignment(
                    Qt::AlignCenter
                );
            }
        }
    }


    updatingRequestItems = false;


    updateRevenueChart();

    updateRevenueRanking();
}


// ============================================================
// 查询订单
// ============================================================

void OrderWidget::searchOrders()
{
    QString orderId =
        orderIdEdit->text()
            .trimmed();


    QString stationCode =
        stationCombo->currentText();


    QString status =
        statusCombo->currentText();


    QList<ChargingOrder> orders =
        OrderManager::instance()
            .getOrders();


    updatingOrderItems = true;


    table->setRowCount(
        0
    );


    for (const ChargingOrder &order
         : orders)
    {
        // ====================================================
        // 订单号
        // ====================================================

        if (!orderId.isEmpty()
            && !order.orderId.contains(
                orderId,
                Qt::CaseInsensitive
            ))
        {
            continue;
        }


        // ====================================================
        // 电站
        // ====================================================

        if (stationCode != "全部"
            && !stationCode.isEmpty()
            && order.stationCode != stationCode)
        {
            continue;
        }


        // ====================================================
        // 状态
        // ====================================================

        if (status != "全部"
            && order.status != status)
        {
            continue;
        }


        int row =
            table->rowCount();


        table->insertRow(
            row
        );


        // ====================================================
        // 选择框
        // ====================================================

        QTableWidgetItem *checkItem =
            new QTableWidgetItem();


        checkItem->setCheckState(
            Qt::Unchecked
        );


        checkItem->setTextAlignment(
            Qt::AlignCenter
        );


        table->setItem(
            row,
            0,
            checkItem
        );


        // ====================================================
        // 订单号
        // ====================================================

        table->setItem(
            row,
            1,
            new QTableWidgetItem(
                order.orderId
            )
        );


        // 电站

        table->setItem(
            row,
            2,
            new QTableWidgetItem(
                order.stationCode
            )
        );


        // 电桩

        table->setItem(
            row,
            3,
            new QTableWidgetItem(
                order.pileCode
            )
        );


        // 金额

        table->setItem(
            row,
            4,
            new QTableWidgetItem(
                QString("¥ %1")
                    .arg(
                        order.amount,
                        0,
                        'f',
                        2
                    )
            )
        );


        // 状态

        table->setItem(
            row,
            5,
            new QTableWidgetItem(
                order.status
            )
        );


        // 开始时间

        table->setItem(
            row,
            6,
            new QTableWidgetItem(
                order.createTime.isValid()
                ? order.createTime.toString(
                      "yyyy-MM-dd HH:mm:ss"
                  )
                : "-"
            )
        );


        // 结束时间

        table->setItem(
            row,
            7,
            new QTableWidgetItem(
                order.finishTime.isValid()
                ? order.finishTime.toString(
                      "yyyy-MM-dd HH:mm:ss"
                  )
                : "-"
            )
        );


        // 持续时间

        table->setItem(
            row,
            8,
            new QTableWidgetItem(
                formatDuration(
                    order.durationSeconds()
                )
            )
        );


        // ====================================================
        // 居中
        // ====================================================

        for (int column = 1;
             column < 9;
             ++column)
        {
            QTableWidgetItem *item =
                table->item(
                    row,
                    column
                );


            if (item)
            {
                item->setTextAlignment(
                    Qt::AlignCenter
                );
            }
        }
    }


    updatingOrderItems = false;


    updateStatistics();
}


// ============================================================
// 更新统计
// ============================================================

void OrderWidget::updateStatistics()
{
    QList<ChargingOrder> orders =
        OrderManager::instance()
            .getOrders();


    int totalOrders = 0;
    int chargingOrders = 0;
    int finishedOrders = 0;
    int cancelledOrders = 0;
    int todayOrders = 0;


    double totalRevenue = 0.0;


    QDate today =
        QDate::currentDate();


    for (const ChargingOrder &order
         : orders)
    {
        ++totalOrders;


        if (order.status == "进行中")
        {
            ++chargingOrders;
        }
        else if (order.status == "已完成")
        {
            ++finishedOrders;

            totalRevenue +=
                order.amount;
        }
        else if (order.status == "已取消")
        {
            ++cancelledOrders;
        }


        if (order.createTime.isValid()
            && order.createTime.date() == today)
        {
            ++todayOrders;
        }
    }


    totalOrderLabel->setText(
        QString::number(
            totalOrders
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


    todayOrderLabel->setText(
        QString::number(
            todayOrders
        )
    );


    double todayRevenue =
        RevenueManager::instance()
            .getTodayRevenue();


    double monthRevenue =
        RevenueManager::instance()
            .getMonthRevenue();


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
}


// ============================================================
// 更新收入趋势
// ============================================================

void OrderWidget::updateRevenueChart()
{
    if (!revenueSeries)
        return;


    revenueSeries->clear();


    QList<ChargingOrder> orders =
        OrderManager::instance()
            .getOrders();


    int days =
        revenueRangeCombo
            ->currentData()
            .toInt();


    if (days != 7 && days != 30)
        days = 7;


    QDate today =
        QDate::currentDate();


    double maxRevenue = 0.0;


    for (int i = days - 1;
         i >= 0;
         --i)
    {
        QDate date =
            today.addDays(-i);


        double revenue = 0.0;


        for (const ChargingOrder &order
             : orders)
        {
            // 只有已完成订单产生收入

            if (order.status != "已完成")
                continue;


            if (!order.finishTime.isValid())
                continue;


            if (order.finishTime.date() == date)
            {
                revenue +=
                    order.amount;
            }
        }


        int x =
            days - 1 - i;


        revenueSeries->append(
            x,
            revenue
        );


        maxRevenue =
            qMax(
                maxRevenue,
                revenue
            );
    }


    QChart *chart =
        revenueChartView->chart();


    // ========================================================
    // 删除旧坐标轴
    // ========================================================

    const auto oldAxes =
        chart->axes();


    for (QAbstractAxis *axis :
         oldAxes)
    {
        chart->removeAxis(
            axis
        );

        delete axis;
    }


    // ========================================================
    // X轴
    // ========================================================

    QValueAxis *axisX =
        new QValueAxis();


    axisX->setRange(
        0,
        days - 1
    );


    if (days == 7)
    {
        axisX->setTickCount(
            7
        );
    }
    else
    {
        axisX->setTickCount(
            7
        );
    }


    axisX->setTitleText(
        "日期"
    );


    chart->addAxis(
        axisX,
        Qt::AlignBottom
    );


    revenueSeries->attachAxis(
        axisX
    );


    // ========================================================
    // Y轴
    // ========================================================

    QValueAxis *axisY =
        new QValueAxis();


    if (maxRevenue <= 0)
        maxRevenue = 100;


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


    chart->addAxis(
        axisY,
        Qt::AlignLeft
    );


    revenueSeries->attachAxis(
        axisY
    );


    // ========================================================
    // 标题
    // ========================================================

    chart->setTitle(
        days == 7
        ? "最近7天收入趋势"
        : "最近30天收入趋势"
    );
}


// ============================================================
// 更新电站收入排名
// ============================================================

void OrderWidget::updateRevenueRanking()
{
    QList<ChargingOrder> orders =
        OrderManager::instance()
            .getOrders();


    QDate today =
        QDate::currentDate();


    QDate monthStart(
        today.year(),
        today.month(),
        1
    );


    // 电站 -> 收入
    QMap<QString, double> revenueMap;

    // 电站 -> 订单数量
    QMap<QString, int> orderMap;


    for (const ChargingOrder &order
         : orders)
    {
        if (order.status != "已完成")
            continue;


        if (!order.finishTime.isValid())
            continue;


        if (order.finishTime.date() <
            monthStart)
        {
            continue;
        }


        if (order.finishTime.date() >
            today)
        {
            continue;
        }


        revenueMap[
            order.stationCode
        ] += order.amount;


        orderMap[
            order.stationCode
        ]++;
    }


    // ========================================================
    // 排序
    // ========================================================

    struct StationRevenue
    {
        QString station;
        double revenue;
        int orders;
    };


    QList<StationRevenue> ranking;


    for (auto it = revenueMap.constBegin();
         it != revenueMap.constEnd();
         ++it)
    {
        StationRevenue item;

        item.station =
            it.key();

        item.revenue =
            it.value();

        item.orders =
            orderMap.value(
                it.key()
            );


        ranking.append(
            item
        );
    }


    std::sort(
        ranking.begin(),
        ranking.end(),
        [](const StationRevenue &a,
           const StationRevenue &b)
        {
            return a.revenue >
                   b.revenue;
        }
    );


    revenueRankingTable->setRowCount(
        0
    );


    for (int i = 0;
         i < ranking.size();
         ++i)
    {
        const StationRevenue &item =
            ranking.at(i);


        int row =
            revenueRankingTable->rowCount();


        revenueRankingTable->insertRow(
            row
        );


        revenueRankingTable->setItem(
            row,
            0,
            new QTableWidgetItem(
                QString::number(
                    i + 1
                )
            )
        );


        revenueRankingTable->setItem(
            row,
            1,
            new QTableWidgetItem(
                item.station
            )
        );


        revenueRankingTable->setItem(
            row,
            2,
            new QTableWidgetItem(
                QString("¥ %1")
                    .arg(
                        item.revenue,
                        0,
                        'f',
                        2
                    )
            )
        );


        revenueRankingTable->setItem(
            row,
            3,
            new QTableWidgetItem(
                QString::number(
                    item.orders
                )
            )
        );


        for (int column = 0;
             column < 4;
             ++column)
        {
            QTableWidgetItem *cell =
                revenueRankingTable->item(
                    row,
                    column
                );


            if (cell)
            {
                cell->setTextAlignment(
                    Qt::AlignCenter
                );
            }
        }
    }


    if (ranking.isEmpty())
    {
        revenueRankingTable->insertRow(
            0
        );


        QTableWidgetItem *item =
            new QTableWidgetItem(
                "本月暂无已完成订单"
            );


        item->setTextAlignment(
            Qt::AlignCenter
        );


        revenueRankingTable->setItem(
            0,
            0,
            item
        );


        revenueRankingTable->setSpan(
            0,
            0,
            1,
            4
        );
    }
}


// ============================================================
// 获取当前选中的订单号
// ============================================================

QString OrderWidget::selectedOrderId() const
{
    // ========================================================
    // 优先检查选择框
    // ========================================================

    for (int row = 0;
         row < table->rowCount();
         ++row)
    {
        QTableWidgetItem *checkItem =
            table->item(
                row,
                0
            );


        if (checkItem &&
            checkItem->checkState()
                == Qt::Checked)
        {
            QTableWidgetItem *orderItem =
                table->item(
                    row,
                    1
                );


            if (orderItem)
                return orderItem->text();
        }
    }


    // ========================================================
    // 再检查当前选中行
    // ========================================================

    int row =
        table->currentRow();


    if (row >= 0)
    {
        QTableWidgetItem *orderItem =
            table->item(
                row,
                1
            );


        if (orderItem)
            return orderItem->text();
    }


    return QString();
}


// ============================================================
// 获取当前选中的取消申请
// ============================================================

QString OrderWidget::selectedRequestId() const
{
    for (int row = 0;
         row < cancelRequestTable->rowCount();
         ++row)
    {
        QTableWidgetItem *checkItem =
            cancelRequestTable->item(
                row,
                0
            );


        if (checkItem &&
            checkItem->checkState()
                == Qt::Checked)
        {
            QTableWidgetItem *requestItem =
                cancelRequestTable->item(
                    row,
                    1
                );


            if (requestItem)
                return requestItem->text();
        }
    }


    int row =
        cancelRequestTable->currentRow();


    if (row >= 0)
    {
        QTableWidgetItem *requestItem =
            cancelRequestTable->item(
                row,
                1
            );


        if (requestItem)
            return requestItem->text();
    }


    return QString();
}


// ============================================================
// 双击查看订单详情
// ============================================================

void OrderWidget::showOrderDetail(
    int row,
    int column)
{
    Q_UNUSED(column);


    QTableWidgetItem *orderItem =
        table->item(
            row,
            1
        );


    if (!orderItem)
        return;


    QString orderId =
        orderItem->text();


    ChargingOrder order =
        OrderManager::instance()
            .getOrder(orderId);


    if (order.orderId.isEmpty())
    {
        QMessageBox::warning(
            this,
            "提示",
            "未找到该订单。"
        );

        return;
    }


    QString startTime =
        order.createTime.isValid()
        ? order.createTime.toString(
              "yyyy-MM-dd HH:mm:ss"
          )
        : "-";


    QString finishTime =
        order.finishTime.isValid()
        ? order.finishTime.toString(
              "yyyy-MM-dd HH:mm:ss"
          )
        : "进行中";


    QString detail =
        QString(
            "订单号：%1\n"
            "用户：%2\n"
            "电站：%3\n"
            "电桩：%4\n"
            "金额：¥ %5\n"
            "状态：%6\n"
            "开始时间：%7\n"
            "结束时间：%8\n"
            "持续时间：%9"
        )
        .arg(order.orderId)
        .arg(order.userId)
        .arg(order.stationCode)
        .arg(order.pileCode)
        .arg(
            order.amount,
            0,
            'f',
            2
        )
        .arg(order.status)
        .arg(startTime)
        .arg(finishTime)
        .arg(
            formatDuration(
                order.durationSeconds()
            )
        );


    QMessageBox::information(
        this,
        "订单详情",
        detail
    );
}


// ============================================================
// 判断选中的订单是否超过12小时
// ============================================================

bool OrderWidget::isSelectedOrderOvertime() const
{
    QString orderId =
        selectedOrderId();


    if (orderId.isEmpty())
        return false;


    return OrderManager::instance()
        .isOvertimeOrder(
            orderId
        );
}


// ============================================================
// 管理员取消超时订单
// ============================================================

void OrderWidget::adminCancelOrder()
{
    QString orderId =
        selectedOrderId();


    if (orderId.isEmpty())
    {
        QMessageBox::warning(
            this,
            "提示",
            "请先选择一个订单。"
        );

        return;
    }


    if (!OrderManager::instance()
            .isOvertimeOrder(orderId))
    {
        QMessageBox::information(
            this,
            "无法取消",
            "该订单未超过12小时，"
            "管理员只能取消超过12小时的订单。"
        );

        return;
    }


    QMessageBox::StandardButton result =
        QMessageBox::question(
            this,
            "确认取消",
            "确定要取消这个超过12小时的订单吗？\n\n"
            "取消后不会产生任何收入。",
            QMessageBox::Yes |
            QMessageBox::No,
            QMessageBox::No
        );


    if (result != QMessageBox::Yes)
        return;


    bool success =
        OrderManager::instance()
            .adminCancelOrder(
                orderId
            );


    if (!success)
    {
        QMessageBox::warning(
            this,
            "操作失败",
            "取消订单失败，请检查订单状态。"
        );

        return;
    }


    QMessageBox::information(
        this,
        "操作成功",
        "订单已取消。\n"
        "该订单不会产生任何收入。"
    );


    emit orderDataChanged();


    refreshTable();
}


// ============================================================
// 同意取消申请
// ============================================================

void OrderWidget::approveCancelRequest()
{
    QString requestId =
        selectedRequestId();


    if (requestId.isEmpty())
    {
        QMessageBox::warning(
            this,
            "提示",
            "请先选择一个取消申请。"
        );

        return;
    }


    QList<OrderCancelRequest> requests =
        OrderManager::instance()
            .getCancelRequests();


    OrderCancelRequest targetRequest;

    bool found = false;


    for (const OrderCancelRequest &request
         : requests)
    {
        if (request.requestId == requestId)
        {
            targetRequest =
                request;

            found = true;

            break;
        }
    }


    if (!found)
    {
        QMessageBox::warning(
            this,
            "提示",
            "未找到该取消申请。"
        );

        return;
    }


    if (targetRequest.status != "待处理")
    {
        QMessageBox::information(
            this,
            "提示",
            "该申请已经处理过了。"
        );

        return;
    }


    QMessageBox::StandardButton result =
        QMessageBox::question(
            this,
            "确认操作",
            QString(
                "确定同意取消订单 %1 吗？\n\n"
                "同意后该订单将被取消，"
                "不会产生收入。"
            )
            .arg(
                targetRequest.orderId
            ),
            QMessageBox::Yes |
            QMessageBox::No,
            QMessageBox::No
        );


    if (result != QMessageBox::Yes)
        return;


    bool success =
        OrderManager::instance()
            .approveCancelRequest(
                requestId
            );


    if (!success)
    {
        QMessageBox::warning(
            this,
            "操作失败",
            "同意取消申请失败。"
        );

        return;
    }


    QMessageBox::information(
        this,
        "操作成功",
        "已同意该取消申请。"
    );


    emit orderDataChanged();


    refreshTable();
}


// ============================================================
// 拒绝取消申请
// ============================================================

void OrderWidget::rejectCancelRequest()
{
    QString requestId =
        selectedRequestId();


    if (requestId.isEmpty())
    {
        QMessageBox::warning(
            this,
            "提示",
            "请先选择一个取消申请。"
        );

        return;
    }


    QList<OrderCancelRequest> requests =
        OrderManager::instance()
            .getCancelRequests();


    OrderCancelRequest targetRequest;

    bool found = false;


    for (const OrderCancelRequest &request
         : requests)
    {
        if (request.requestId == requestId)
        {
            targetRequest =
                request;

            found = true;

            break;
        }
    }


    if (!found)
    {
        QMessageBox::warning(
            this,
            "提示",
            "未找到该取消申请。"
        );

        return;
    }


    if (targetRequest.status != "待处理")
    {
        QMessageBox::information(
            this,
            "提示",
            "该申请已经处理过了。"
        );

        return;
    }


    QMessageBox::StandardButton result =
        QMessageBox::question(
            this,
            "确认操作",
            QString(
                "确定拒绝订单 %1 的取消申请吗？"
            )
            .arg(
                targetRequest.orderId
            ),
            QMessageBox::Yes |
            QMessageBox::No,
            QMessageBox::No
        );


    if (result != QMessageBox::Yes)
        return;


    bool success =
        OrderManager::instance()
            .rejectCancelRequest(
                requestId
            );


    if (!success)
    {
        QMessageBox::warning(
            this,
            "操作失败",
            "拒绝取消申请失败。"
        );

        return;
    }


    QMessageBox::information(
        this,
        "操作成功",
        "已拒绝该取消申请。"
    );


    emit orderDataChanged();


    refreshTable();
}


// ============================================================
// 选择框改变
// ============================================================

void OrderWidget::onOrderItemChanged(
    QTableWidgetItem *item)
{
    if (updatingOrderItems)
        return;


    if (!item)
        return;


    if (item->column() != 0)
        return;


    if (item->checkState()
        != Qt::Checked)
    {
        return;
    }


    int row =
        item->row();


    // 单选：取消其他行

    updatingOrderItems = true;


    for (int i = 0;
         i < table->rowCount();
         ++i)
    {
        if (i == row)
            continue;


        QTableWidgetItem *other =
            table->item(
                i,
                0
            );


        if (other)
        {
            other->setCheckState(
                Qt::Unchecked
            );
        }
    }


    updatingOrderItems = false;


    table->selectRow(
        row
    );
}


// ============================================================
// 取消申请选择框
// ============================================================

void OrderWidget::onCancelRequestItemChanged(
    QTableWidgetItem *item)
{
    if (updatingRequestItems)
        return;


    if (!item)
        return;


    if (item->column() != 0)
        return;


    if (item->checkState()
        != Qt::Checked)
    {
        return;
    }


    int row =
        item->row();


    updatingRequestItems = true;


    for (int i = 0;
         i < cancelRequestTable->rowCount();
         ++i)
    {
        if (i == row)
            continue;


        QTableWidgetItem *other =
            cancelRequestTable->item(
                i,
                0
            );


        if (other)
        {
            other->setCheckState(
                Qt::Unchecked
            );
        }
    }


    updatingRequestItems = false;


    cancelRequestTable->selectRow(
        row
    );
}


// ============================================================
// 行高亮
// ============================================================

void OrderWidget::highlightRow(
    QTableWidget *targetTable,
    int row)
{
    if (!targetTable)
        return;


    if (row < 0 ||
        row >= targetTable->rowCount())
    {
        return;
    }


    targetTable->selectRow(
        row
    );
}


// ============================================================
// 鼠标悬停整行高亮
// ============================================================

bool OrderWidget::eventFilter(
    QObject *watched,
    QEvent *event)
{
    if (event->type() !=
        QEvent::MouseMove)
    {
        return QWidget::eventFilter(
            watched,
            event
        );
    }


    QTableWidget *targetTable =
        qobject_cast<QTableWidget *>(
            watched->parent()
        );


    if (!targetTable)
    {
        return QWidget::eventFilter(
            watched,
            event
        );
    }


    QMouseEvent *mouseEvent =
        static_cast<QMouseEvent *>(
            event
        );


    QModelIndex index =
        targetTable->indexAt(
            mouseEvent->pos()
        );


    if (index.isValid())
    {
        highlightRow(
            targetTable,
            index.row()
        );
    }


    return QWidget::eventFilter(
        watched,
        event
    );
}


// ============================================================
// 时间格式化
// ============================================================

QString OrderWidget::formatDuration(
    qint64 seconds) const
{
    if (seconds < 0)
        seconds = 0;


    qint64 hours =
        seconds / 3600;


    qint64 minutes =
        (seconds % 3600) / 60;


    qint64 secs =
        seconds % 60;


    if (hours > 0)
    {
        return QString(
            "%1小时%2分%3秒"
        )
        .arg(hours)
        .arg(minutes)
        .arg(secs);
    }


    if (minutes > 0)
    {
        return QString(
            "%1分%2秒"
        )
        .arg(minutes)
        .arg(secs);
    }


    return QString(
        "%1秒"
    )
    .arg(secs);
}