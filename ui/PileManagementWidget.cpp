#include "PileManagementWidget.h"

#include "../manager/PileManager.h"
#include "../manager/StationManager.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QAbstractItemView>
#include <QTableWidgetItem>
#include <QCheckBox>
#include <QSignalBlocker>


// ============================================================
// 构造函数
// ============================================================

PileManagementWidget::PileManagementWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    loadStations();
    loadPiles();
}


// ============================================================
// 创建界面
// ============================================================

void PileManagementWidget::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);

    // ========================================================
    // 标题
    // ========================================================

    auto *titleLabel =
        new QLabel("充电桩管理", this);

    QFont titleFont;
    titleFont.setPointSize(18);
    titleFont.setBold(true);

    titleLabel->setFont(titleFont);

    mainLayout->addWidget(titleLabel);


    // ========================================================
    // 查询区域
    // ========================================================

    auto *filterLayout =
        new QHBoxLayout();

    searchEdit =
        new QLineEdit(this);

    searchEdit->setPlaceholderText(
        "输入电桩编号搜索"
    );

    stationCombo =
        new QComboBox(this);

    stationCombo->setMinimumWidth(160);

    statusCombo =
        new QComboBox(this);

    statusCombo->addItem("全部状态");
    statusCombo->addItem("在用");
    statusCombo->addItem("闲置");
    statusCombo->addItem("故障");
    statusCombo->addItem("断电");

    searchButton =
        new QPushButton("查询", this);

    resetButton =
        new QPushButton("重置", this);


    filterLayout->addWidget(
        new QLabel("电桩编号：")
    );

    filterLayout->addWidget(
        searchEdit
    );

    filterLayout->addWidget(
        new QLabel("所属电站：")
    );

    filterLayout->addWidget(
        stationCombo
    );

    filterLayout->addWidget(
        new QLabel("状态：")
    );

    filterLayout->addWidget(
        statusCombo
    );

    filterLayout->addWidget(
        searchButton
    );

    filterLayout->addWidget(
        resetButton
    );

    mainLayout->addLayout(
        filterLayout
    );


    // ========================================================
    // 批量操作区域
    // ========================================================

    auto *actionLayout =
        new QHBoxLayout();


    QPushButton *selectAllButton =
        new QPushButton("一键全选", this);

    QPushButton *clearSelectionButton =
        new QPushButton("取消全选", this);


    restartButton =
        new QPushButton("远程重启", this);

    shutdownButton =
        new QPushButton("紧急断电", this);


    actionLayout->addWidget(
        selectAllButton
    );

    actionLayout->addWidget(
        clearSelectionButton
    );

    actionLayout->addSpacing(15);

    actionLayout->addWidget(
        restartButton
    );

    actionLayout->addWidget(
        shutdownButton
    );

    actionLayout->addStretch();


    summaryLabel =
        new QLabel(this);

    actionLayout->addWidget(
        summaryLabel
    );

    mainLayout->addLayout(
        actionLayout
    );


    // ========================================================
    // 表格
    // ========================================================

    table =
        new QTableWidget(this);

    table->setColumnCount(10);

    table->setHorizontalHeaderLabels({
        "选择",
        "电桩编号",
        "所属电站",
        "类型",
        "功率(kW)",
        "状态",
        "在线",
        "累计充电次数",
        "累计充电时长",
        "操作状态"
    });


    table->horizontalHeader()
        ->setSectionResizeMode(
            QHeaderView::Stretch
        );


    // 第一列选择框不要太宽
    table->horizontalHeader()
        ->setSectionResizeMode(
            0,
            QHeaderView::Fixed
        );

    table->setColumnWidth(
        0,
        55
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

    table->verticalHeader()
        ->setVisible(false);


    mainLayout->addWidget(
        table
    );


    // ========================================================
    // 一键全选
    // ========================================================

    connect(
        selectAllButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            selectAllPiles();
        }
    );


    // ========================================================
    // 取消全选
    // ========================================================

    connect(
        clearSelectionButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            clearAllSelections();
        }
    );


    // ========================================================
    // 查询
    // ========================================================

    connect(
        searchButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            refreshTable();
        }
    );


    // ========================================================
    // 重置
    // ========================================================

    connect(
        resetButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            searchEdit->clear();

            stationCombo->setCurrentIndex(
                0
            );

            statusCombo->setCurrentIndex(
                0
            );

            refreshTable();
        }
    );


    // ========================================================
    // 远程重启
    // ========================================================

    connect(
        restartButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            restartSelectedPile();
        }
    );


    // ========================================================
    // 紧急断电
    // ========================================================

    connect(
        shutdownButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            shutdownSelectedPile();
        }
    );


    // ========================================================
    // 电站筛选
    // ========================================================

    connect(
        stationCombo,
        QOverload<int>::of(
            &QComboBox::currentIndexChanged
        ),
        this,
        [this](int)
        {
            refreshTable();
        }
    );


    // ========================================================
    // 状态筛选
    // ========================================================

    connect(
        statusCombo,
        QOverload<int>::of(
            &QComboBox::currentIndexChanged
        ),
        this,
        [this](int)
        {
            refreshTable();
        }
    );
}


// ============================================================
// 加载电站
// ============================================================

void PileManagementWidget::loadStations()
{
    stationCombo->clear();

    stationCombo->addItem(
        "全部电站",
        ""
    );


    const QList<ManagedStation> stations =
        StationManager::instance()
            .getStations();


    for (const auto &station : stations)
    {
        stationCombo->addItem(
            station.name,
            station.code
        );
    }
}


// ============================================================
// 初始加载
// ============================================================

void PileManagementWidget::loadPiles()
{
    refreshTable();
}


// ============================================================
// 刷新表格
// ============================================================

void PileManagementWidget::refreshTable()
{
    const QString keyword =
        searchEdit->text().trimmed();

    const QString stationCode =
        stationCombo->currentData()
            .toString();

    const QString status =
        statusCombo->currentText();


    const QList<ChargingPile> piles =
        PileManager::instance()
            .getPiles();


    table->setRowCount(0);


    int totalCount = 0;
    int onlineCount = 0;
    int faultCount = 0;
    int idleCount = 0;
    int usingCount = 0;


    for (const auto &pile : piles)
    {
        // ----------------------------------------------------
        // 电桩编号搜索
        // ----------------------------------------------------

        if (!keyword.isEmpty() &&
            !pile.code.contains(
                keyword,
                Qt::CaseInsensitive
            ))
        {
            continue;
        }


        // ----------------------------------------------------
        // 电站筛选
        // ----------------------------------------------------

        if (!stationCode.isEmpty() &&
            pile.stationCode != stationCode)
        {
            continue;
        }


        // ----------------------------------------------------
        // 状态筛选
        // ----------------------------------------------------

        if (status != "全部状态" &&
            pile.status != status)
        {
            continue;
        }


        // ----------------------------------------------------
        // 统计
        // ----------------------------------------------------

        ++totalCount;


        if (pile.online)
            ++onlineCount;


        if (pile.status == "故障")
            ++faultCount;


        if (pile.status == "闲置")
            ++idleCount;


        if (pile.status == "在用")
            ++usingCount;


        // ----------------------------------------------------
        // 创建行
        // ----------------------------------------------------

        const int row =
            table->rowCount();

        table->insertRow(row);


        // ----------------------------------------------------
        // 第一列：复选框
        // ----------------------------------------------------

        auto *checkBox =
            new QCheckBox(table);


        auto *checkWidget =
            new QWidget(table);

        auto *checkLayout =
            new QHBoxLayout(
                checkWidget
            );


        checkLayout->setContentsMargins(
            0,
            0,
            0,
            0
        );

        checkLayout->setAlignment(
            Qt::AlignCenter
        );

        checkLayout->addWidget(
            checkBox
        );


        table->setCellWidget(
            row,
            0,
            checkWidget
        );


        // ----------------------------------------------------
        // 电桩编号
        // ----------------------------------------------------

        table->setItem(
            row,
            1,
            new QTableWidgetItem(
                pile.code
            )
        );


        // ----------------------------------------------------
        // 所属电站
        // ----------------------------------------------------

        table->setItem(
            row,
            2,
            new QTableWidgetItem(
                pile.stationName
            )
        );


        // ----------------------------------------------------
        // 类型
        // ----------------------------------------------------

        table->setItem(
            row,
            3,
            new QTableWidgetItem(
                pile.type
            )
        );


        // ----------------------------------------------------
        // 功率
        // ----------------------------------------------------

        table->setItem(
            row,
            4,
            new QTableWidgetItem(
                QString::number(
                    pile.power,
                    'f',
                    1
                )
            )
        );


        // ----------------------------------------------------
        // 状态
        // ----------------------------------------------------

        table->setItem(
            row,
            5,
            new QTableWidgetItem(
                pile.status
            )
        );


        // ----------------------------------------------------
        // 在线
        // ----------------------------------------------------

        table->setItem(
            row,
            6,
            new QTableWidgetItem(
                pile.online
                    ? "在线"
                    : "离线"
            )
        );


        // ----------------------------------------------------
        // 累计次数
        // ----------------------------------------------------

        table->setItem(
            row,
            7,
            new QTableWidgetItem(
                QString::number(
                    pile.chargingCount
                )
            )
        );


        // ----------------------------------------------------
        // 累计时长
        // ----------------------------------------------------

        const double hours =
            pile.chargingMinutes / 60.0;


        table->setItem(
            row,
            8,
            new QTableWidgetItem(
                QString::number(
                    hours,
                    'f',
                    1
                ) + " h"
            )
        );


        // ----------------------------------------------------
        // 操作状态
        // ----------------------------------------------------

        QString operationStatus;


        if (pile.status == "故障")
        {
            operationStatus =
                "需要处理";
        }
        else if (pile.status == "断电")
        {
            operationStatus =
                "已断电";
        }
        else if (pile.status == "在用")
        {
            operationStatus =
                "正常运行";
        }
        else
        {
            operationStatus =
                "正常";
        }


        table->setItem(
            row,
            9,
            new QTableWidgetItem(
                operationStatus
            )
        );
    }


    // ========================================================
    // 汇总
    // ========================================================

    summaryLabel->setText(
        QString(
            "当前 %1 台 | 在线 %2 | 在用 %3 | 闲置 %4 | 故障 %5"
        )
        .arg(totalCount)
        .arg(onlineCount)
        .arg(usingCount)
        .arg(idleCount)
        .arg(faultCount)
    );
}


// ============================================================
// 一键全选
// ============================================================

void PileManagementWidget::selectAllPiles()
{
    for (int row = 0;
         row < table->rowCount();
         ++row)
    {
        QWidget *widget =
            table->cellWidget(
                row,
                0
            );

        if (!widget)
            continue;


        QCheckBox *checkBox =
            widget->findChild<QCheckBox*>();


        if (!checkBox)
            continue;


        checkBox->setChecked(true);
    }
}


// ============================================================
// 取消全选
// ============================================================

void PileManagementWidget::clearAllSelections()
{
    for (int row = 0;
         row < table->rowCount();
         ++row)
    {
        QWidget *widget =
            table->cellWidget(
                row,
                0
            );

        if (!widget)
            continue;


        QCheckBox *checkBox =
            widget->findChild<QCheckBox*>();


        if (!checkBox)
            continue;


        checkBox->setChecked(false);
    }
}


// ============================================================
// 获取所有选中的电桩编号
// ============================================================

QList<QString>
PileManagementWidget::selectedPileCodes() const
{
    QList<QString> result;


    for (int row = 0;
         row < table->rowCount();
         ++row)
    {
        QWidget *widget =
            table->cellWidget(
                row,
                0
            );

        if (!widget)
            continue;


        QCheckBox *checkBox =
            widget->findChild<QCheckBox*>();


        if (!checkBox ||
            !checkBox->isChecked())
        {
            continue;
        }


        QTableWidgetItem *item =
            table->item(
                row,
                1
            );


        if (!item)
            continue;


        result.append(
            item->text()
        );
    }


    return result;
}


// ============================================================
// 获取当前选中的单个电桩
// ============================================================

QString
PileManagementWidget::selectedPileCode() const
{
    const QList<QTableWidgetItem*> selected =
        table->selectedItems();


    if (selected.isEmpty())
        return QString();


    const int row =
        selected.first()->row();


    QTableWidgetItem *item =
        table->item(
            row,
            1
        );


    if (!item)
        return QString();


    return item->text();
}


// ============================================================
// 批量远程重启
// ============================================================

void PileManagementWidget::restartSelectedPile()
{
    const QList<QString> pileCodes =
        selectedPileCodes();


    if (pileCodes.isEmpty())
    {
        QMessageBox::warning(
            this,
            "提示",
            "请至少勾选一个电桩。"
        );

        return;
    }


    const int result =
        QMessageBox::question(
            this,
            "批量远程重启",
            QString(
                "确定要对选中的 %1 台电桩执行远程重启吗？"
            )
            .arg(pileCodes.size())
        );


    if (result != QMessageBox::Yes)
        return;


    int successCount = 0;


    for (const QString &pileCode :
         pileCodes)
    {
        if (PileManager::instance()
                .restartPile(pileCode))
        {
            ++successCount;
        }
    }


    QMessageBox::information(
        this,
        "操作完成",
        QString(
            "远程重启指令已发送。\n\n"
            "成功：%1 台\n"
            "失败：%2 台"
        )
        .arg(successCount)
        .arg(
            pileCodes.size()
            - successCount
        )
    );


    refreshTable();
}


// ============================================================
// 批量紧急断电
// ============================================================

void PileManagementWidget::shutdownSelectedPile()
{
    const QList<QString> pileCodes =
        selectedPileCodes();


    if (pileCodes.isEmpty())
    {
        QMessageBox::warning(
            this,
            "提示",
            "请至少勾选一个电桩。"
        );

        return;
    }


    const int result =
        QMessageBox::warning(
            this,
            "批量紧急断电",
            QString(
                "确定要对选中的 %1 台电桩执行紧急断电吗？\n\n"
                "该操作会使这些电桩立即离线。"
            )
            .arg(pileCodes.size()),
            QMessageBox::Yes |
            QMessageBox::No,
            QMessageBox::No
        );


    if (result != QMessageBox::Yes)
        return;


    int successCount = 0;


    for (const QString &pileCode :
         pileCodes)
    {
        if (PileManager::instance()
                .emergencyShutdown(pileCode))
        {
            ++successCount;
        }
    }


    QMessageBox::information(
        this,
        "操作完成",
        QString(
            "紧急断电指令已发送。\n\n"
            "成功：%1 台\n"
            "失败：%2 台"
        )
        .arg(successCount)
        .arg(
            pileCodes.size()
            - successCount
        )
    );


    refreshTable();
}
