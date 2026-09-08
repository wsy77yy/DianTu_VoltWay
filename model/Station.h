#ifndef STATION_H
#define STATION_H


#include <QString>


class Station
{


public:


    int id;


    QString name;


    QString city;


    QString district;


    QString address;


    double longitude;


    double latitude;


    int pileCount;


    double onlineRate;


    double monthRevenue;


    int monthOrders;


    double totalRevenue;


};


#endif