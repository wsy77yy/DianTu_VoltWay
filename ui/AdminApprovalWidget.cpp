#include "AdminApprovalWidget.h"

#include "../service/AdminManageService.h"

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QAbstractItemView>


AdminApprovalWidget::AdminApprovalWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("管理员审批");

    resize(700, 450);


    // =====================================
    // 创建审批表格
    // =====================================

    table = new QTableWidget(this);

    table->setColumnCount(5);

    QStringList headers;

    headers
        << "账号"
        << "申请角色"
        << "时间"
        << "通过"
        << "拒绝";

    table->setHorizontalHeaderLabels(headers);


    // =====================================
    // 表格列宽
    // =====================================

    QHeaderView *header =
        table->horizontalHeader();

    // 五列平均占满整个表格
    for (int i = 0; i < 5; ++i)
    {
        header->setSectionResizeMode(
            i,
            QHeaderView::Stretch
        );
    }

    header->setStretchLastSection(false);


    // =====================================
    // 表格显示优化
    // =====================================

    table->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );

    table->setSelectionMode(
        QAbstractItemView::SingleSelection
    );

    table->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );

    table->setAlternatingRowColors(true);

    table->verticalHeader()->setDefaultSectionSize(
        40
    );

    table->verticalHeader()->setVisible(false);

    table->horizontalHeader()->setDefaultAlignment(
        Qt::AlignCenter
    );


    // =====================================
    // 创建底部按钮
    // 这里只保留“返回首页”
    // =====================================

    backButton =
        new QPushButton("返回首页", this);


    // =====================================
    // 按钮布局
    // =====================================

    QHBoxLayout *buttonLayout =
        new QHBoxLayout;

    buttonLayout->setSpacing(10);

    // 左侧留空
    buttonLayout->addStretch();

    // 右下角只保留返回首页
    buttonLayout->addWidget(
        backButton
    );


    // =====================================
    // 主布局
    // =====================================

    QVBoxLayout *layout =
        new QVBoxLayout(this);

    layout->setContentsMargins(
        20,
        20,
        20,
        20
    );

    layout->setSpacing(15);

    layout->addWidget(table);

    layout->addLayout(buttonLayout);

    setLayout(layout);


    // =====================================
    // 返回首页按钮连接
    // =====================================

    connect(
        backButton,
        &QPushButton::clicked,
        this,
        &AdminApprovalWidget::backClicked
    );


    // =====================================
    // 加载审批数据
    // =====================================

    loadRequests();
}


// =========================================
// 加载待审批申请
// =========================================

void AdminApprovalWidget::loadRequests()
{
    auto list =
        AdminManageService::instance()
        .pendingRequests();


    table->setRowCount(0);


    int row = 0;


    for (auto r : list)
    {
        table->insertRow(row);


        // -----------------------------
        // 账号
        // -----------------------------

        table->setItem(
            row,
            0,
            new QTableWidgetItem(
                r.account
            )
        );


        // -----------------------------
        // 申请角色
        // -----------------------------

        table->setItem(
            row,
            1,
            new QTableWidgetItem(
                r.applyRole
            )
        );


        // -----------------------------
        // 申请时间
        // -----------------------------

        QTableWidgetItem *timeItem =
            new QTableWidgetItem(
                r.applyTime
            );

        timeItem->setTextAlignment(
            Qt::AlignCenter
        );

        table->setItem(
            row,
            2,
            timeItem
        );


        // -----------------------------
        // 通过按钮
        // -----------------------------

        QPushButton *agree =
            new QPushButton("通过");

        table->setCellWidget(
            row,
            3,
            agree
        );


        // -----------------------------
        // 拒绝按钮
        // -----------------------------

        QPushButton *deny =
            new QPushButton("拒绝");

        table->setCellWidget(
            row,
            4,
            deny
        );


        // 保存申请 ID
        int id = r.id;


        // -----------------------------
        // 通过
        // -----------------------------

        connect(
            agree,
            &QPushButton::clicked,
            this,
            [this, id]()
            {
                AdminManageService::instance()
                    .approve(id);

                loadRequests();
            }
        );


        // -----------------------------
        // 拒绝
        // -----------------------------

        connect(
            deny,
            &QPushButton::clicked,
            this,
            [this, id]()
            {
                AdminManageService::instance()
                    .reject(id);

                loadRequests();
            }
        );


        row++;
    }
}
