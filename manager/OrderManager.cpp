#include "OrderManager.h"

#include <QDateTime>


// ============================================================
// 单例
// ============================================================
OrderManager& OrderManager::instance()
{
    static OrderManager manager;
    return manager;
}


// ============================================================
// 构造函数
// ============================================================
OrderManager::OrderManager()
{
    createTestOrders();
}


// ============================================================
// 创建订单
// ============================================================
void OrderManager::createOrder(const QString &orderId,
                                const QString &userId,
                                const QString &stationCode,
                                const QString &pileCode)
{
    // --------------------------------------------------------
    // 防止重复创建相同订单
    // --------------------------------------------------------
    for (const ChargingOrder &order : orders)
    {
        if (order.orderId == orderId)
            return;
    }

    ChargingOrder order;

    order.orderId = orderId;
    order.userId = userId;
    order.stationCode = stationCode;
    order.pileCode = pileCode;

    order.amount = 0.0;
    order.status = QStringLiteral("进行中");

    order.createTime = QDateTime::currentDateTime();
    order.finishTime = QDateTime();

    orders.append(order);
}


// ============================================================
// 完成订单
// ============================================================
bool OrderManager::finishOrder(const QString &orderId,
                                double amount)
{
    for (ChargingOrder &order : orders)
    {
        if (order.orderId != orderId)
            continue;

        // ----------------------------------------------------
        // 只有进行中的订单才能完成
        // ----------------------------------------------------
        if (order.status != QStringLiteral("进行中"))
            return false;

        order.amount = amount;
        order.status = QStringLiteral("已完成");
        order.finishTime = QDateTime::currentDateTime();

        return true;
    }

    return false;
}


// ============================================================
// 用户主动取消订单
// ============================================================
bool OrderManager::cancelOrder(const QString &orderId)
{
    for (ChargingOrder &order : orders)
    {
        if (order.orderId != orderId)
            continue;

        // ----------------------------------------------------
        // 只有进行中的订单才能取消
        // ----------------------------------------------------
        if (order.status != QStringLiteral("进行中"))
            return false;

        order.status = QStringLiteral("已取消");
        order.finishTime = QDateTime::currentDateTime();

        // ----------------------------------------------------
        // 取消订单不产生收入
        // ----------------------------------------------------
        order.amount = 0.0;

        return true;
    }

    return false;
}


// ============================================================
// 判断订单是否超时
// ============================================================
bool OrderManager::isOvertimeOrder(const QString &orderId) const
{
    for (const ChargingOrder &order : orders)
    {
        if (order.orderId != orderId)
            continue;

        // ----------------------------------------------------
        // 非进行中订单不存在超时问题
        // ----------------------------------------------------
        if (order.status != QStringLiteral("进行中"))
            return false;

        if (!order.createTime.isValid())
            return false;

        // ----------------------------------------------------
        // 当前项目规则：超过 12 小时视为超时
        // ----------------------------------------------------
        const qint64 seconds =
            order.createTime.secsTo(QDateTime::currentDateTime());

        return seconds >= 12 * 60 * 60;
    }

    return false;
}


// ============================================================
// 管理员直接取消订单
// ============================================================
bool OrderManager::adminCancelOrder(const QString &orderId)
{
    for (ChargingOrder &order : orders)
    {
        if (order.orderId != orderId)
            continue;

        // ----------------------------------------------------
        // 已完成 / 已取消订单不能再次取消
        // ----------------------------------------------------
        if (order.status != QStringLiteral("进行中"))
            return false;

        order.status = QStringLiteral("已取消");
        order.finishTime = QDateTime::currentDateTime();

        // ----------------------------------------------------
        // 管理员取消同样不产生收入
        // ----------------------------------------------------
        order.amount = 0.0;

        return true;
    }

    return false;
}


// ============================================================
// 用户申请管理员取消
// ============================================================
bool OrderManager::requestAdminCancel(const QString &orderId,
                                       const QString &reason)
{
    // --------------------------------------------------------
    // 首先确认订单存在
    // --------------------------------------------------------
    bool found = false;

    for (const ChargingOrder &order : orders)
    {
        if (order.orderId == orderId)
        {
            found = true;

            // 只有进行中的订单可以申请取消
            if (order.status != QStringLiteral("进行中"))
                return false;

            break;
        }
    }

    if (!found)
        return false;

    // --------------------------------------------------------
    // 同一个订单不能重复提交待处理申请
    // --------------------------------------------------------
    if (hasPendingCancelRequest(orderId))
        return false;

    // --------------------------------------------------------
    // 创建取消申请
    // --------------------------------------------------------
    OrderCancelRequest request;

    request.requestId =
        QStringLiteral("CR%1")
            .arg(cancelRequests.size() + 1, 3, 10, QChar('0'));

    request.orderId = orderId;
    request.reason = reason;
    request.requestTime = QDateTime::currentDateTime();
    request.status = QStringLiteral("待处理");

    cancelRequests.append(request);

    return true;
}


// ============================================================
// 获取所有取消申请
// ============================================================
QList<OrderCancelRequest> OrderManager::getCancelRequests() const
{
    return cancelRequests;
}


// ============================================================
// 管理员同意取消申请
// ============================================================
bool OrderManager::approveCancelRequest(const QString &requestId)
{
    for (OrderCancelRequest &request : cancelRequests)
    {
        if (request.requestId != requestId)
            continue;

        // ----------------------------------------------------
        // 只有待处理申请才能审批
        // ----------------------------------------------------
        if (request.status != QStringLiteral("待处理"))
            return false;

        // ----------------------------------------------------
        // 找到对应订单
        // ----------------------------------------------------
        for (ChargingOrder &order : orders)
        {
            if (order.orderId != request.orderId)
                continue;

            if (order.status != QStringLiteral("进行中"))
                return false;

            // -----------------------------------------------
            // 同意取消
            // -----------------------------------------------
            order.status = QStringLiteral("已取消");
            order.finishTime = QDateTime::currentDateTime();
            order.amount = 0.0;

            request.status = QStringLiteral("已同意");

            return true;
        }

        return false;
    }

    return false;
}


// ============================================================
// 管理员拒绝取消申请
// ============================================================
bool OrderManager::rejectCancelRequest(const QString &requestId)
{
    for (OrderCancelRequest &request : cancelRequests)
    {
        if (request.requestId != requestId)
            continue;

        // ----------------------------------------------------
        // 只有待处理申请才能拒绝
        // ----------------------------------------------------
        if (request.status != QStringLiteral("待处理"))
            return false;

        request.status = QStringLiteral("已拒绝");

        return true;
    }

    return false;
}


// ============================================================
// 判断某订单是否存在待处理取消申请
// ============================================================
bool OrderManager::hasPendingCancelRequest(const QString &orderId) const
{
    for (const OrderCancelRequest &request : cancelRequests)
    {
        if (request.orderId == orderId &&
            request.status == QStringLiteral("待处理"))
        {
            return true;
        }
    }

    return false;
}


// ============================================================
// 获取所有订单
// ============================================================
QList<ChargingOrder> OrderManager::getOrders() const
{
    return orders;
}


// ============================================================
// 获取指定订单
// ============================================================
ChargingOrder OrderManager::getOrder(const QString &orderId) const
{
    for (const ChargingOrder &order : orders)
    {
        if (order.orderId == orderId)
            return order;
    }

    // --------------------------------------------------------
    // 没找到时返回一个默认订单
    // --------------------------------------------------------
    return ChargingOrder();
}


// ============================================================
// 清空订单
// ============================================================
void OrderManager::clearOrders()
{
    orders.clear();
    cancelRequests.clear();
}


// ============================================================
// 创建测试订单
// ============================================================
void OrderManager::createTestOrders()
{
    orders.clear();
    cancelRequests.clear();

    // ========================================================
    // ORD001
    // 用户 U001
    // ST001 / P001
    // 已完成，收入 120
    // ========================================================
    {
        ChargingOrder order;

        order.orderId = QStringLiteral("ORD001");
        order.userId = QStringLiteral("U001");
        order.stationCode = QStringLiteral("ST001");
        order.pileCode = QStringLiteral("P001");

        order.amount = 120.0;
        order.status = QStringLiteral("已完成");

        order.createTime =
            QDateTime::currentDateTime().addDays(-2);

        order.finishTime =
            order.createTime.addSecs(60 * 60);

        orders.append(order);
    }


    // ========================================================
    // ORD002
    // 用户 U001
    // ST001 / P002
    // 已完成，收入 180
    // ========================================================
    {
        ChargingOrder order;

        order.orderId = QStringLiteral("ORD002");
        order.userId = QStringLiteral("U001");
        order.stationCode = QStringLiteral("ST001");
        order.pileCode = QStringLiteral("P002");

        order.amount = 180.0;
        order.status = QStringLiteral("已完成");

        order.createTime =
            QDateTime::currentDateTime().addDays(-1);

        order.finishTime =
            order.createTime.addSecs(90 * 60);

        orders.append(order);
    }


    // ========================================================
    // ORD003
    // 用户 U002
    // ST001 / P003
    // 进行中
    // ========================================================
    {
        ChargingOrder order;

        order.orderId = QStringLiteral("ORD003");
        order.userId = QStringLiteral("U002");
        order.stationCode = QStringLiteral("ST001");
        order.pileCode = QStringLiteral("P003");

        order.amount = 0.0;
        order.status = QStringLiteral("进行中");

        order.createTime =
            QDateTime::currentDateTime().addSecs(-2 * 60 * 60);

        order.finishTime = QDateTime();

        orders.append(order);
    }


    // ========================================================
    // ORD004
    // 用户 U002
    // ST001 / P004
    // 已取消
    // ========================================================
    {
        ChargingOrder order;

        order.orderId = QStringLiteral("ORD004");
        order.userId = QStringLiteral("U002");
        order.stationCode = QStringLiteral("ST001");
        order.pileCode = QStringLiteral("P004");

        order.amount = 0.0;
        order.status = QStringLiteral("已取消");

        order.createTime =
            QDateTime::currentDateTime().addDays(-3);

        order.finishTime =
            order.createTime.addSecs(30 * 60);

        orders.append(order);
    }


    // ========================================================
    // ORD005
    // 用户 U003
    // ST001 / P005
    // 进行中，并且已经持续 13 小时
    //
    // 用于测试“超时订单”
    // ========================================================
    {
        ChargingOrder order;

        order.orderId = QStringLiteral("ORD005");
        order.userId = QStringLiteral("U003");
        order.stationCode = QStringLiteral("ST001");
        order.pileCode = QStringLiteral("P005");

        order.amount = 0.0;
        order.status = QStringLiteral("进行中");

        order.createTime =
            QDateTime::currentDateTime().addSecs(-13 * 60 * 60);

        order.finishTime = QDateTime();

        orders.append(order);
    }
}