#include "AuthService.h"



AuthService::AuthService()
{

}



AuthService& AuthService::instance()
{

    static AuthService service;

    return service;

}





bool AuthService::login(
    const QString& account,
    const QString& password
)
{


    QList<Admin>& admins =
        DataManager::instance().admins;



    for(Admin admin : admins)
    {

        if(admin.account == account
           &&
           admin.password == password
           &&
           admin.enabled)
        {

            loginAdmin = admin;

            return true;

        }


    }



    return false;


}





Admin AuthService::currentAdmin()
{

    return loginAdmin;

}