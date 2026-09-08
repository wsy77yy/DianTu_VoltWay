#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "ui/RevenueWidget.h"
#include "ui/OrderWidget.h"
#include "ui/DashboardWidget.h"
#include "ui/StationWidget.h"
#include "ui/PileManagementWidget.h"
#include "ui/UserManagementWidget.h"

class QStackedWidget;
class QWidget;
class QPushButton;
class QLabel;
class QPropertyAnimation;
class AIWarningWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void logoutRequested();

public:

    // ============================================================
    // 构造函数
    // account：
    // 当前登录的管理员账号
    //
    // 例如：
    // MainWindow("admin");
    // ============================================================

    explicit MainWindow(
        const QString &account,
        QWidget *parent = nullptr
    );


private:

    // ============================================================
    // UI 创建
    // ============================================================

    void createHeader();

    void createNavigation();

    void createPages();

    void createStyle();


    // ============================================================
    // Sidebar
    // ============================================================

    void toggleSidebar();

    void updateSidebarTexts(
        bool expanded
    );


    // ============================================================
    // 页面切换
    // ============================================================

    void showPage( QWidget *page);

    void setActiveButton( QPushButton *button );


    // ============================================================
    // 页面容器
    // ============================================================

    QStackedWidget *stack;


    // ============================================================
    // 页面
    // ============================================================

    DashboardWidget *dashboardWidget;

    RevenueWidget *revenueWidget;

    OrderWidget *orderWidget;

    StationWidget *stationWidget;

    PileManagementWidget *pileManagementWidget;

    UserManagementWidget *userManagementWidget;

    QWidget *approvalWidget;

    AIWarningWidget *aiWarningWidget;
    // ============================================================
    // Sidebar
    // ============================================================

    QWidget *sidebar;

    QPushButton *sidebarToggleButton;

    QPushButton *dashboardButton;

    QPushButton *approvalButton;

    QPushButton *stationButton;

    QPushButton *pileManagementButton;

    QPushButton *userManagementButton;

    QPushButton *orderButton;

    QPushButton *revenueButton;

    QPushButton *aiWarningButton;

    QPushButton *logoutButton;

    // ============================================================
    // Header
    // ============================================================

    QLabel *workTitle;

    QLabel *systemTitle;

    QLabel *accountLabel;


    // ============================================================
    // 当前登录账号
    //
    // 例如：
    // admin
    // ============================================================

    QString loginAccount;

    // ========================================================
    // AI
    // ========================================================

    // ============================================================
    // Sidebar 动画
    // ============================================================

    QPropertyAnimation *sidebarAnimation;

    bool sidebarExpanded;
};

#endif

