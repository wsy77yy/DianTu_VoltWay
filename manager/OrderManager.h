#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H

#include <QList>
#include <QString>
#include <QDateTime>

// ============================================================
// 充电订单
// ============================================================
struct ChargingOrder
{
    QString orderId;
    QString userId;
    QString stationCode;
    QString pileCode;

    double amount = 0.0;

    // 进行中 / 已完成 / 已取消
    QString status;

    QDateTime createTime;

    // 进行中的订单为空
    QDateTime finishTime;

    // ========================================================
    // 获取订单持续时间（秒）
    // ========================================================
    qint64 durationSeconds() const
    {
        if (!createTime.isValid())
            return 0;

        if (finishTime.isValid())
            return createTime.secsTo(finishTime);

        return createTime.secsTo(QDateTime::currentDateTime());
    }
};


// ============================================================
// 订单取消申请
// ============================================================
struct OrderCancelRequest
{
    QString requestId;
    QString orderId;
    QString reason;

    QDateTime requestTime;

    // 待处理 / 已同意 / 已拒绝
    QString status;
};


// ============================================================
// 订单管理器
// ============================================================
class OrderManager
{
public:
    // ========================================================
    // 单例
    // ========================================================
    static OrderManager& instance();

    // ========================================================
    // 用户创建订单
    // ========================================================
    void createOrder(const QString &orderId,
                     const QString &userId,
                     const QString &stationCode,
                     const QString &pileCode);

    // ========================================================
    // 完成订单
    //
    // amount：本次充电产生的金额
    // ========================================================
    bool finishOrder(const QString &orderId,
                     double amount);

    // ========================================================
    // 用户主动取消订单
    // ========================================================
    bool cancelOrder(const QString &orderId);

    // ========================================================
    // 判断订单是否超时
    //
    // 当前规则：
    // 进行中的订单持续时间超过 12 小时
    // ========================================================
    bool isOvertimeOrder(const QString &orderId) const;

    // ========================================================
    // 管理员直接取消订单
    // ========================================================
    bool adminCancelOrder(const QString &orderId);

    // ========================================================
    // 用户申请管理员取消
    // ========================================================
    bool requestAdminCancel(const QString &orderId,
                            const QString &reason);

    // ========================================================
    // 获取所有取消申请
    // ========================================================
    QList<OrderCancelRequest> getCancelRequests() const;

    // ========================================================
    // 管理员同意取消申请
    // ========================================================
    bool approveCancelRequest(const QString &requestId);

    // ========================================================
    // 管理员拒绝取消申请
    // ========================================================
    bool rejectCancelRequest(const QString &requestId);

    // ========================================================
    // 判断某订单是否存在待处理取消申请
    // ========================================================
    bool hasPendingCancelRequest(const QString &orderId) const;

    // ========================================================
    // 获取所有订单
    // ========================================================
    QList<ChargingOrder> getOrders() const;

    // ========================================================
    // 根据订单号获取订单
    // ========================================================
    ChargingOrder getOrder(const QString &orderId) const;

    // ========================================================
    // 清空所有订单
    // ========================================================
    void clearOrders();

private:
    OrderManager();

    // ========================================================
    // 创建测试数据
    // ========================================================
    void createTestOrders();

private:
    QList<ChargingOrder> orders;
    QList<OrderCancelRequest> cancelRequests;
};

#endif // ORDERMANAGER_H