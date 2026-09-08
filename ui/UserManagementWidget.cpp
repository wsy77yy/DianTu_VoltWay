#include "UserManagementWidget.h"

#include "../manager/UserManager.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QTableWidgetItem>


// ============================================================
// 构造函数
// ============================================================

UserManagementWidget::UserManagementWidget(
    QWidget *parent
)
    : QWidget(parent)
{
    setupUi();

    loadUsers();
}


// ============================================================
// 创建界面
// ============================================================

void UserManagementWidget::setupUi()
{
    auto *mainLayout =
        new QVBoxLayout(this);

    // ========================================================
    // 标题
    // ========================================================

    auto *titleLabel =
        new QLabel(
            "用户管理",
            this
        );

    QFont titleFont;

    titleFont.setPointSize(18);
    titleFont.setBold(true);

    titleLabel->setFont(titleFont);

    mainLayout->addWidget(
        titleLabel
    );


    // ========================================================
    // 查询区域
    // ========================================================

    auto *searchLayout =
        new QHBoxLayout();

    searchEdit =
        new QLineEdit(this);

    searchEdit->setPlaceholderText(
        "输入手机号进行模糊搜索"
    );

    searchButton =
        new QPushButton(
            "查询",
            this
        );

    resetButton =
        new QPushButton(
            "重置",
            this
        );


    searchLayout->addWidget(
        new QLabel(
            "手机号：",
            this
        )
    );

    searchLayout->addWidget(
        searchEdit
    );

    searchLayout->addWidget(
        searchButton
    );

    searchLayout->addWidget(
        resetButton
    );

    searchLayout->addStretch();


    mainLayout->addLayout(
        searchLayout
    );


    // ========================================================
    // 操作区域
    // ========================================================

    auto *actionLayout =
        new QHBoxLayout();


    freezeButton =
        new QPushButton(
            "冻结用户",
            this
        );

    unfreezeButton =
        new QPushButton(
            "解冻用户",
            this
        );

    ordersButton =
        new QPushButton(
            "查看用户订单",
            this
        );


    actionLayout->addWidget(
        freezeButton
    );

    actionLayout->addWidget(
        unfreezeButton
    );

    actionLayout->addWidget(
        ordersButton
    );

    actionLayout->addStretch();


    summaryLabel =
        new QLabel(
            this
        );


    actionLayout->addWidget(
        summaryLabel
    );


    mainLayout->addLayout(
        actionLayout
    );


    // ========================================================
    // 用户表格
    // ========================================================

    table =
        new QTableWidget(
            this
        );


    table->setColumnCount(
        6
    );


    table->setHorizontalHeaderLabels({
        "用户ID",
        "手机号",
        "昵称",
        "钱包余额",
        "注册时间",
        "状态"
    });


    table->horizontalHeader()
        ->setSectionResizeMode(
            QHeaderView::Stretch
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

            refreshTable();
        }
    );


    // ========================================================
    // 回车搜索
    // ========================================================

    connect(
        searchEdit,
        &QLineEdit::returnPressed,
        this,
        [this]()
        {
            refreshTable();
        }
    );


    // ========================================================
    // 冻结
    // ========================================================

    connect(
        freezeButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            freezeSelectedUser();
        }
    );


    // ========================================================
    // 解冻
    // ========================================================

    connect(
        unfreezeButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            unfreezeSelectedUser();
        }
    );


    // ========================================================
    // 查看订单
    // ========================================================

    connect(
        ordersButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            viewSelectedUserOrders();
        }
    );
}


// ============================================================
// 加载用户
// ============================================================

void UserManagementWidget::loadUsers()
{
    refreshTable();
}


// ============================================================
// 刷新用户表格
// ============================================================

void UserManagementWidget::refreshTable()
{
    const QString keyword =
        searchEdit->text().trimmed();


    QList<ManagedUser> users;


    if (keyword.isEmpty())
    {
        users =
            UserManager::instance()
                .getUsers();
    }
    else
    {
        users =
            UserManager::instance()
                .searchUsersByPhone(
                    keyword
                );
    }


    table->setRowCount(0);


    int normalCount = 0;
    int frozenCount = 0;


    for (const auto &user : users)
    {
        const int row =
            table->rowCount();


        table->insertRow(row);


        // ----------------------------------------------------
        // 用户ID
        // ----------------------------------------------------

        table->setItem(
            row,
            0,
            new QTableWidgetItem(
                user.userId
            )
        );


        // ----------------------------------------------------
        // 手机号
        // ----------------------------------------------------

        table->setItem(
            row,
            1,
            new QTableWidgetItem(
                user.phone
            )
        );


        // ----------------------------------------------------
        // 昵称
        // ----------------------------------------------------

        table->setItem(
            row,
            2,
            new QTableWidgetItem(
                user.nickname
            )
        );


        // ----------------------------------------------------
        // 钱包余额
        // ----------------------------------------------------

        table->setItem(
            row,
            3,
            new QTableWidgetItem(
                QString("¥%1")
                    .arg(
                        user.balance,
                        0,
                        'f',
                        2
                    )
            )
        );


        // ----------------------------------------------------
        // 注册时间
        // ----------------------------------------------------

        table->setItem(
            row,
            4,
            new QTableWidgetItem(
                user.registerTime
                    .toString(
                        "yyyy-MM-dd HH:mm"
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
                user.status
            )
        );


        if (user.status == "正常")
        {
            ++normalCount;
        }
        else if (user.status == "冻结")
        {
            ++frozenCount;
        }
    }


    // ========================================================
    // 汇总
    // ========================================================

    summaryLabel->setText(
        QString(
            "共 %1 人 | 正常 %2 | 冻结 %3"
        )
        .arg(users.size())
        .arg(normalCount)
        .arg(frozenCount)
    );
}


// ============================================================
// 获取当前用户ID
// ============================================================

QString UserManagementWidget::selectedUserId() const
{
    const QList<QTableWidgetItem*> selected =
        table->selectedItems();


    if (selected.isEmpty())
    {
        return QString();
    }


    const int row =
        selected.first()->row();


    QTableWidgetItem *item =
        table->item(
            row,
            0
        );


    if (!item)
    {
        return QString();
    }


    return item->text();
}


// ============================================================
// 冻结用户
// ============================================================

void UserManagementWidget::freezeSelectedUser()
{
    const QString userId =
        selectedUserId();


    if (userId.isEmpty())
    {
        QMessageBox::warning(
            this,
            "提示",
            "请先选择一个用户。"
        );

        return;
    }


    ManagedUser user =
        UserManager::instance()
            .getUser(userId);


    if (user.userId.isEmpty())
    {
        QMessageBox::warning(
            this,
            "操作失败",
            "未找到该用户。"
        );

        return;
    }


    if (user.status == "冻结")
    {
        QMessageBox::information(
            this,
            "提示",
            "该用户已经处于冻结状态。"
        );

        return;
    }


    const int result =
        QMessageBox::warning(
            this,
            "冻结用户",
            QString(
                "确定要冻结用户 %1（%2）吗？"
            )
            .arg(
                user.userId
            )
            .arg(
                user.nickname
            ),
            QMessageBox::Yes |
            QMessageBox::No,
            QMessageBox::No
        );


    if (result != QMessageBox::Yes)
    {
        return;
    }


    if (
        UserManager::instance()
            .freezeUser(userId)
    )
    {
        QMessageBox::information(
            this,
            "操作成功",
            QString(
                "用户 %1 已冻结。"
            )
            .arg(userId)
        );

        refreshTable();
    }
    else
    {
        QMessageBox::warning(
            this,
            "操作失败",
            "冻结用户失败。"
        );
    }
}


// ============================================================
// 解冻用户
// ============================================================

void UserManagementWidget::unfreezeSelectedUser()
{
    const QString userId =
        selectedUserId();


    if (userId.isEmpty())
    {
        QMessageBox::warning(
            this,
            "提示",
            "请先选择一个用户。"
        );

        return;
    }


    ManagedUser user =
        UserManager::instance()
            .getUser(userId);


    if (user.userId.isEmpty())
    {
        QMessageBox::warning(
            this,
            "操作失败",
            "未找到该用户。"
        );

        return;
    }


    if (user.status == "正常")
    {
        QMessageBox::information(
            this,
            "提示",
            "该用户已经处于正常状态。"
        );

        return;
    }


    const int result =
        QMessageBox::question(
            this,
            "解冻用户",
            QString(
                "确定要解冻用户 %1（%2）吗？"
            )
            .arg(
                user.userId
            )
            .arg(
                user.nickname
            )
        );


    if (result != QMessageBox::Yes)
    {
        return;
    }


    if (
        UserManager::instance()
            .unfreezeUser(userId)
    )
    {
        QMessageBox::information(
            this,
            "操作成功",
            QString(
                "用户 %1 已解冻。"
            )
            .arg(userId)
        );

        refreshTable();
    }
    else
    {
        QMessageBox::warning(
            this,
            "操作失败",
            "解冻用户失败。"
        );
    }
}


// ============================================================
// 查看用户全部订单
// ============================================================

void UserManagementWidget::viewSelectedUserOrders()
{
    const QString userId =
        selectedUserId();


    if (userId.isEmpty())
    {
        QMessageBox::warning(
            this,
            "提示",
            "请先选择一个用户。"
        );

        return;
    }


    ManagedUser user =
        UserManager::instance()
            .getUser(userId);


    if (user.userId.isEmpty())
    {
        QMessageBox::warning(
            this,
            "操作失败",
            "未找到该用户。"
        );

        return;
    }


    const QList<ChargingOrder> orders =
        UserManager::instance()
            .getUserOrders(userId);


    QDialog dialog(this);

    dialog.setWindowTitle(
        QString(
            "用户订单 - %1 (%2)"
        )
        .arg(
            user.nickname
        )
        .arg(
            userId
        )
    );


    dialog.resize(
        900,
        500
    );


    auto *layout =
        new QVBoxLayout(
            &dialog
        );


    auto *infoLabel =
        new QLabel(
            QString(
                "用户：%1    手机号：%2    订单数量：%3"
            )
            .arg(
                user.nickname
            )
            .arg(
                user.phone
            )
            .arg(
                orders.size()
            )
        );


    layout->addWidget(
        infoLabel
    );


    auto *orderTable =
        new QTableWidget(
            &dialog
        );


    orderTable->setColumnCount(
        7
    );


    orderTable->setHorizontalHeaderLabels({
        "订单ID",
        "电站",
        "电桩",
        "金额",
        "状态",
        "开始时间",
        "持续时间"
    });


    orderTable->horizontalHeader()
        ->setSectionResizeMode(
            QHeaderView::Stretch
        );


    orderTable->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );


    orderTable->setSelectionMode(
        QAbstractItemView::SingleSelection
    );


    orderTable->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );


    orderTable->verticalHeader()
        ->setVisible(false);


    for (const auto &order : orders)
    {
        const int row =
            orderTable->rowCount();


        orderTable->insertRow(row);


        orderTable->setItem(
            row,
            0,
            new QTableWidgetItem(
                order.orderId
            )
        );


        orderTable->setItem(
            row,
            1,
            new QTableWidgetItem(
                order.stationCode
            )
        );


        orderTable->setItem(
            row,
            2,
            new QTableWidgetItem(
                order.pileCode
            )
        );


        orderTable->setItem(
            row,
            3,
            new QTableWidgetItem(
                QString("¥%1")
                    .arg(
                        order.amount,
                        0,
                        'f',
                        2
                    )
            )
        );


        orderTable->setItem(
            row,
            4,
            new QTableWidgetItem(
                order.status
            )
        );


        orderTable->setItem(
            row,
            5,
            new QTableWidgetItem(
                order.createTime.toString(
                    "yyyy-MM-dd HH:mm"
                )
            )
        );


        const qint64 seconds =
            order.durationSeconds();


        const qint64 hours =
            seconds / 3600;


        const qint64 minutes =
            (seconds % 3600) / 60;


        orderTable->setItem(
            row,
            6,
            new QTableWidgetItem(
                QString("%1小时%2分钟")
                    .arg(hours)
                    .arg(minutes)
            )
        );
    }


    layout->addWidget(
        orderTable
    );


    // ========================================================
    // 代结算按钮
    // ========================================================

    auto *settleButton =
        new QPushButton(
            "管理员代结算",
            &dialog
        );


    layout->addWidget(
        settleButton
    );


    // ========================================================
    // 关闭按钮
    // ========================================================

    auto *buttonBox =
        new QDialogButtonBox(
            QDialogButtonBox::Close,
            &dialog
        );


    layout->addWidget(
        buttonBox
    );


    connect(
        buttonBox,
        &QDialogButtonBox::rejected,
        &dialog,
        &QDialog::reject
    );


    connect(
        settleButton,
        &QPushButton::clicked,
        &dialog,
        [this, &dialog, orderTable]()
        {
            const QList<QTableWidgetItem*> selected =
                orderTable->selectedItems();


            if (selected.isEmpty())
            {
                QMessageBox::warning(
                    &dialog,
                    "提示",
                    "请先选择一个订单。"
                );

                return;
            }


            const int row =
                selected.first()->row();


            QTableWidgetItem *orderIdItem =
                orderTable->item(
                    row,
                    0
                );


            if (!orderIdItem)
                return;


            const QString orderId =
                orderIdItem->text();


            if (
                !UserManager::instance()
                    .canAdminSettleOrder(
                        orderId
                    )
            )
            {
                QMessageBox::information(
                    &dialog,
                    "暂不可结算",
                    "该订单目前不满足管理员代结算条件。\n\n"
                    "条件：进行中订单已达到12小时，"
                    "或存在用户代结算请求。"
                );

                return;
            }


            ChargingOrder order =
                OrderManager::instance()
                    .getOrder(orderId);


            if (order.orderId.isEmpty())
            {
                QMessageBox::warning(
                    &dialog,
                    "操作失败",
                    "未找到该订单。"
                );

                return;
            }


            // =================================================
            // 金额输入
            // =================================================

            QDialog amountDialog(
                &dialog
            );


            amountDialog.setWindowTitle(
                "管理员代结算"
            );


            amountDialog.resize(
                360,
                160
            );


            auto *formLayout =
                new QFormLayout(
                    &amountDialog
                );


            auto *amountSpin =
                new QDoubleSpinBox(
                    &amountDialog
                );


            amountSpin->setRange(
                0.01,
                999999.99
            );


            amountSpin->setDecimals(
                2
            );


            amountSpin->setValue(
                order.amount > 0
                    ? order.amount
                    : 50.00
            );


            amountSpin->setPrefix(
                "¥ "
            );


            formLayout->addRow(
                "结算金额：",
                amountSpin
            );


            auto *amountButtons =
                new QDialogButtonBox(
                    QDialogButtonBox::Ok |
                    QDialogButtonBox::Cancel,
                    &amountDialog
                );


            formLayout->addWidget(
                amountButtons
            );


            connect(
                amountButtons,
                &QDialogButtonBox::accepted,
                &amountDialog,
                &QDialog::accept
            );


            connect(
                amountButtons,
                &QDialogButtonBox::rejected,
                &amountDialog,
                &QDialog::reject
            );


            if (
                amountDialog.exec()
                != QDialog::Accepted
            )
            {
                return;
            }


            const double amount =
                amountSpin->value();


            if (
                UserManager::instance()
                    .adminSettleOrder(
                        orderId,
                        amount
                    )
            )
            {
                QMessageBox::information(
                    &dialog,
                    "结算成功",
                    QString(
                        "订单 %1 已由管理员代结算。\n"
                        "结算金额：¥%2"
                    )
                    .arg(
                        orderId
                    )
                    .arg(
                        amount,
                        0,
                        'f',
                        2
                    )
                );


                // ------------------------------------------------
                // 更新订单表格
                // ------------------------------------------------

                orderTable->item(
                    row,
                    3
                )->setText(
                    QString("¥%1")
                        .arg(
                            amount,
                            0,
                            'f',
                            2
                        )
                );


                orderTable->item(
                    row,
                    4
                )->setText(
                    "已完成"
                );
            }
            else
            {
                QMessageBox::warning(
                    &dialog,
                    "结算失败",
                    "管理员代结算失败。"
                );
            }
        }
    );


    dialog.exec();
}