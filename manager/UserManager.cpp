#include "UserManager.h"

#include "OrderManager.h"

#include <QDateTime>


// ============================================================
// 单例
// ============================================================

UserManager& UserManager::instance()
{
    static UserManager manager;

    return manager;
}


// ============================================================
// 构造函数
// ============================================================

UserManager::UserManager()
{
    createTestUsers();
}


// ============================================================
// 创建测试用户
// ============================================================

void UserManager::createTestUsers()
{
    ManagedUser u1;

    u1.userId = "U001";
    u1.phone = "13800138001";
    u1.nickname = "张三";

    u1.balance = 86.50;

    u1.registerTime =
        QDateTime::currentDateTime()
            .addDays(-60);

    u1.status = "正常";

    users.append(u1);


    ManagedUser u2;

    u2.userId = "U002";
    u2.phone = "13900139002";
    u2.nickname = "李四";

    u2.balance = 125.00;

    u2.registerTime =
        QDateTime::currentDateTime()
            .addDays(-45);

    u2.status = "正常";

    users.append(u2);


    ManagedUser u3;

    u3.userId = "U003";
    u3.phone = "13700137003";
    u3.nickname = "王五";

    u3.balance = 52.30;

    u3.registerTime =
        QDateTime::currentDateTime()
            .addDays(-30);

    u3.status = "正常";

    users.append(u3);


    ManagedUser u4;

    u4.userId = "U004";
    u4.phone = "13600136004";
    u4.nickname = "赵六";

    u4.balance = 210.80;

    u4.registerTime =
        QDateTime::currentDateTime()
            .addDays(-20);

    u4.status = "冻结";

    users.append(u4);


    ManagedUser u5;

    u5.userId = "U005";
    u5.phone = "13500135005";
    u5.nickname = "小明";

    u5.balance = 68.00;

    u5.registerTime =
        QDateTime::currentDateTime()
            .addDays(-10);

    u5.status = "正常";

    users.append(u5);
}


// ============================================================
// 获取全部用户
// ============================================================

QList<ManagedUser> UserManager::getUsers() const
{
    return users;
}


// ============================================================
// 获取单个用户
// ============================================================

ManagedUser UserManager::getUser(
    const QString &userId
) const
{
    for (const ManagedUser &user : users)
    {
        if (user.userId == userId)
        {
            return user;
        }
    }

    return ManagedUser();
}


// ============================================================
// 手机号模糊搜索
// ============================================================

QList<ManagedUser>
UserManager::searchUsersByPhone(
    const QString &keyword
) const
{
    QList<ManagedUser> result;

    QString text =
        keyword.trimmed();

    // 搜索为空时返回全部用户
    if (text.isEmpty())
    {
        return users;
    }

    for (const ManagedUser &user : users)
    {
        if (user.phone.contains(text))
        {
            result.append(user);
        }
    }

    return result;
}


// ============================================================
// 冻结用户
// ============================================================

bool UserManager::freezeUser(
    const QString &userId
)
{
    for (ManagedUser &user : users)
    {
        if (user.userId != userId)
            continue;

        if (user.status == "冻结")
            return false;

        user.status = "冻结";

        return true;
    }

    return false;
}


// ============================================================
// 解冻用户
// ============================================================

bool UserManager::unfreezeUser(
    const QString &userId
)
{
    for (ManagedUser &user : users)
    {
        if (user.userId != userId)
            continue;

        if (user.status == "正常")
            return false;

        user.status = "正常";

        return true;
    }

    return false;
}


// ============================================================
// 切换用户状态
// ============================================================

bool UserManager::toggleUserStatus(
    const QString &userId
)
{
    for (ManagedUser &user : users)
    {
        if (user.userId != userId)
            continue;

        if (user.status == "正常")
        {
            user.status = "冻结";
        }
        else
        {
            user.status = "正常";
        }

        return true;
    }

    return false;
}


// ============================================================
// 获取用户所有订单
// ============================================================

QList<ChargingOrder>
UserManager::getUserOrders(
    const QString &userId
) const
{
    QList<ChargingOrder> result;

    QList<ChargingOrder> allOrders =
        OrderManager::instance()
            .getOrders();

    for (const ChargingOrder &order : allOrders)
    {
        if (order.userId == userId)
        {
            result.append(order);
        }
    }

    return result;
}


// ============================================================
// 判断是否允许管理员代结算
// ============================================================

bool UserManager::canAdminSettleOrder(
    const QString &orderId
) const
{
    ChargingOrder order =
        OrderManager::instance()
            .getOrder(orderId);

    // 必须是进行中的订单
    if (order.status != "进行中")
    {
        return false;
    }

    // 超过12小时
    if (order.durationSeconds() >=
        12 * 60 * 60)
    {
        return true;
    }

    // 用户已经提交管理员取消申请
    if (OrderManager::instance()
            .hasPendingCancelRequest(orderId))
    {
        return true;
    }

    return false;
}


// ============================================================
// 管理员代结算
// ============================================================

bool UserManager::adminSettleOrder(
    const QString &orderId,
    double amount
)
{
    if (!canAdminSettleOrder(orderId))
    {
        return false;
    }

    if (amount <= 0)
    {
        return false;
    }

    /*
     * 管理员代结算直接调用 OrderManager
     * 正常完成订单会：
     *
     * 1. 修改订单状态
     * 2. 写入金额
     * 3. 写入结束时间
     * 4. 产生收入
     */
    return OrderManager::instance()
        .finishOrder(
            orderId,
            amount
        );
}


// ============================================================
// 清空用户
// ============================================================

void UserManager::clearUsers()
{
    users.clear();
}