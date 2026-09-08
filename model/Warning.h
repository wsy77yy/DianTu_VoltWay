#ifndef WARNING_H
#define WARNING_H


#include <QString>


class Warning
{


public:


    enum Level
    {
        Normal,
        High,
        Serious
    };


    int id;

    int pileId;

    int stationId;

    QString type;

    Level level;

    QString message;

    QString suggestion;

    bool handled;

    QString createTime;

};


#endif