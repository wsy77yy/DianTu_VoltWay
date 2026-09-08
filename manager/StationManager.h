#ifndef STATIONMANAGER_H
#define STATIONMANAGER_H

#include <QList>
#include <QString>

struct ManagedStation
{
    QString code;
    QString name;
    QString address;
    QString status;

    // =========================
    // 区域信息
    // =========================
    QString city;       // 城市
    QString district;   // 区县

    int total = 0;
    int available = 0;
    int fault = 0;

    // 该电站累计收入
    double revenue = 0.0;
};


class StationManager
{
public:

    static StationManager& instance();

    QList<ManagedStation> getStations() const;

    void addStation(const ManagedStation &station);

    void removeStation(int index);

    void updateStation(
        int index,
        const ManagedStation &station
    );

    void clearStations();

private:

    StationManager();

    QList<ManagedStation> stations;
};

#endif