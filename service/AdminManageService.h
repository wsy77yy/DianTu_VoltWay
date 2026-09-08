#ifndef ADMINMANAGESERVICE_H
#define ADMINMANAGESERVICE_H


#include "../database/DataManager.h"


class AdminManageService
{

public:
    static AdminManageService& instance();

    //提交注册申请
    bool registerAdmin(
        QString account,
        QString password,
        QString role
    );

    //查询待审批
    QList<AdminRequest>
    pendingRequests();

    //审批通过
    bool approve( int id);

    //拒绝
    bool reject( int id );


private:

    AdminManageService(){}
};



#endif