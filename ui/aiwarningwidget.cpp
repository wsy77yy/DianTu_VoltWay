#include "aiwarningwidget.h"

#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>

#include <algorithm>


/*
 * =========================================================
 * 构造函数
 * =========================================================
 */
AIWarningWidget::AIWarningWidget(QWidget *parent)
    : QWidget(parent),
      scrollArea(nullptr),
      contentWidget(nullptr),
      mainLayout(nullptr),

      titleLabel(nullptr),
      subtitleLabel(nullptr),

      currentWarningFrame(nullptr),
      currentWarningIcon(nullptr),
      currentWarningTitle(nullptr),
      currentWarningLevel(nullptr),
      currentWarningDescription(nullptr),

      modelStatusLabel(nullptr),
      lastUpdateLabel(nullptr),
      modelStatusFrame(nullptr),

      warningStatisticsFrame(nullptr),
      criticalCountLabel(nullptr),
      dangerousCountLabel(nullptr),
      attentionCountLabel(nullptr),
      normalCountLabel(nullptr),

      predictionFrame(nullptr),
      peakTimeLabel(nullptr),
      peakOccupancyLabel(nullptr),
      peakVolumeLabel(nullptr),
      idlePilesLabel(nullptr),

      stationWarningFrame(nullptr),
      stationWarningLayout(nullptr),

      warningDetailFrame(nullptr),
      warningDetailLayout(nullptr)
{
    /*
     * =====================================================
     * 临时模拟 AI 数据
     *
     * 后续接入 AIManager / PyBind11 后，
     * 这里的数据会由真实 AI 模型提供。
     * =====================================================
     */

    currentResult.level = AIWarningLevel::Attention;

    currentResult.modelRunning = true;

    currentResult.lastUpdateTime = "2026-09-08 09:20";

    currentResult.warningCount = 6;

    currentResult.criticalCount = 1;
    currentResult.dangerousCount = 2;
    currentResult.attentionCount = 3;
    currentResult.normalCount = 26;

    currentResult.peakTime = "18:00 - 20:00";

    currentResult.peakOccupancy = 91.5;

    currentResult.peakOrderVolume = 186;

    currentResult.predictedIdlePiles = 12;

    currentResult.mainMessage =
        "AI模型预测今日晚高峰期间充电需求明显增加，"
        "部分区域可能出现充电桩资源紧张情况，"
        "建议提前做好充电资源调度。";


    /*
     * =====================================================
     * 模拟电站预警数据
     *
     * 注意：
     * region 已经成为独立字段。
     * 后续所有区域筛选都直接使用 warning.region。
     * =====================================================
     */

    AIStationWarning warning;


    // -----------------------------------------------------
    // 1. 浦东新区中心电站
    // -----------------------------------------------------
    warning.stationName = "浦东新区中心电站";
    warning.region = "华东地区";
    warning.level = AIWarningLevel::Critical;

    warning.description =
        "预计晚高峰期间充电需求极高，"
        "充电桩占用率可能超过 95%。";

    warning.suggestion =
        "建议提前调度周边空闲充电桩，"
        "并加强现场运行监控。";

    warning.predictedOccupancy = 96.8;

    currentResult.stationWarnings.append(warning);


    // -----------------------------------------------------
    // 2. 徐汇智慧电站
    // -----------------------------------------------------
    warning.stationName = "徐汇智慧电站";
    warning.region = "华东地区";
    warning.level = AIWarningLevel::Dangerous;

    warning.description =
        "预测晚高峰充电需求快速上升，"
        "充电桩利用率较高。";

    warning.suggestion =
        "建议提前做好充电资源调度，"
        "避免出现排队现象。";

    warning.predictedOccupancy = 91.2;

    currentResult.stationWarnings.append(warning);


    // -----------------------------------------------------
    // 3. 静安新能源汽车服务站
    // -----------------------------------------------------
    warning.stationName = "静安新能源汽车服务站";
    warning.region = "华东地区";
    warning.level = AIWarningLevel::Dangerous;

    warning.description =
        "近期充电订单量持续增长，"
        "晚高峰期间可能出现充电资源不足。";

    warning.suggestion =
        "建议关注实时订单变化，"
        "必要时启动周边电站分流。";

    warning.predictedOccupancy = 88.6;

    currentResult.stationWarnings.append(warning);


    // -----------------------------------------------------
    // 4. 杭州西湖电站
    // -----------------------------------------------------
    warning.stationName = "杭州西湖电站";
    warning.region = "华东地区";
    warning.level = AIWarningLevel::Attention;

    warning.description =
        "预计晚高峰期间充电桩利用率有所上升。";

    warning.suggestion =
        "建议持续观察订单量变化。";

    warning.predictedOccupancy = 78.5;

    currentResult.stationWarnings.append(warning);


    // -----------------------------------------------------
    // 5. 南京新街口电站
    // -----------------------------------------------------
    warning.stationName = "南京新街口电站";
    warning.region = "华东地区";
    warning.level = AIWarningLevel::Attention;

    warning.description =
        "预计高峰期间充电需求增加，"
        "部分时段可能出现资源紧张。";

    warning.suggestion =
        "建议关注高峰时段运行情况。";

    warning.predictedOccupancy = 72.4;

    currentResult.stationWarnings.append(warning);


    // -----------------------------------------------------
    // 6. 苏州工业园区电站
    // -----------------------------------------------------
    warning.stationName = "苏州工业园区电站";
    warning.region = "华东地区";
    warning.level = AIWarningLevel::Attention;

    warning.description =
        "当前整体运行正常，"
        "预计晚高峰利用率有所提升。";

    warning.suggestion =
        "建议保持正常监控。";

    warning.predictedOccupancy = 69.7;

    currentResult.stationWarnings.append(warning);


    /*
     * =====================================================
     * 创建界面
     * =====================================================
     */

    createUI();

    /*
     * 初始化显示
     */
    setWarningResult(currentResult);
}


/*
 * =========================================================
 * 析构函数
 * =========================================================
 */
AIWarningWidget::~AIWarningWidget()
{
}


/*
 * =========================================================
 * 创建整个页面
 * =========================================================
 */
void AIWarningWidget::createUI()
{
    QVBoxLayout *rootLayout = new QVBoxLayout(this);

    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);


    /*
     * =====================================================
     * ScrollArea
     * =====================================================
     */

    scrollArea = new QScrollArea(this);

    scrollArea->setWidgetResizable(true);

    scrollArea->setFrameShape(QFrame::NoFrame);

    scrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );


    /*
     * =====================================================
     * 内容区域
     * =====================================================
     */

    contentWidget = new QWidget;

    mainLayout = new QVBoxLayout(contentWidget);

    mainLayout->setContentsMargins(
        28,
        24,
        28,
        30
    );

    mainLayout->setSpacing(18);


    /*
     * =====================================================
     * 创建各个区域
     * =====================================================
     */

    createHeader();

    createCurrentWarningCard();

    createModelStatusCard();

    createWarningStatistics();

    createPredictionCards();

    createStationWarningSection();

    createWarningDetailSection();


    /*
     * 底部留白
     */

    mainLayout->addStretch();


    scrollArea->setWidget(contentWidget);

    rootLayout->addWidget(scrollArea);
}


/*
 * =========================================================
 * 页面标题
 * =========================================================
 */
void AIWarningWidget::createHeader()
{
    titleLabel = new QLabel(
        "🤖  AI智能预警",
        contentWidget
    );

    titleLabel->setStyleSheet(
        "font-size: 26px;"
        "font-weight: 700;"
        "color: #1f2937;"
    );


    subtitleLabel = new QLabel(
        "基于人工智能模型预测充电需求，并提前发现潜在运行风险",
        contentWidget
    );

    subtitleLabel->setStyleSheet(
        "font-size: 14px;"
        "color: #6b7280;"
    );


    mainLayout->addWidget(titleLabel);

    mainLayout->addWidget(subtitleLabel);
}


/*
 * =========================================================
 * 当前总体预警卡片
 * =========================================================
 */
void AIWarningWidget::createCurrentWarningCard()
{
    currentWarningFrame = new QFrame(contentWidget);

    currentWarningFrame->setObjectName(
        "currentWarningFrame"
    );

    currentWarningFrame->setFrameShape(
        QFrame::StyledPanel
    );


    QHBoxLayout *layout =
        new QHBoxLayout(currentWarningFrame);

    layout->setContentsMargins(
        24,
        20,
        24,
        20
    );

    layout->setSpacing(18);


    currentWarningIcon = new QLabel(
        currentWarningFrame
    );

    currentWarningIcon->setFixedSize(
        56,
        56
    );

    currentWarningIcon->setAlignment(
        Qt::AlignCenter
    );

    currentWarningIcon->setStyleSheet(
        "font-size: 30px;"
    );


    QVBoxLayout *textLayout =
        new QVBoxLayout;

    textLayout->setSpacing(4);


    currentWarningTitle = new QLabel(
        "当前系统总体预警",
        currentWarningFrame
    );

    currentWarningTitle->setStyleSheet(
        "font-size: 13px;"
        "color: #6b7280;"
    );


    currentWarningLevel = new QLabel(
        currentWarningFrame
    );

    currentWarningLevel->setStyleSheet(
        "font-size: 24px;"
        "font-weight: 700;"
    );


    currentWarningDescription = new QLabel(
        currentWarningFrame
    );

    currentWarningDescription->setWordWrap(true);

    currentWarningDescription->setStyleSheet(
        "font-size: 13px;"
        "color: #6b7280;"
    );


    textLayout->addWidget(
        currentWarningTitle
    );

    textLayout->addWidget(
        currentWarningLevel
    );

    textLayout->addWidget(
        currentWarningDescription
    );


    layout->addWidget(
        currentWarningIcon
    );

    layout->addLayout(
        textLayout
    );

    layout->addStretch();


    mainLayout->addWidget(
        currentWarningFrame
    );
}


/*
 * =========================================================
 * AI模型状态
 * =========================================================
 */
void AIWarningWidget::createModelStatusCard()
{
    modelStatusFrame = new QFrame(
        contentWidget
    );

    modelStatusFrame->setFrameShape(
        QFrame::StyledPanel
    );


    QHBoxLayout *layout =
        new QHBoxLayout(modelStatusFrame);

    layout->setContentsMargins(
        20,
        16,
        20,
        16
    );


    modelStatusLabel =
        new QLabel(modelStatusFrame);

    modelStatusLabel->setStyleSheet(
        "font-size: 14px;"
        "font-weight: 600;"
    );


    lastUpdateLabel =
        new QLabel(modelStatusFrame);

    lastUpdateLabel->setStyleSheet(
        "font-size: 13px;"
        "color: #6b7280;"
    );


    layout->addWidget(
        modelStatusLabel
    );

    layout->addStretch();

    layout->addWidget(
        lastUpdateLabel
    );


    mainLayout->addWidget(
        modelStatusFrame
    );
}


/*
 * =========================================================
 * 预警统计
 * =========================================================
 */
void AIWarningWidget::createWarningStatistics()
{
    warningStatisticsFrame =
        new QFrame(contentWidget);

    QGridLayout *layout =
        new QGridLayout(
            warningStatisticsFrame
        );

    layout->setContentsMargins(
        0,
        0,
        0,
        0
    );

    layout->setSpacing(14);


    auto createStatCard =
        [&](const QString &title,
            QLabel **valueLabel)
        {
            QFrame *frame =
                new QFrame(
                    warningStatisticsFrame
                );

            frame->setFrameShape(
                QFrame::StyledPanel
            );


            QVBoxLayout *cardLayout =
                new QVBoxLayout(frame);

            cardLayout->setContentsMargins(
                18,
                16,
                18,
                16
            );


            QLabel *titleLabel =
                new QLabel(
                    title,
                    frame
                );

            titleLabel->setStyleSheet(
                "font-size: 13px;"
                "color: #6b7280;"
            );


            *valueLabel =
                new QLabel(frame);

            (*valueLabel)->setStyleSheet(
                "font-size: 26px;"
                "font-weight: 700;"
            );


            cardLayout->addWidget(
                titleLabel
            );

            cardLayout->addWidget(
                *valueLabel
            );


            return frame;
        };


    layout->addWidget(
        createStatCard(
            "特别危险",
            &criticalCountLabel
        ),
        0,
        0
    );


    layout->addWidget(
        createStatCard(
            "危险",
            &dangerousCountLabel
        ),
        0,
        1
    );


    layout->addWidget(
        createStatCard(
            "待关注",
            &attentionCountLabel
        ),
        0,
        2
    );


    layout->addWidget(
        createStatCard(
            "一般",
            &normalCountLabel
        ),
        0,
        3
    );


    mainLayout->addWidget(
        warningStatisticsFrame
    );
}


/*
 * =========================================================
 * AI预测信息
 * =========================================================
 */
void AIWarningWidget::createPredictionCards()
{
    predictionFrame =
        new QFrame(contentWidget);


    QGridLayout *layout =
        new QGridLayout(predictionFrame);

    layout->setContentsMargins(
        0,
        0,
        0,
        0
    );

    layout->setSpacing(14);


    peakTimeLabel = nullptr;
    peakOccupancyLabel = nullptr;
    peakVolumeLabel = nullptr;
    idlePilesLabel = nullptr;


    QFrame *card1 =
        createInfoCard(
            "预测高峰时间",
            &peakTimeLabel,
            nullptr
        );


    QFrame *card2 =
        createInfoCard(
            "预测最高占用率",
            &peakOccupancyLabel,
            nullptr
        );


    QFrame *card3 =
        createInfoCard(
            "预测高峰订单量",
            &peakVolumeLabel,
            nullptr
        );


    QFrame *card4 =
        createInfoCard(
            "预测空闲充电桩",
            &idlePilesLabel,
            nullptr
        );


    layout->addWidget(
        card1,
        0,
        0
    );

    layout->addWidget(
        card2,
        0,
        1
    );

    layout->addWidget(
        card3,
        0,
        2
    );

    layout->addWidget(
        card4,
        0,
        3
    );


    mainLayout->addWidget(
        predictionFrame
    );
}


/*
 * =========================================================
 * 电站预警区域
 * =========================================================
 */
void AIWarningWidget::createStationWarningSection()
{
    stationWarningFrame =
        new QFrame(contentWidget);


    QVBoxLayout *outerLayout =
        new QVBoxLayout(
            stationWarningFrame
        );

    outerLayout->setContentsMargins(
        18,
        18,
        18,
        18
    );

    outerLayout->setSpacing(14);


    QLabel *title =
        new QLabel(
            "电站风险预警",
            stationWarningFrame
        );

    title->setStyleSheet(
        "font-size: 18px;"
        "font-weight: 700;"
        "color: #1f2937;"
    );


    QLabel *description =
        new QLabel(
            "查看 AI 模型预测出的高风险电站，并支持区域和预警等级筛选。",
            stationWarningFrame
        );

    description->setStyleSheet(
        "font-size: 13px;"
        "color: #6b7280;"
    );


    /*
     * =====================================================
     * 筛选区域
     * =====================================================
     */

    QFrame *filterFrame =
        new QFrame(
            stationWarningFrame
        );


    QHBoxLayout *filterLayout =
        new QHBoxLayout(filterFrame);

    filterLayout->setContentsMargins(
        0,
        6,
        0,
        6
    );

    filterLayout->setSpacing(10);


    QLabel *regionLabel =
        new QLabel(
            "区域：",
            filterFrame
        );


    QComboBox *regionCombo =
        new QComboBox(
            filterFrame
        );

    regionCombo->setObjectName(
        "regionCombo"
    );

    regionCombo->addItem(
        "全部区域",
        "ALL"
    );

    regionCombo->addItem(
        "华东地区",
        "华东地区"
    );

    regionCombo->addItem(
        "华北地区",
        "华北地区"
    );

    regionCombo->addItem(
        "华南地区",
        "华南地区"
    );

    regionCombo->addItem(
        "华中地区",
        "华中地区"
    );


    QLabel *levelLabel =
        new QLabel(
            "等级：",
            filterFrame
        );


    QComboBox *levelCombo =
        new QComboBox(
            filterFrame
        );

    levelCombo->setObjectName(
        "levelCombo"
    );


    levelCombo->addItem(
        "全部等级",
        "ALL"
    );

    levelCombo->addItem(
        "特别危险",
        static_cast<int>(
            AIWarningLevel::Critical
        )
    );

    levelCombo->addItem(
        "危险",
        static_cast<int>(
            AIWarningLevel::Dangerous
        )
    );

    levelCombo->addItem(
        "待关注",
        static_cast<int>(
            AIWarningLevel::Attention
        )
    );

    levelCombo->addItem(
        "一般",
        static_cast<int>(
            AIWarningLevel::Normal
        )
    );


    QLineEdit *searchEdit =
        new QLineEdit(
            filterFrame
        );

    searchEdit->setObjectName(
        "searchEdit"
    );

    searchEdit->setPlaceholderText(
        "输入电站名称或区域"
    );

    searchEdit->setMinimumWidth(
        220
    );


    QPushButton *searchButton =
        new QPushButton(
            "搜索",
            filterFrame
        );

    QPushButton *resetButton =
        new QPushButton(
            "重置",
            filterFrame
        );


    filterLayout->addWidget(
        regionLabel
    );

    filterLayout->addWidget(
        regionCombo
    );

    filterLayout->addWidget(
        levelLabel
    );

    filterLayout->addWidget(
        levelCombo
    );

    filterLayout->addWidget(
        searchEdit
    );

    filterLayout->addWidget(
        searchButton
    );

    filterLayout->addWidget(
        resetButton
    );

    filterLayout->addStretch();


    /*
     * =====================================================
     * 电站列表
     * =====================================================
     */

    stationWarningLayout =
        new QVBoxLayout;

    stationWarningLayout->setContentsMargins(
        0,
        0,
        0,
        0
    );

    stationWarningLayout->setSpacing(
        10
    );


    outerLayout->addWidget(
        title
    );

    outerLayout->addWidget(
        description
    );

    outerLayout->addWidget(
        filterFrame
    );

    outerLayout->addLayout(
        stationWarningLayout
    );


    mainLayout->addWidget(
        stationWarningFrame
    );


    /*
     * =====================================================
     * 筛选逻辑
     * =====================================================
     */

    auto filterFunction =
        [this,
         regionCombo,
         levelCombo,
         searchEdit]()
        {
            /*
             * 清空当前列表
             */
            QLayoutItem *item;

            while (
                (item =
                     stationWarningLayout->takeAt(0))
                != nullptr
            )
            {
                if (item->widget() != nullptr)
                {
                    item->widget()->deleteLater();
                }

                delete item;
            }


            QString region =
                regionCombo->currentData()
                    .toString();

            QVariant levelData =
                levelCombo->currentData();


            QString keyword =
                searchEdit->text().trimmed();


            int filteredCount = 0;


            /*
             * =================================================
             * 遍历所有 AI 电站预警
             * =================================================
             */

            for (
                const AIStationWarning &warning
                : currentResult.stationWarnings
            )
            {
                /*
                 * ---------------------------------------------
                 * 区域筛选
                 *
                 * 重点：
                 * 直接使用 warning.region
                 * 不再根据 stationName 猜区域。
                 * ---------------------------------------------
                 */

                if (
                    region != "ALL" &&
                    warning.region != region
                )
                {
                    continue;
                }


                /*
                 * ---------------------------------------------
                 * 等级筛选
                 * ---------------------------------------------
                 */

                if (
                    levelData.isValid() &&
                    levelData.toString() != "ALL"
                )
                {
                    int levelValue =
                        levelData.toInt();

                    if (
                        static_cast<int>(
                            warning.level
                        ) != levelValue
                    )
                    {
                        continue;
                    }
                }


                /*
                 * ---------------------------------------------
                 * 关键词搜索
                 *
                 * 搜索：
                 * 1. 电站名称
                 * 2. 所属区域
                 * 3. 风险描述
                 * ---------------------------------------------
                 */

                if (!keyword.isEmpty())
                {
                    if (
                        !warning.stationName.contains(
                            keyword,
                            Qt::CaseInsensitive
                        ) &&
                        !warning.region.contains(
                            keyword,
                            Qt::CaseInsensitive
                        ) &&
                        !warning.description.contains(
                            keyword,
                            Qt::CaseInsensitive
                        )
                    )
                    {
                        continue;
                    }
                }


                /*
                 * ---------------------------------------------
                 * 通过筛选
                 * ---------------------------------------------
                 */

                stationWarningLayout->addWidget(
                    createStationWarningCard(
                        warning
                    )
                );

                ++filteredCount;
            }


            /*
             * 如果没有匹配结果
             */

            if (filteredCount == 0)
            {
                QLabel *emptyLabel =
                    new QLabel(
                        "暂无符合条件的电站预警",
                        stationWarningFrame
                    );

                emptyLabel->setAlignment(
                    Qt::AlignCenter
                );

                emptyLabel->setStyleSheet(
                    "padding: 30px;"
                    "color: #9ca3af;"
                    "font-size: 14px;"
                );

                stationWarningLayout->addWidget(
                    emptyLabel
                );
            }
        };


    /*
     * 下拉框变化立即筛选
     */

    connect(
        regionCombo,
        QOverload<int>::of(
            &QComboBox::currentIndexChanged
        ),
        this,
        [filterFunction](int)
        {
            filterFunction();
        }
    );


    connect(
        levelCombo,
        QOverload<int>::of(
            &QComboBox::currentIndexChanged
        ),
        this,
        [filterFunction](int)
        {
            filterFunction();
        }
    );


    /*
     * 搜索按钮
     */

    connect(
        searchButton,
        &QPushButton::clicked,
        this,
        [filterFunction]()
        {
            filterFunction();
        }
    );


    /*
     * Enter 搜索
     */

    connect(
        searchEdit,
        &QLineEdit::returnPressed,
        this,
        [filterFunction]()
        {
            filterFunction();
        }
    );


    /*
     * 重置
     */

    connect(
        resetButton,
        &QPushButton::clicked,
        this,
        [regionCombo,
         levelCombo,
         searchEdit,
         filterFunction]()
        {
            regionCombo->setCurrentIndex(0);

            levelCombo->setCurrentIndex(0);

            searchEdit->clear();

            filterFunction();
        }
    );
}


/*
 * =========================================================
 * AI详情区域
 * =========================================================
 */
void AIWarningWidget::createWarningDetailSection()
{
    warningDetailFrame =
        new QFrame(contentWidget);


    warningDetailLayout =
        new QVBoxLayout(
            warningDetailFrame
        );


    warningDetailLayout->setContentsMargins(
        18,
        18,
        18,
        18
    );

    warningDetailLayout->setSpacing(
        10
    );


    QLabel *title =
        new QLabel(
            "AI分析详情",
            warningDetailFrame
        );

    title->setStyleSheet(
        "font-size: 18px;"
        "font-weight: 700;"
        "color: #1f2937;"
    );


    warningDetailLayout->addWidget(
        title
    );


    mainLayout->addWidget(
        warningDetailFrame
    );
}


/*
 * =========================================================
 * 创建普通信息卡片
 * =========================================================
 */
QFrame *AIWarningWidget::createInfoCard(
    const QString &title,
    QLabel **valueLabel,
    QLabel **descriptionLabel
)
{
    QFrame *frame =
        new QFrame(contentWidget);


    frame->setFrameShape(
        QFrame::StyledPanel
    );


    QVBoxLayout *layout =
        new QVBoxLayout(frame);


    layout->setContentsMargins(
        18,
        16,
        18,
        16
    );


    QLabel *titleLabel =
        new QLabel(
            title,
            frame
        );


    titleLabel->setStyleSheet(
        "font-size: 13px;"
        "color: #6b7280;"
    );


    if (valueLabel != nullptr)
    {
        *valueLabel =
            new QLabel(frame);

        (*valueLabel)->setStyleSheet(
            "font-size: 22px;"
            "font-weight: 700;"
        );

        layout->addWidget(
            *valueLabel
        );
    }


    if (descriptionLabel != nullptr)
    {
        *descriptionLabel =
            new QLabel(frame);

        (*descriptionLabel)->setWordWrap(
            true
        );

        (*descriptionLabel)->setStyleSheet(
            "font-size: 12px;"
            "color: #9ca3af;"
        );

        layout->addWidget(
            *descriptionLabel
        );
    }


    layout->insertWidget(
        0,
        titleLabel
    );


    return frame;
}


/*
 * =========================================================
 * 创建电站预警卡片
 * =========================================================
 */
QFrame *AIWarningWidget::createStationWarningCard(
    const AIStationWarning &warning
)
{
    QFrame *frame =
        new QFrame(
            stationWarningFrame
        );


    frame->setFrameShape(
        QFrame::StyledPanel
    );


    QHBoxLayout *layout =
        new QHBoxLayout(frame);


    layout->setContentsMargins(
        18,
        14,
        18,
        14
    );


    /*
     * 左侧
     */

    QVBoxLayout *leftLayout =
        new QVBoxLayout;


    QLabel *nameLabel =
        new QLabel(
            warning.stationName,
            frame
        );


    nameLabel->setStyleSheet(
        "font-size: 16px;"
        "font-weight: 700;"
        "color: #1f2937;"
    );


    QLabel *regionLabel =
        new QLabel(
            "区域：" + warning.region,
            frame
        );


    regionLabel->setStyleSheet(
        "font-size: 12px;"
        "color: #6b7280;"
    );


    QLabel *descriptionLabel =
        new QLabel(
            warning.description,
            frame
        );


    descriptionLabel->setWordWrap(
        true
    );


    descriptionLabel->setStyleSheet(
        "font-size: 13px;"
        "color: #4b5563;"
    );


    leftLayout->addWidget(
        nameLabel
    );

    leftLayout->addWidget(
        regionLabel
    );

    leftLayout->addWidget(
        descriptionLabel
    );


    /*
     * 中间建议
     */

    QLabel *suggestionLabel =
        new QLabel(
            "建议：" +
            warning.suggestion,
            frame
        );


    suggestionLabel->setWordWrap(
        true
    );


    suggestionLabel->setMinimumWidth(
        220
    );


    suggestionLabel->setStyleSheet(
        "font-size: 12px;"
        "color: #6b7280;"
    );


    /*
     * 右侧信息
     */

    QVBoxLayout *rightLayout =
        new QVBoxLayout;


    QLabel *levelLabel =
        new QLabel(
            warningLevelIcon(
                warning.level
            ) +
            " " +
            warningLevelText(
                warning.level
            ),
            frame
        );


    levelLabel->setAlignment(
        Qt::AlignRight
    );


    levelLabel->setStyleSheet(
        warningLevelStyle(
            warning.level
        )
    );


    QLabel *occupancyLabel =
        new QLabel(
            QString(
                "预测占用率：%1%"
            ).arg(
                warning.predictedOccupancy,
                0,
                'f',
                1
            ),
            frame
        );


    occupancyLabel->setAlignment(
        Qt::AlignRight
    );


    occupancyLabel->setStyleSheet(
        "font-size: 13px;"
        "font-weight: 600;"
        "color: #374151;"
    );


    rightLayout->addWidget(
        levelLabel
    );

    rightLayout->addWidget(
        occupancyLabel
    );

    rightLayout->addStretch();


    layout->addLayout(
        leftLayout,
        3
    );

    layout->addWidget(
        suggestionLabel,
        2
    );

    layout->addLayout(
        rightLayout,
        1
    );


    return frame;
}


/*
 * =========================================================
 * 刷新数据
 * =========================================================
 */
void AIWarningWidget::refreshData()
{
    /*
     * 当前 Linux 版本暂时使用模拟数据。
     *
     * 后续接入 AIManager 后：
     *
     * currentResult =
     *     aiManager->predict(...);
     *
     * 然后：
     *
     * setWarningResult(currentResult);
     */

    setWarningResult(
        currentResult
    );
}


/*
 * =========================================================
 * 设置 AI 预测结果
 * =========================================================
 */
void AIWarningWidget::setWarningResult(
    const AIWarningResult &result
)
{
    currentResult = result;


    updateCurrentWarning();

    updateModelStatus();

    updateWarningStatistics();

    updatePredictionCards();

    updateStationWarnings();

    updateWarningDetails();
}


/*
 * =========================================================
 * 更新总体预警
 * =========================================================
 */
void AIWarningWidget::updateCurrentWarning()
{
    if (currentWarningIcon == nullptr)
        return;


    currentWarningIcon->setText(
        warningLevelIcon(
            currentResult.level
        )
    );


    currentWarningLevel->setText(
        warningLevelText(
            currentResult.level
        )
    );


    currentWarningDescription->setText(
        warningLevelDescription(
            currentResult.level
        )
    );


    currentWarningLevel->setStyleSheet(
        warningLevelStyle(
            currentResult.level
        )
    );
}


/*
 * =========================================================
 * 更新模型状态
 * =========================================================
 */
void AIWarningWidget::updateModelStatus()
{
    if (modelStatusLabel == nullptr)
        return;


    if (currentResult.modelRunning)
    {
        modelStatusLabel->setText(
            "🟢 AI模型运行中"
        );
    }
    else
    {
        modelStatusLabel->setText(
            "🔴 AI模型未运行"
        );
    }


    lastUpdateLabel->setText(
        "最后分析时间：" +
        currentResult.lastUpdateTime
    );
}


/*
 * =========================================================
 * 更新预警统计
 * =========================================================
 */
void AIWarningWidget::updateWarningStatistics()
{
    if (criticalCountLabel != nullptr)
    {
        criticalCountLabel->setText(
            QString::number(
                currentResult.criticalCount
            )
        );
    }


    if (dangerousCountLabel != nullptr)
    {
        dangerousCountLabel->setText(
            QString::number(
                currentResult.dangerousCount
            )
        );
    }


    if (attentionCountLabel != nullptr)
    {
        attentionCountLabel->setText(
            QString::number(
                currentResult.attentionCount
            )
        );
    }


    if (normalCountLabel != nullptr)
    {
        normalCountLabel->setText(
            QString::number(
                currentResult.normalCount
            )
        );
    }
}


/*
 * =========================================================
 * 更新预测信息
 * =========================================================
 */
void AIWarningWidget::updatePredictionCards()
{
    if (peakTimeLabel != nullptr)
    {
        peakTimeLabel->setText(
            currentResult.peakTime
        );
    }


    if (peakOccupancyLabel != nullptr)
    {
        peakOccupancyLabel->setText(
            QString(
                "%1%"
            ).arg(
                currentResult.peakOccupancy,
                0,
                'f',
                1
            )
        );
    }


    if (peakVolumeLabel != nullptr)
    {
        peakVolumeLabel->setText(
            QString::number(
                currentResult.peakOrderVolume
            )
        );
    }


    if (idlePilesLabel != nullptr)
    {
        idlePilesLabel->setText(
            QString::number(
                currentResult.predictedIdlePiles
            )
        );
    }
}


/*
 * =========================================================
 * 更新电站预警列表
 * =========================================================
 */
void AIWarningWidget::updateStationWarnings()
{
    if (stationWarningLayout == nullptr)
        return;


    /*
     * 找到筛选控件
     */

    QComboBox *regionCombo =
        stationWarningFrame->findChild<QComboBox *>(
            "regionCombo"
        );

    QComboBox *levelCombo =
        stationWarningFrame->findChild<QComboBox *>(
            "levelCombo"
        );

    QLineEdit *searchEdit =
        stationWarningFrame->findChild<QLineEdit *>(
            "searchEdit"
        );


    /*
     * 清空列表
     */

    QLayoutItem *item;

    while (
        (item =
             stationWarningLayout->takeAt(0))
        != nullptr
    )
    {
        if (item->widget() != nullptr)
        {
            item->widget()->deleteLater();
        }

        delete item;
    }


    /*
     * 如果筛选控件不存在，
     * 则直接显示全部。
     */

    QString region = "ALL";

    QString keyword;


    if (regionCombo != nullptr)
    {
        region =
            regionCombo->currentData()
                .toString();
    }


    QVariant levelData;


    if (levelCombo != nullptr)
    {
        levelData =
            levelCombo->currentData();
    }


    if (searchEdit != nullptr)
    {
        keyword =
            searchEdit->text().trimmed();
    }


    int count = 0;


    /*
     * =====================================================
     * 遍历 AI 预警数据
     * =====================================================
     */

    for (
        const AIStationWarning &warning
        : currentResult.stationWarnings
    )
    {
        /*
         * 区域筛选
         *
         * 这里必须直接使用：
         *
         * warning.region
         *
         * =================================================
         */

        if (
            region != "ALL" &&
            warning.region != region
        )
        {
            continue;
        }


        /*
         * 等级筛选
         */

        if (
            levelData.isValid() &&
            levelData.toString() != "ALL"
        )
        {
            int levelValue =
                levelData.toInt();

            if (
                static_cast<int>(
                    warning.level
                ) != levelValue
            )
            {
                continue;
            }
        }


        /*
         * 关键词筛选
         *
         * 支持：
         *
         * 电站名称
         * 区域
         * 风险描述
         */

        if (!keyword.isEmpty())
        {
            if (
                !warning.stationName.contains(
                    keyword,
                    Qt::CaseInsensitive
                ) &&
                !warning.region.contains(
                    keyword,
                    Qt::CaseInsensitive
                ) &&
                !warning.description.contains(
                    keyword,
                    Qt::CaseInsensitive
                )
            )
            {
                continue;
            }
        }


        /*
         * 添加卡片
         */

        stationWarningLayout->addWidget(
            createStationWarningCard(
                warning
            )
        );

        ++count;
    }


    /*
     * 没有结果
     */

    if (count == 0)
    {
        QLabel *emptyLabel =
            new QLabel(
                "暂无符合条件的电站预警",
                stationWarningFrame
            );

        emptyLabel->setAlignment(
            Qt::AlignCenter
        );

        emptyLabel->setStyleSheet(
            "padding: 30px;"
            "color: #9ca3af;"
            "font-size: 14px;"
        );


        stationWarningLayout->addWidget(
            emptyLabel
        );
    }
}


/*
 * =========================================================
 * 更新 AI 详情
 * =========================================================
 */
void AIWarningWidget::updateWarningDetails()
{
    if (warningDetailLayout == nullptr)
        return;


    /*
     * 删除旧内容，但是保留第一个标题。
     */

    while (
        warningDetailLayout->count() > 1
    )
    {
        QLayoutItem *item =
            warningDetailLayout->takeAt(1);

        if (item->widget() != nullptr)
        {
            item->widget()->deleteLater();
        }

        delete item;
    }


    QLabel *messageLabel =
        new QLabel(
            currentResult.mainMessage,
            warningDetailFrame
        );


    messageLabel->setWordWrap(
        true
    );


    messageLabel->setStyleSheet(
        "font-size: 14px;"
        "color: #4b5563;"
        "line-height: 1.5;"
    );


    warningDetailLayout->addWidget(
        messageLabel
    );
}


/*
 * =========================================================
 * 获取预警等级文字
 * =========================================================
 */
QString AIWarningWidget::warningLevelText(
    AIWarningLevel level
) const
{
    switch (level)
    {
    case AIWarningLevel::Critical:
        return "特别危险";

    case AIWarningLevel::Dangerous:
        return "危险";

    case AIWarningLevel::Attention:
        return "待关注";

    case AIWarningLevel::Normal:
    default:
        return "一般";
    }
}


/*
 * =========================================================
 * 获取预警等级图标
 * =========================================================
 */
QString AIWarningWidget::warningLevelIcon(
    AIWarningLevel level
) const
{
    switch (level)
    {
    case AIWarningLevel::Critical:
        return "🚨";

    case AIWarningLevel::Dangerous:
        return "⚠️";

    case AIWarningLevel::Attention:
        return "🔔";

    case AIWarningLevel::Normal:
    default:
        return "✅";
    }
}


/*
 * =========================================================
 * 获取预警等级描述
 * =========================================================
 */
QString AIWarningWidget::warningLevelDescription(
    AIWarningLevel level
) const
{
    switch (level)
    {
    case AIWarningLevel::Critical:
        return "系统检测到多个高风险电站，需要立即关注并采取调度措施。";

    case AIWarningLevel::Dangerous:
        return "部分电站存在较高运行风险，建议及时关注并进行资源调度。";

    case AIWarningLevel::Attention:
        return "当前系统整体运行正常，但部分电站可能在高峰期间出现资源紧张。";

    case AIWarningLevel::Normal:
    default:
        return "当前系统运行正常，暂未发现明显异常风险。";
    }
}


/*
 * =========================================================
 * 获取预警等级样式
 * =========================================================
 */
QString AIWarningWidget::warningLevelStyle(
    AIWarningLevel level
) const
{
    switch (level)
    {
    case AIWarningLevel::Critical:
        return
            "font-size: 22px;"
            "font-weight: 700;"
            "color: #dc2626;";

    case AIWarningLevel::Dangerous:
        return
            "font-size: 22px;"
            "font-weight: 700;"
            "color: #ea580c;";

    case AIWarningLevel::Attention:
        return
            "font-size: 22px;"
            "font-weight: 700;"
            "color: #d97706;";

    case AIWarningLevel::Normal:
    default:
        return
            "font-size: 22px;"
            "font-weight: 700;"
            "color: #16a34a;";
    }
}
