#include "StationManager.h"


StationManager& StationManager::instance()
{
    static StationManager manager;

    return manager;
}


// =========================
// 构造函数
// =========================
StationManager::StationManager()
{
    // ==================================================
    // ST001
    // ==================================================
    ManagedStation s1;

    s1.code = "ST001";
    s1.name = "阳光充电站";
    s1.address = "科技园A区";
    s1.status = "启用";

    // 区域
    s1.city = "深圳市";
    s1.district = "南山区";

    s1.total = 20;
    s1.available = 18;
    s1.fault = 2;

    s1.revenue = 12800.0;

    stations.append(s1);


    // ==================================================
    // ST002
    // ==================================================
    ManagedStation s2;

    s2.code = "ST002";
    s2.name = "中心充电站";
    s2.address = "市中心";
    s2.status = "暂停";

    // 区域
    s2.city = "深圳市";
    s2.district = "福田区";

    s2.total = 30;
    s2.available = 30;
    s2.fault = 0;

    s2.revenue = 8600.0;

    stations.append(s2);
}


// =========================
// 获取全部电站
// =========================
QList<ManagedStation> StationManager::getStations() const
{
    return stations;
}


// =========================
// 添加电站
// =========================
void StationManager::addStation(
    const ManagedStation &station
)
{
    stations.append(station);
}


// =========================
// 删除电站
// =========================
void StationManager::removeStation(
    int index
)
{
    if (index >= 0 &&
        index < stations.size())
    {
        stations.removeAt(index);
    }
}


// =========================
// 修改电站
// =========================
void StationManager::updateStation(
    int index,
    const ManagedStation &station
)
{
    if (index >= 0 &&
        index < stations.size())
    {
        stations[index] = station;
    }
}


// =========================
// 清空电站
// =========================
void StationManager::clearStations()
{
    stations.clear();
}