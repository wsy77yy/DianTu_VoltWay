#include "AdminManageService.h"

AdminManageService&
AdminManageService::instance()
{

    static AdminManageService service;

    return service;

}





bool AdminManageService::registerAdmin(
    QString account,
    QString password,
    QString role
)
{


    AdminRequest request;


    request.id =
        DataManager::instance()
        .adminRequests.size()+1;


    request.account=account;


    request.password=password;


    request.applyRole=role;


    request.status = AdminRequest::Pending;


    request.applyTime=
        "2026-09-04";



    DataManager::instance()
    .adminRequests
    .append(request);



    return true;

}






QList<AdminRequest>
AdminManageService::pendingRequests()
{


    QList<AdminRequest> list;



    for(auto r :
        DataManager::instance()
        .adminRequests)
    {


        if(r.status==AdminRequest::Pending)
        {

            list.append(r);

        }

    }


    return list;

}






bool AdminManageService::approve(int id)
{


    auto &requests =
        DataManager::instance()
        .adminRequests;



    for(auto &r:requests)
    {

        if(r.id==id)
        {


            r.status = AdminRequest::Approved;


            Admin admin;


            admin.id =
                DataManager::instance()
                .admins.size()+1;


            admin.account=r.account;


            admin.password=r.password;


            admin.enabled=true;



            DataManager::instance()
            .admins
            .append(admin);



            return true;

        }

    }


    return false;

}






bool AdminManageService::reject(int id)
{


    auto &requests =
        DataManager::instance()
        .adminRequests;



    for(auto &r:requests)
    {

        if(r.id==id)
        {

            r.status = AdminRequest::Rejected;
            return true;
        }
    }

    return false;

}