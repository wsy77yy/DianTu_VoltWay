#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H


#include "../database/DataManager.h"
#include "../model/Admin.h"

#include <QString>


class AuthService
{


public:


    static AuthService& instance();



    //管理员登录
    bool login(
        const QString& account,
        const QString& password
    );


    //当前登录管理员

    Admin currentAdmin();



private:


    AuthService();


    Admin loginAdmin;



};



#endif