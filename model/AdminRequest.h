#ifndef ADMINREQUEST_H
#define ADMINREQUEST_H

#include <QString>


class AdminRequest
{
public:
    enum Status
    {
        Pending,
        Approved,
        Rejected
    };

    int id=0;

    QString account;

    QString password;

    QString applyRole;

    QString applyTime;

    Status status;

};


#endif // ADMINREQUEST_H