#include "MainWindow.h"

#include "ui/AdminApprovalWidget.h"
#include "ui/aiwarningwidget.h"

#include <QAbstractAnimation>
#include <QEasingCurve>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QStackedWidget>
#include <QStyle>
#include <QVBoxLayout>
#include <QWidget>


// ============================================================
// 构造函数
// ============================================================

MainWindow::MainWindow(
    const QString &account,
    QWidget *parent
)
    : QMainWindow(parent),

      // 页面容器
      stack(nullptr),

      // 页面
      dashboardWidget(nullptr),
      revenueWidget(nullptr),
      orderWidget(nullptr),
      stationWidget(nullptr),
      pileManagementWidget(nullptr),
      userManagementWidget(nullptr),
      approvalWidget(nullptr),
      aiWarningWidget(nullptr),

      // Sidebar
      sidebar(nullptr),
      sidebarToggleButton(nullptr),
      dashboardButton(nullptr),
      approvalButton(nullptr),
      stationButton(nullptr),
      pileManagementButton(nullptr),
      userManagementButton(nullptr),
      orderButton(nullptr),
      revenueButton(nullptr),
      aiWarningButton(nullptr),
      logoutButton(nullptr),

      // Header
      workTitle(nullptr),
      systemTitle(nullptr),
      accountLabel(nullptr),

      // 登录账号
      loginAccount(account),

      // Sidebar 动画
      sidebarAnimation(nullptr),
      sidebarExpanded(true)
{
    // ========================================================
    // 窗口基础设置
    // ========================================================

    resize(1280, 800);

    setMinimumSize(1050, 650);

    setWindowTitle(
        QStringLiteral("电途—VoltWay充电管理平台")
    );


    // ========================================================
    // 创建页面容器
    // ========================================================

    stack = new QStackedWidget(this);

    stack->setObjectName(
        QStringLiteral("pageStack")
    );


    // ========================================================
    // 创建业务页面
    // ========================================================

    createPages();


    // ========================================================
    // 创建中央窗口
    // ========================================================

    QWidget *centralWidget =
        new QWidget(this);

    centralWidget->setObjectName(
        QStringLiteral("centralWidget")
    );


    QVBoxLayout *mainLayout =
        new QVBoxLayout(centralWidget);

    mainLayout->setContentsMargins(
        0,
        0,
        0,
        0
    );

    mainLayout->setSpacing(0);


    // ========================================================
    // 顶部 Header
    // ========================================================

    createHeader();


    QWidget *header =
        findChild<QWidget *>(
            QStringLiteral("topHeader")
        );

    if (header != nullptr)
    {
        mainLayout->addWidget(header);
    }


    // ========================================================
    // 主体区域
    // ========================================================

    QWidget *bodyWidget =
        new QWidget(centralWidget);

    bodyWidget->setObjectName(
        QStringLiteral("bodyWidget")
    );


    QHBoxLayout *bodyLayout =
        new QHBoxLayout(bodyWidget);

    bodyLayout->setContentsMargins(
        0,
        0,
        0,
        0
    );

    bodyLayout->setSpacing(0);


    // ========================================================
    // 创建左侧导航栏
    // ========================================================

    createNavigation();


    if (sidebar != nullptr)
    {
        bodyLayout->addWidget(sidebar);
    }


    // ========================================================
    // 右侧页面区域
    // ========================================================

    bodyLayout->addWidget(
        stack,
        1
    );


    mainLayout->addWidget(
        bodyWidget,
        1
    );


    // ========================================================
    // 设置中央窗口
    // ========================================================

    setCentralWidget(centralWidget);


    // ========================================================
    // 创建统一样式
    // ========================================================

    createStyle();


    // ========================================================
    // Sidebar 动画
    // ========================================================

    if (sidebar != nullptr)
    {
        sidebarAnimation =
            new QPropertyAnimation(
                sidebar,
                "minimumWidth",
                this
            );

        sidebarAnimation->setDuration(220);

        sidebarAnimation->setEasingCurve(
            QEasingCurve::InOutCubic
        );
    }


    // ========================================================
    // 默认显示 Dashboard
    // ========================================================

    showPage(dashboardWidget);


    // ========================================================
    // 首次刷新 Dashboard 数据
    // ========================================================

    if (dashboardWidget != nullptr)
    {
        dashboardWidget->refreshData();
    }


    // ========================================================
    // 首次刷新 AI 预警数据
    // ========================================================

    if (aiWarningWidget != nullptr)
    {
        aiWarningWidget->refreshData();
    }
}


// ============================================================
// 创建顶部 Header
// ============================================================

void MainWindow::createHeader()
{
    QWidget *header =
        new QWidget(this);

    header->setObjectName(
        QStringLiteral("topHeader")
    );

    header->setFixedHeight(72);


    QHBoxLayout *layout =
        new QHBoxLayout(header);

    layout->setContentsMargins(
        28,
        0,
        28,
        0
    );

    layout->setSpacing(12);


    // ========================================================
    // Logo
    // ========================================================

    QLabel *logoLabel = new QLabel(header);

    QPixmap logo(":/resources/logo.png");

    logoLabel->setPixmap(
        logo.scaled(
            40,
            40,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        )
    );

    logoLabel->setFixedSize(40, 40);
    logoLabel->setAlignment(Qt::AlignCenter);

    // ========================================================
    // 品牌名称
    // ========================================================

    QLabel *brand =
        new QLabel(
            QStringLiteral("电途 VoltWay"),
            header
        );

    brand->setObjectName(
        QStringLiteral("brandLabel")
    );


    QLabel *subtitle =
        new QLabel(
            QStringLiteral("充电管理后台"),
            header
        );

    subtitle->setObjectName(
        QStringLiteral("brandSubtitle")
    );


    QVBoxLayout *brandLayout =
        new QVBoxLayout();

    brandLayout->setContentsMargins(
        0,
        0,
        0,
        0
    );

    brandLayout->setSpacing(0);

    brandLayout->addWidget(brand);
    brandLayout->addWidget(subtitle);


    layout->addWidget(logoLabel);
    layout->addLayout(brandLayout);


    // ========================================================
    // 中间弹性空间
    // ========================================================

    layout->addStretch();


    // ========================================================
    // Header 右侧
    // ========================================================

    QHBoxLayout *headerRightLayout =
        new QHBoxLayout();

    headerRightLayout->setSpacing(16);


    // 当前登录管理员
    accountLabel =
        new QLabel(
            QStringLiteral("欢迎 ") +
            loginAccount +
            QStringLiteral(" 管理员"),
            header
        );

    accountLabel->setObjectName(
        QStringLiteral("accountLabel")
    );


    QLabel *systemLabel =
        new QLabel(
            QStringLiteral("后台管理系统"),
            header
        );

    systemLabel->setObjectName(
        QStringLiteral("systemLabel")
    );


    headerRightLayout->addWidget(accountLabel);
    headerRightLayout->addWidget(systemLabel);

    layout->addLayout(headerRightLayout);
}


// ============================================================
// 创建左侧导航栏
// ============================================================

void MainWindow::createNavigation()
{
    sidebar =
        new QWidget(this);

    sidebar->setObjectName(
        QStringLiteral("sideBar")
    );

    sidebar->setMinimumWidth(220);
    sidebar->setMaximumWidth(220);


    QVBoxLayout *layout =
        new QVBoxLayout(sidebar);

    layout->setContentsMargins(
        12,
        14,
        12,
        18
    );

    layout->setSpacing(6);


    // ========================================================
    // 收起 / 展开按钮
    // ========================================================

    sidebarToggleButton =
        new QPushButton(
            QStringLiteral("☰  收起菜单"),
            sidebar
        );

    sidebarToggleButton->setObjectName(
        QStringLiteral("sidebarToggleButton")
    );

    sidebarToggleButton->setCursor(
        Qt::PointingHandCursor
    );

    sidebarToggleButton->setToolTip(
        QStringLiteral("收起侧边栏")
    );

    layout->addWidget(sidebarToggleButton);


    // ========================================================
    // 工作台标题
    // ========================================================

    workTitle =
        new QLabel(
            QStringLiteral("🔧"),
            sidebar
        );

    workTitle->setObjectName(
        QStringLiteral("sideTitle")
    );

    layout->addWidget(workTitle);


    // ========================================================
    // 首页
    // ========================================================

    dashboardButton =
        new QPushButton(
            QStringLiteral("📚  首页"),
            sidebar
        );

    dashboardButton->setObjectName(
        QStringLiteral("sideButton")
    );

    dashboardButton->setCursor(
        Qt::PointingHandCursor
    );

    layout->addWidget(dashboardButton);


    // ========================================================
    // 管理员审批
    // ========================================================

    approvalButton =
        new QPushButton(
            QStringLiteral("📋  管理员审批"),
            sidebar
        );

    approvalButton->setObjectName(
        QStringLiteral("sideButton")
    );

    approvalButton->setCursor(
        Qt::PointingHandCursor
    );

    layout->addWidget(approvalButton);


    // ========================================================
    // 电站管理
    // ========================================================

    stationButton =
        new QPushButton(
            QStringLiteral("🏭  电站管理"),
            sidebar
        );

    stationButton->setObjectName(
        QStringLiteral("sideButton")
    );

    stationButton->setCursor(
        Qt::PointingHandCursor
    );

    layout->addWidget(stationButton);


    // ========================================================
    // 充电桩管理
    // ========================================================

    pileManagementButton =
        new QPushButton(
            QStringLiteral("🔌  充电桩管理"),
            sidebar
        );

    pileManagementButton->setObjectName(
        QStringLiteral("sideButton")
    );

    pileManagementButton->setCursor(
        Qt::PointingHandCursor
    );

    layout->addWidget(pileManagementButton);


    // ========================================================
    // 用户管理
    // ========================================================

    userManagementButton =
        new QPushButton(
            QStringLiteral("👥  用户管理"),
            sidebar
        );

    userManagementButton->setObjectName(
        QStringLiteral("sideButton")
    );

    userManagementButton->setCursor(
        Qt::PointingHandCursor
    );

    layout->addWidget(userManagementButton);


    // ========================================================
    // 订单管理
    // ========================================================

    orderButton =
        new QPushButton(
            QStringLiteral("📝  订单管理"),
            sidebar
        );

    orderButton->setObjectName(
        QStringLiteral("sideButton")
    );

    orderButton->setCursor(
        Qt::PointingHandCursor
    );

    layout->addWidget(orderButton);


    // ========================================================
    // 收入统计
    // ========================================================

    revenueButton =
        new QPushButton(
            QStringLiteral("📊  收入统计"),
            sidebar
        );

    revenueButton->setObjectName(
        QStringLiteral("sideButton")
    );

    revenueButton->setCursor(
        Qt::PointingHandCursor
    );

    layout->addWidget(revenueButton);


    // ========================================================
    // AI 智能预警
    // ========================================================

    aiWarningButton =
        new QPushButton(
            QStringLiteral("🤖  AI预警"),
            sidebar
        );

    aiWarningButton->setObjectName(
        QStringLiteral("sideButton")
    );

    aiWarningButton->setCursor(
        Qt::PointingHandCursor
    );

    aiWarningButton->setToolTip(
        QStringLiteral("AI智能预测与预警")
    );

    layout->addWidget(aiWarningButton);


    // ========================================================
    // 底部弹性空间
    // ========================================================

    layout->addStretch();


    // ========================================================
    // 系统标题
    // ========================================================

    systemTitle =
        new QLabel(
            QStringLiteral("⚙"),
            sidebar
        );

    systemTitle->setObjectName(
        QStringLiteral("sideTitle")
    );

    layout->addWidget(systemTitle);


    // ========================================================
    // 退出登录
    // ========================================================

    logoutButton =
        new QPushButton(
            QStringLiteral("🚪  退出登录"),
            sidebar
        );

    logoutButton->setObjectName(
        QStringLiteral("sideButton")
    );

    logoutButton->setCursor(
        Qt::PointingHandCursor
    );

    layout->addWidget(logoutButton);


    // ========================================================
    // Sidebar 收起 / 展开
    // ========================================================

    connect(
        sidebarToggleButton,
        &QPushButton::clicked,
        this,
        &MainWindow::toggleSidebar
    );


    // ========================================================
    // 首页
    // ========================================================

    connect(
        dashboardButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            showPage(dashboardWidget);

            if (dashboardWidget != nullptr)
            {
                dashboardWidget->refreshData();
            }
        }
    );


    // ========================================================
    // 管理员审批
    // ========================================================

    connect(
        approvalButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            showPage(approvalWidget);
        }
    );


    // ========================================================
    // 电站管理
    // ========================================================

    connect(
        stationButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            showPage(stationWidget);
        }
    );


    // ========================================================
    // 充电桩管理
    // ========================================================

    connect(
        pileManagementButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            showPage(pileManagementWidget);
        }
    );


    // ========================================================
    // 用户管理
    // ========================================================

    connect(
        userManagementButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            showPage(userManagementWidget);
        }
    );


    // ========================================================
    // 订单管理
    // ========================================================

    connect(
        orderButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            showPage(orderWidget);
        }
    );


    // ========================================================
    // 收入统计
    // ========================================================

    connect(
        revenueButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            showPage(revenueWidget);
        }
    );


    // ========================================================
    // AI 智能预警
    // ========================================================

    connect(
        aiWarningButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            showPage(aiWarningWidget);

            if (aiWarningWidget != nullptr)
            {
                aiWarningWidget->refreshData();
            }
        }
    );


    // ========================================================
    // 退出登录
    // ========================================================

    connect(
        logoutButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            QMessageBox::StandardButton result =
                QMessageBox::question(
                    this,
                    QStringLiteral("退出登录"),
                    QStringLiteral("确定要退出登录吗？"),
                    QMessageBox::Yes |
                    QMessageBox::No,
                    QMessageBox::No
                );

            if (result == QMessageBox::Yes)
            {
                emit logoutRequested();
            }
        }
    );
}


// ============================================================
// Sidebar 收起 / 展开
// ============================================================

void MainWindow::toggleSidebar()
{
    if (sidebar == nullptr ||
        sidebarAnimation == nullptr)
    {
        return;
    }


    if (sidebarAnimation->state() ==
        QAbstractAnimation::Running)
    {
        return;
    }


    const bool expand =
        !sidebarExpanded;


    const int startWidth =
        sidebar->width();


    const int endWidth =
        expand ? 220 : 72;


    sidebarAnimation->stop();


    sidebarAnimation->setStartValue(
        startWidth
    );

    sidebarAnimation->setEndValue(
        endWidth
    );


    sidebarExpanded = expand;


    sidebar->setProperty(
        "collapsed",
        !sidebarExpanded
    );


    // 同时设置最大宽度
    sidebar->setMaximumWidth(
        endWidth
    );


    updateSidebarTexts(
        sidebarExpanded
    );


    // 重新应用 QSS
    sidebar->style()->unpolish(sidebar);
    sidebar->style()->polish(sidebar);
    sidebar->update();


    if (sidebarExpanded)
    {
        sidebarToggleButton->setText(
            QStringLiteral("☰  收起菜单")
        );

        sidebarToggleButton->setToolTip(
            QStringLiteral("收起侧边栏")
        );
    }
    else
    {
        sidebarToggleButton->setText(
            QStringLiteral("☰")
        );

        sidebarToggleButton->setToolTip(
            QStringLiteral("展开侧边栏")
        );
    }


    sidebarAnimation->start();
}


// ============================================================
// 更新 Sidebar 文字
// ============================================================

void MainWindow::updateSidebarTexts(
    bool expanded
)
{
    if (expanded)
    {
        workTitle->setText(
            QStringLiteral("🔧")
        );

        systemTitle->setText(
            QStringLiteral("⚙")
        );

        dashboardButton->setText(
            QStringLiteral("📚  首页")
        );

        approvalButton->setText(
            QStringLiteral("📋  管理员审批")
        );

        stationButton->setText(
            QStringLiteral("🏭  电站管理")
        );

        pileManagementButton->setText(
            QStringLiteral("🔌  充电桩管理")
        );

        userManagementButton->setText(
            QStringLiteral("👥  用户管理")
        );

        orderButton->setText(
            QStringLiteral("📝  订单管理")
        );

        revenueButton->setText(
            QStringLiteral("📊  收入统计")
        );

        aiWarningButton->setText(
            QStringLiteral("🤖  AI预警")
        );

        logoutButton->setText(
            QStringLiteral("🚪  退出登录")
        );
    }
    else
    {
        workTitle->setText(
            QStringLiteral("🔧")
        );

        systemTitle->setText(
            QStringLiteral("⚙")
        );

        dashboardButton->setText(
            QStringLiteral("📚")
        );

        approvalButton->setText(
            QStringLiteral("📋")
        );

        stationButton->setText(
            QStringLiteral("🏭")
        );

        pileManagementButton->setText(
            QStringLiteral("🔌")
        );

        userManagementButton->setText(
            QStringLiteral("👥")
        );

        orderButton->setText(
            QStringLiteral("📝")
        );

        revenueButton->setText(
            QStringLiteral("📊")
        );

        aiWarningButton->setText(
            QStringLiteral("🤖")
        );

        logoutButton->setText(
            QStringLiteral("🚪")
        );
    }
}


// ============================================================
// 创建所有业务页面
// ============================================================

void MainWindow::createPages()
{
    // ========================================================
    // Dashboard
    // ========================================================

    dashboardWidget =
        new DashboardWidget(
            this
        );

    stack->addWidget(
        dashboardWidget
    );


    // ========================================================
    // 管理员审批
    // ========================================================

    AdminApprovalWidget *approval =
        new AdminApprovalWidget(this);

    approvalWidget = approval;

    stack->addWidget(
        approvalWidget
    );


    // ========================================================
    // 订单管理
    // ========================================================

    orderWidget =
        new OrderWidget(this);

    stack->addWidget(
        orderWidget
    );


    // ========================================================
    // 收入统计
    // ========================================================

    revenueWidget =
        new RevenueWidget(this);

    stack->addWidget(
        revenueWidget
    );


    // ========================================================
    // 电站管理
    // ========================================================

    stationWidget =
        new StationWidget(this);

    stack->addWidget(
        stationWidget
    );


    // ========================================================
    // 充电桩管理
    // ========================================================

    pileManagementWidget =
        new PileManagementWidget(this);

    stack->addWidget(
        pileManagementWidget
    );


    // ========================================================
    // 用户管理
    // ========================================================

    userManagementWidget =
        new UserManagementWidget(this);

    stack->addWidget(
        userManagementWidget
    );


    // ========================================================
    // AI 智能预警
    // ========================================================

    aiWarningWidget =
        new AIWarningWidget(this);

    stack->addWidget(
        aiWarningWidget
    );


    // ========================================================
    // 管理员审批 → Dashboard
    // ========================================================

    connect(
        approval,
        &AdminApprovalWidget::backClicked,
        this,
        [this]()
        {
            showPage(dashboardWidget);
        }
    );


    // ========================================================
    // 订单 → Dashboard
    // ========================================================

    connect(
        orderWidget,
        &OrderWidget::backHome,
        this,
        [this]()
        {
            showPage(dashboardWidget);
        }
    );


    // ========================================================
    // 订单数据变化 → Dashboard
    // ========================================================

    connect(
        orderWidget,
        &OrderWidget::orderDataChanged,
        dashboardWidget,
        &DashboardWidget::refreshData
    );


    // ========================================================
    // 电站 → Dashboard
    // ========================================================

    connect(
        stationWidget,
        &StationWidget::backHome,
        this,
        [this]()
        {
            showPage(dashboardWidget);
        }
    );


    // ========================================================
    // 电站数据变化 → Dashboard
    // ========================================================

    connect(
        stationWidget,
        &StationWidget::stationDataChanged,
        dashboardWidget,
        &DashboardWidget::refreshData
    );


    // ========================================================
    // Dashboard → 订单
    //
    // 注意：
    // revenueClicked 不应该跳订单。
    // 收入统计应该进入 RevenueWidget。
    // ========================================================

    connect(
        dashboardWidget,
        &DashboardWidget::revenueClicked,
        this,
        [this]()
        {
            showPage(revenueWidget);
        }
    );


    // ========================================================
    // Dashboard → 电站
    // ========================================================

    connect(
        dashboardWidget,
        &DashboardWidget::stationClicked,
        this,
        [this]()
        {
            showPage(stationWidget);
        }
    );


    // ========================================================
    // Dashboard → 充电桩
    //
    // 原来的代码这里错误地跳到了 stationWidget。
    // ========================================================

    connect(
        dashboardWidget,
        &DashboardWidget::pileClicked,
        this,
        [this]()
        {
            showPage(pileManagementWidget);
        }
    );


    // ========================================================
    // Dashboard → 用户
    // ========================================================

    connect(
        dashboardWidget,
        &DashboardWidget::userClicked,
        this,
        [this]()
        {
            showPage(userManagementWidget);
        }
    );
}


// ============================================================
// 页面切换
// ============================================================

void MainWindow::showPage(
    QWidget *page
)
{
    if (page == nullptr ||
        stack == nullptr)
    {
        return;
    }


    stack->setCurrentWidget(page);


    // ========================================================
    // 根据当前页面设置 Sidebar 高亮
    // ========================================================

    if (page == dashboardWidget)
    {
        setActiveButton(dashboardButton);
    }
    else if (page == approvalWidget)
    {
        setActiveButton(approvalButton);
    }
    else if (page == stationWidget)
    {
        setActiveButton(stationButton);
    }
    else if (page == pileManagementWidget)
    {
        setActiveButton(pileManagementButton);
    }
    else if (page == userManagementWidget)
    {
        setActiveButton(userManagementButton);
    }
    else if (page == orderWidget)
    {
        setActiveButton(orderButton);
    }
    else if (page == revenueWidget)
    {
        setActiveButton(revenueButton);
    }
    else if (page == aiWarningWidget)
    {
        setActiveButton(aiWarningButton);
    }
}


// ============================================================
// 设置当前 Sidebar 按钮
// ============================================================

void MainWindow::setActiveButton(
    QPushButton *button
)
{
    QPushButton *buttons[] =
    {
        dashboardButton,
        approvalButton,
        stationButton,
        pileManagementButton,
        userManagementButton,
        orderButton,
        revenueButton,
        aiWarningButton
    };


    for (QPushButton *item : buttons)
    {
        if (item == nullptr)
        {
            continue;
        }


        item->setProperty(
            "active",
            item == button
        );


        item->style()->unpolish(item);
        item->style()->polish(item);
        item->update();
    }
}


// ============================================================
// 创建统一 UI 样式
// ============================================================

void MainWindow::createStyle()
{
    setStyleSheet(R"(
        /* =====================================================
           全局
           ===================================================== */

        QMainWindow,
        QWidget {
            font-family:
                "Microsoft YaHei",
                "PingFang SC",
                "Noto Sans CJK SC",
                "Segoe UI",
                Arial;

            color: #3f424b;
        }


        /* =====================================================
           Central Widget
           ===================================================== */

        #centralWidget {
            background: #f8fbff;
        }


        /* =====================================================
           Header
           ===================================================== */

        #topHeader {
            background: #ffffff;

            border-bottom:
                1px solid #e2eaf3;
        }


        #logoIcon {
            color: #f17796;

            font-size: 28px;

            font-weight: bold;
        }


        #brandLabel {
            color: #3f424b;

            font-size: 19px;

            font-weight: 700;
        }


        #brandSubtitle {
            color: #777d88;

            font-size: 11px;
        }


        #accountLabel {
            color: #3f424b;

            background-color: #f8fbff;

            border:
                1px solid #e2eaf3;

            border-radius: 18px;

            padding:
                7px 16px;

            font-size: 13px;

            font-weight: 600;
        }


        #systemLabel {
            color: #777d88;

            font-size: 13px;

            font-weight: 600;

            background: transparent;
        }


        /* =====================================================
           Sidebar
           ===================================================== */

        #sideBar {
            background: #ffffff;

            border-right:
                1px solid #e2eaf3;
        }


        #sidebarToggleButton {
            background: #f8fbff;

            color: #5f6570;

            border:
                1px solid #e2eaf3;

            border-radius: 8px;

            min-height: 40px;

            padding:
                0 12px;

            font-size: 13px;

            font-weight: 600;

            text-align: left;
        }


        #sidebarToggleButton:hover {
            background: #fff1f5;

            border-color: #f17796;

            color: #d94f79;
        }


        #sideTitle {
            color: #9aa1ac;

            font-size: 11px;

            font-weight: 600;

            padding:
                10px 12px 5px 12px;
        }


        QPushButton#sideButton {
            background: transparent;

            color: #5f6570;

            border: none;

            border-radius: 8px;

            text-align: left;

            padding:
                12px 14px;

            min-height: 42px;

            font-size: 13px;
        }


        QPushButton#sideButton:hover {
            background: #fff1f5;

            color: #d94f79;
        }


        QPushButton#sideButton[active="true"] {
            background: #ffe8ef;

            color: #d94f79;

            font-weight: 600;
        }


        /* =====================================================
           Sidebar 收起状态
           ===================================================== */

        #sideBar[collapsed="true"] {
            background: #ffffff;
        }


        #sideBar[collapsed="true"]
        QPushButton#sideButton {
            text-align: center;

            padding:
                12px 0;

            font-size: 18px;
        }


        #sideBar[collapsed="true"]
        #sidebarToggleButton {
            text-align: center;

            padding: 0;

            font-size: 18px;
        }


        #sideBar[collapsed="true"]
        #sideTitle {
            text-align: center;

            padding:
                10px 0 5px 0;
        }


        /* =====================================================
           Body
           ===================================================== */

        #bodyWidget {
            background: #f8fbff;
        }


        /* =====================================================
           页面容器
           ===================================================== */

        #pageStack {
            background: #f8fbff;

            border: none;
        }


        #pageStack > QWidget {
            background: #f8fbff;
        }


        /* =====================================================
           滚动区域
           ===================================================== */

        QScrollArea {
            background: transparent;

            border: none;
        }


        QScrollBar:vertical {
            background: transparent;

            width: 8px;

            margin: 4px;
        }


        QScrollBar::handle:vertical {
            background: #d8e1eb;

            border-radius: 4px;

            min-height: 30px;
        }


        QScrollBar::handle:vertical:hover {
            background: #bfcbd8;
        }


        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
            height: 0px;
        }


        /* =====================================================
           通用按钮
           ===================================================== */

        QPushButton {
            outline: none;
        }


        QPushButton:disabled {
            color: #b5bcc5;

            background: #f3f6f9;
        }
    )");
}

