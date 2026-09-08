#ifndef PILE_H
#define PILE_H


#include <QString>


class Pile
{


public:


    enum Status
    {
        Idle,       // 空闲
        Charging,   // 使用中
        Fault       // 故障
    };


    enum Type
    {
        FastCharge,
        SlowCharge
    };



    int id;


    int stationId;


    QString number;


    Type type;


    double power;


    Status status;


    int chargeCount;


    double chargeHours;


    bool online;



};


#endif