#ifndef DATAMANAGER_H
#define DATAMANAGER_H


#include <QList>
#include <QVector>
#include <QString>


#include "../model/Admin.h"
#include "../model/AdminRequest.h"
#include "../model/User.h"
#include "../model/Station.h"
#include "../model/Pile.h"
#include "../model/Order.h"
#include "../model/Warning.h"
#include "../model/AdminRequest.h"

struct RevenueData
{
    QString date;
    double amount;
};


class DataManager
{

public:

    // 获取单例对象
    static DataManager& instance();

    // 初始化测试数据
    void initData();

    QList<Admin> admins;

    QList<AdminRequest> adminRequests;

    QList<User> users;

    QList<Station> stations;

    QList<Pile> piles;

    QList<Order> orders;

    QList<Warning> warnings;

    QVector<RevenueData> revenue7Days();

    QVector<RevenueData> revenue30Days();

    double todayRevenue();

    double monthRevenue();

    double totalRevenue();

private:

    DataManager();

    DataManager(const DataManager&) = delete;

    DataManager& operator=(const DataManager&) = delete;

};



#endif // DATAMANAGER_H