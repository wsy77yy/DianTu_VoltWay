#include "DataManager.h"



DataManager& DataManager::instance()
{
    static DataManager manager;

    return manager;
}



DataManager::DataManager()
{
    // DataManager 是单例，创建时初始化一次内置测试数据。
    initData();
}



void DataManager::initData()
{

    // 防止重复初始化导致管理员、用户等数据重复添加。
    if (!admins.isEmpty())
    {
        return;
    }

    /*
        初始化管理员账号
        默认：
        admin
        123456

    */

    Admin admin;

    admin.id = 1;

    admin.account = "admin";

    admin.password = "123456";

    admin.role = Admin::SuperAdmin;

    admin.enabled = true;

    admin.createTime = "2026-09-04";


    admins.append(admin);

    /*
        初始化用户
    */


    User user1;


    user1.id = 1;

    user1.phone = "13800138000";

    user1.nickname = "张三";

    user1.balance = 200;

    user1.registerTime = "2026-08-01";

    user1.frozen = false;

    user1.online = true;

    user1.totalSpend = 800;



    users.append(user1);




    /*
        初始化充电站
    */


    Station station1;


    station1.id = 1;


    station1.name =
            "浦东智慧能源站";


    station1.city =
            "上海";


    station1.district =
            "浦东新区";


    station1.address =
            "世纪大道100号";


    station1.longitude = 121.48;


    station1.latitude = 31.23;


    station1.pileCount = 3;


    station1.onlineRate = 100;


    station1.monthRevenue = 35000;


    station1.monthOrders = 230;


    station1.totalRevenue = 500000;



    stations.append(station1);




    /*
        初始化电桩
    */


    Pile pile1;


    pile1.id = 1;


    pile1.stationId = 1;


    pile1.number =
            "P001";


    pile1.type =
            Pile::FastCharge;


    pile1.power = 120;


    pile1.status =
            Pile::Idle;


    pile1.chargeCount = 350;


    pile1.chargeHours = 420;


    pile1.online = true;



    piles.append(pile1);



    /*
        更多测试数据后续继续增加

    */


}

QVector<RevenueData> DataManager::revenue7Days()
{

    QVector<RevenueData> list;


    list.append({"09-01",1200});
    list.append({"09-02",1800});
    list.append({"09-03",1500});
    list.append({"09-04",2300});
    list.append({"09-05",2600});
    list.append({"09-06",2100});
    list.append({"09-07",3000});


    return list;

}



QVector<RevenueData> DataManager::revenue30Days()
{

    QVector<RevenueData> list;


    for(int i=1;i<=30;i++)
    {

        RevenueData data;

        data.date =
            QString("09-%1").arg(i);

        data.amount =
            1000+i*120;


        list.append(data);

    }


    return list;

}



double DataManager::todayRevenue()
{

    return 3000;

}



double DataManager::monthRevenue()
{

    return 68000;

}



double DataManager::totalRevenue()
{

    return 560000;

}