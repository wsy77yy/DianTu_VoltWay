#include "PileManager.h"


// =========================
// 单例
// =========================
PileManager& PileManager::instance()
{
    static PileManager manager;

    return manager;
}


// =========================
// 构造函数
// 初始化模拟电桩数据
// =========================
PileManager::PileManager()
{
    // ==================================================
    // ST001 阳光充电站
    // ==================================================

    ChargingPile p1;

    p1.code = "P001";
    p1.stationCode = "ST001";
    p1.stationName = "阳光充电站";

    p1.type = "快充";
    p1.power = 120.0;

    p1.status = "在用";

    p1.chargingCount = 356;
    p1.chargingMinutes = 53520;

    p1.online = true;

    piles.append(p1);


    ChargingPile p2;

    p2.code = "P002";
    p2.stationCode = "ST001";
    p2.stationName = "阳光充电站";

    p2.type = "快充";
    p2.power = 120.0;

    p2.status = "闲置";

    p2.chargingCount = 281;
    p2.chargingMinutes = 43860;

    p2.online = true;

    piles.append(p2);


    ChargingPile p3;

    p3.code = "P003";
    p3.stationCode = "ST001";
    p3.stationName = "阳光充电站";

    p3.type = "慢充";
    p3.power = 60.0;

    p3.status = "故障";

    p3.chargingCount = 194;
    p3.chargingMinutes = 30600;

    p3.online = false;

    piles.append(p3);


    ChargingPile p4;

    p4.code = "P004";
    p4.stationCode = "ST001";
    p4.stationName = "阳光充电站";

    p4.type = "快充";
    p4.power = 120.0;

    p4.status = "在用";

    p4.chargingCount = 420;
    p4.chargingMinutes = 64800;

    p4.online = true;

    piles.append(p4);


    ChargingPile p5;

    p5.code = "P005";
    p5.stationCode = "ST001";
    p5.stationName = "阳光充电站";

    p5.type = "慢充";
    p5.power = 60.0;

    p5.status = "闲置";

    p5.chargingCount = 165;
    p5.chargingMinutes = 25200;

    p5.online = true;

    piles.append(p5);


    // ==================================================
    // ST002 中心充电站
    // ==================================================

    ChargingPile p6;

    p6.code = "P006";
    p6.stationCode = "ST002";
    p6.stationName = "中心充电站";

    p6.type = "快充";
    p6.power = 180.0;

    p6.status = "在用";

    p6.chargingCount = 512;
    p6.chargingMinutes = 76800;

    p6.online = true;

    piles.append(p6);


    ChargingPile p7;

    p7.code = "P007";
    p7.stationCode = "ST002";
    p7.stationName = "中心充电站";

    p7.type = "快充";
    p7.power = 180.0;

    p7.status = "闲置";

    p7.chargingCount = 388;
    p7.chargingMinutes = 58200;

    p7.online = true;

    piles.append(p7);


    ChargingPile p8;

    p8.code = "P008";
    p8.stationCode = "ST002";
    p8.stationName = "中心充电站";

    p8.type = "慢充";
    p8.power = 60.0;

    p8.status = "闲置";

    p8.chargingCount = 245;
    p8.chargingMinutes = 36900;

    p8.online = true;

    piles.append(p8);


    ChargingPile p9;

    p9.code = "P009";
    p9.stationCode = "ST002";
    p9.stationName = "中心充电站";

    p9.type = "快充";
    p9.power = 180.0;

    p9.status = "故障";

    p9.chargingCount = 126;
    p9.chargingMinutes = 18900;

    p9.online = false;

    piles.append(p9);


    ChargingPile p10;

    p10.code = "P010";
    p10.stationCode = "ST002";
    p10.stationName = "中心充电站";

    p10.type = "慢充";
    p10.power = 60.0;

    p10.status = "闲置";

    p10.chargingCount = 302;
    p10.chargingMinutes = 45300;

    p10.online = true;

    piles.append(p10);
}


// =========================
// 获取全部电桩
// =========================
QList<ChargingPile> PileManager::getPiles() const
{
    return piles;
}


// =========================
// 根据电站编号获取电桩
// =========================
QList<ChargingPile> PileManager::getPilesByStation(
    const QString &stationCode
) const
{
    QList<ChargingPile> result;

    for (const ChargingPile &pile : piles)
    {
        if (pile.stationCode == stationCode)
        {
            result.append(pile);
        }
    }

    return result;
}


// =========================
// 新增电桩
// =========================
void PileManager::addPile(
    const ChargingPile &pile
)
{
    piles.append(pile);
}


// =========================
// 删除电桩
// =========================
void PileManager::removePile(
    int index
)
{
    if (index >= 0 &&
        index < piles.size())
    {
        piles.removeAt(index);
    }
}


// =========================
// 更新电桩
// =========================
void PileManager::updatePile(
    int index,
    const ChargingPile &pile
)
{
    if (index >= 0 &&
        index < piles.size())
    {
        piles[index] = pile;
    }
}


// =========================
// 查找电桩
// =========================
int PileManager::findPileIndex(
    const QString &pileCode
) const
{
    for (int i = 0; i < piles.size(); ++i)
    {
        if (piles[i].code == pileCode)
        {
            return i;
        }
    }

    return -1;
}


// =========================
// 远程重启
// =========================
bool PileManager::restartPile(
    const QString &pileCode
)
{
    int index = findPileIndex(pileCode);

    if (index < 0)
    {
        return false;
    }

    ChargingPile &pile = piles[index];

    // 模拟远程重启
    // 故障电桩重启后恢复为空闲状态
    pile.status = "闲置";
    pile.online = true;

    return true;
}


// =========================
// 紧急断电
// =========================
bool PileManager::emergencyShutdown(
    const QString &pileCode
)
{
    int index = findPileIndex(pileCode);

    if (index < 0)
    {
        return false;
    }

    ChargingPile &pile = piles[index];

    // 模拟发送紧急断电指令
    pile.status = "断电";
    pile.online = false;

    return true;
}


// =========================
// 修改电桩状态
// =========================
bool PileManager::setPileStatus(
    const QString &pileCode,
    const QString &status
)
{
    int index = findPileIndex(pileCode);

    if (index < 0)
    {
        return false;
    }

    ChargingPile &pile = piles[index];

    pile.status = status;

    // 故障、断电时视为离线
    if (status == "故障" ||
        status == "断电")
    {
        pile.online = false;
    }
    else
    {
        pile.online = true;
    }

    return true;
}


// =========================
// 清空所有电桩
// =========================
void PileManager::clearPiles()
{
    piles.clear();
}