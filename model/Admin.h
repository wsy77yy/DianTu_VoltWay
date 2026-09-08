#ifndef ADMIN_H
#define ADMIN_H

#include <QString>


class Admin
{

public:


    enum Role
    {
        SuperAdmin,
        OperatorAdmin,
        AreaAdmin
    };


    int id = 0;


    QString account;


    QString password;


    Role role = SuperAdmin;


    bool enabled = false;


    QString createTime;


};


#endif // ADMIN_H