#ifndef ORDER_H
#define ORDER_H


#include <QString>


class Order
{


public:


    enum Status
    {
        Charging,
        Finished,
        WaitingSettlement
    };


    int id;


    int userId;


    int stationId;


    int pileId;


    QString startTime;


    QString endTime;


    double energy;


    double amount;


    Status status;



};


#endif