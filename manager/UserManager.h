#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QList>
#include <QString>
#include <QDateTime>

#include "OrderManager.h"


// ============================================================
// 用户
// ============================================================

struct ManagedUser
{
    QString userId;
    QString phone;
    QString nickname;

    double balance = 0.0;

    QDateTime registerTime;

    // 正常 / 冻结
    QString status;
};


// ============================================================
// 用户管理器
// ============================================================

class UserManager
{
public:

    static UserManager& instance();


    // ========================================================
    // 用户查询
    // ========================================================

    QList<ManagedUser> getUsers() const;


    // 根据用户ID获取用户
    ManagedUser getUser(
        const QString &userId
    ) const;


    // 根据手机号模糊搜索
    QList<ManagedUser> searchUsersByPhone(
        const QString &keyword
    ) const;


    // ========================================================
    // 用户管理
    // ========================================================

    bool freezeUser(
        const QString &userId
    );

    bool unfreezeUser(
        const QString &userId
    );

    bool toggleUserStatus(
        const QString &userId
    );


    // ========================================================
    // 用户订单
    // ========================================================

    QList<ChargingOrder> getUserOrders(
        const QString &userId
    ) const;


    // ========================================================
    // 用户订单代结算
    // ========================================================

    // 判断订单是否允许管理员代结算
    bool canAdminSettleOrder(
        const QString &orderId
    ) const;


    // 管理员代结算订单
    bool adminSettleOrder(
        const QString &orderId,
        double amount
    );


    // ========================================================
    // 清空
    // ========================================================

    void clearUsers();


private:

    UserManager();

    void createTestUsers();


private:

    QList<ManagedUser> users;
};

#endif