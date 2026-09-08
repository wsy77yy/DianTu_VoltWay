#ifndef PILEMANAGER_H
#define PILEMANAGER_H

#include <QList>
#include <QString>

// =========================
// 充电桩
// =========================
struct ChargingPile
{
    QString code;          // 电桩编号
    QString stationCode;   // 所属电站编号
    QString stationName;   // 所属电站名称

    QString type;          // 快充 / 慢充
    double power = 0.0;    // 功率 kW

    QString status;        // 在用 / 闲置 / 故障 / 断电

    int chargingCount = 0;       // 累计充电次数
    int chargingMinutes = 0;     // 累计充电时长

    bool online = true;          // 是否在线
};


// =========================
// 电桩管理器
// =========================
class PileManager
{
public:

    static PileManager& instance();

    // 获取所有电桩
    QList<ChargingPile> getPiles() const;

    // 根据电站编号获取电桩
    QList<ChargingPile> getPilesByStation(
        const QString &stationCode
    ) const;

    // 新增电桩
    void addPile(
        const ChargingPile &pile
    );

    // 删除电桩
    void removePile(int index);

    // 更新电桩
    void updatePile(
        int index,
        const ChargingPile &pile
    );

    // 根据编号查找电桩
    int findPileIndex(
        const QString &pileCode
    ) const;

    // 远程重启
    bool restartPile(
        const QString &pileCode
    );

    // 紧急断电
    bool emergencyShutdown(
        const QString &pileCode
    );

    // 修改电桩状态
    bool setPileStatus(
        const QString &pileCode,
        const QString &status
    );

    // 清空所有电桩
    void clearPiles();

private:

    PileManager();

    QList<ChargingPile> piles;
};

#endif