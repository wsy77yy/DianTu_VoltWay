#include "MainWindow.h"
#include "ui/LoginWidget.h"
#include "database/DataManager.h"
#include "server/ApiServer.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << "[Main] Application started";


    // ========================================================
    // 1. 初始化数据库 / 项目基础数据
    // ========================================================

    DataManager::instance().initData();

    qDebug() << "[Main] DataManager initialized";


    // ========================================================
    // 2. 启动 Web API Server
    //
    // 当前 Linux administrator 验证阶段：
    // 暂时不初始化 ML / AIManager。
    // ========================================================

    ApiServer apiServer;

    if (!apiServer.start(8766))
    {
        qWarning()
            << "[Main] ApiServer start failed!";

        qWarning()
            << "[Main] Please check whether port 8766 is occupied.";
    }
    else
    {
        qDebug()
            << "[Main] ApiServer started successfully.";

        qDebug()
            << "[Main] API address:"
            << QString("http://127.0.0.1:%1")
                   .arg(apiServer.port());
    }


    // ========================================================
    // 3. 创建登录窗口
    //
    // 注意：
    // 登录窗口不能在第一次登录后 deleteLater。
    // 因为退出管理员账号后还需要重新显示它。
    // ========================================================

    LoginWidget *login =
        new LoginWidget;


    // ========================================================
    // 4. MainWindow 指针
    // ========================================================

    MainWindow *mainWindow =
        nullptr;


    // ========================================================
    // 5. 登录成功
    // ========================================================

    QObject::connect(
        login,
        &LoginWidget::loginSuccess,
        [&a, login, &mainWindow](
            const QString &account
        )
        {
            qDebug()
                << "[Main] Login successful:"
                << account;


            // ==================================================
            // 如果之前已经存在 MainWindow
            // 先清理旧窗口
            // ==================================================

            if (mainWindow != nullptr)
            {
                mainWindow->deleteLater();
                mainWindow = nullptr;
            }


            // ==================================================
            // 创建管理员后台
            // ==================================================

            mainWindow =
                new MainWindow(
                    account
                );


            // ==================================================
            // 监听退出登录
            // ==================================================

            QObject::connect(
                mainWindow,
                &MainWindow::logoutRequested,
                [login, &mainWindow]()
                {
                    qDebug()
                        << "[Main] Logout requested";


                    // ==========================================
                    // 1. 关闭管理员后台
                    // ==========================================

                    if (mainWindow != nullptr)
                    {
                        MainWindow *oldWindow =
                            mainWindow;

                        mainWindow = nullptr;

                        oldWindow->close();

                        oldWindow->deleteLater();
                    }


                    // ==========================================
                    // 2. 重新显示登录界面
                    // ==========================================

                    login->show();

                    login->raise();

                    login->activateWindow();


                    qDebug()
                        << "[Main] Login window shown again";
                }
            );


            // ==================================================
            // 显示管理员后台
            // ==================================================

            mainWindow->show();


            // ==================================================
            // 隐藏登录窗口
            //
            // 注意：这里不能 delete
            // ==================================================

            login->hide();
        }
    );


    // ========================================================
    // 6. 显示登录界面
    // ========================================================

    login->show();

    qDebug()
        << "[Main] Login window shown";


    // ========================================================
    // 7. Qt 事件循环
    // ========================================================

    return a.exec();
}
