QT += core gui widgets charts network

CONFIG += c++17

TEMPLATE = app
TARGET = administrator

# ============================================================
# Python + pybind11
# Cross-platform configuration
# ============================================================

win32 {
    # Windows
    PYTHON_ROOT = C:/Users/shiyi/anaconda3
    PYBIND11_ROOT = C:/Users/shiyi/anaconda3/Lib/site-packages/pybind11

    INCLUDEPATH += \
        $$PYBIND11_ROOT/include \
        $$PYTHON_ROOT/include

    LIBS += -L$$PYTHON_ROOT/libs -lpython313
}

unix:!android {
    # Linux
    PYTHON_INCLUDE = /usr/include/python3.10
    PYBIND11_INCLUDE = /home/bit/.local/lib/python3.10/site-packages/pybind11/include

    INCLUDEPATH += \
        $$PYTHON_INCLUDE \
        $$PYBIND11_INCLUDE

    LIBS += -L/usr/lib/x86_64-linux-gnu -lpython3.10
}


RESOURCES += resources.qrc \
    resources.qrc

SOURCES += \
    main.cpp \
    MainWindow.cpp\
    database/DataManager.cpp\
    service/AuthService.cpp\
    service/AdminManageService.cpp\
    ui/LoginWidget.cpp\
    ui/RegisterWidget.cpp\
    ui/AdminApprovalWidget.cpp\
    ui/RevenueWidget.cpp\
    ui/DashboardWidget.cpp\
    ui/StationWidget.cpp\
    ui/OrderWidget.cpp\
    ui/PileManagementWidget.cpp\
    ui/UserManagementWidget.cpp\
    manager/StationManager.cpp\
    manager/RevenueManager.cpp\
    manager/OrderManager.cpp\
    manager/PileManager.cpp\
    manager/UserManager.cpp\
    dialog/StationAddDialog.cpp\
    server/ApiServer.cpp \
    ui/aiwarningwidget.cpp



HEADERS += \
    MainWindow.h \
    model/Admin.h \
    model/AdminRequest.h \
    model/User.h \
    model/Station.h \
    model/Pile.h \
    model/Order.h \
    model/Warning.h\
    database/DataManager.h\
    service/AuthService.h\
    service/AdminManageService.h\
    ui/LoginWidget.h \
    ui/RegisterWidget.h\
    ui/AdminApprovalWidget.h\
    ui/RevenueWidget.h\
    ui/DashboardWidget.h\
    ui/StationWidget.h\
    ui/OrderWidget.h\
    ui/PileManagementWidget.h\
    ui/UserManagementWidget.h\
    dialog/StationAddDialog.h \
    manager/StationManager.h\
    manager/OrderManager.h\
    manager/PileManager.h\
    manager/UserManager.h\
    server/ApiServer.h \
    ui/aiwarningwidget.h
