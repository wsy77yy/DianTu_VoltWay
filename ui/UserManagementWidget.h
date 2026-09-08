#ifndef USERMANAGEMENTWIDGET_H
#define USERMANAGEMENTWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QList>

#include "../manager/OrderManager.h"

class UserManagementWidget : public QWidget
{
public:
    explicit UserManagementWidget(
        QWidget *parent = nullptr
    );

private:
    void setupUi();

    void loadUsers();

    void refreshTable();

    QString selectedUserId() const;

    void freezeSelectedUser();

    void unfreezeSelectedUser();

    void viewSelectedUserOrders();

    void settleSelectedOrder();

private:
    QLineEdit *searchEdit = nullptr;

    QPushButton *searchButton = nullptr;

    QPushButton *resetButton = nullptr;

    QPushButton *freezeButton = nullptr;

    QPushButton *unfreezeButton = nullptr;

    QPushButton *ordersButton = nullptr;

    QTableWidget *table = nullptr;

    QLabel *summaryLabel = nullptr;
};

#endif