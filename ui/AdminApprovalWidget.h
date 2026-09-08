#ifndef ADMINAPPROVALWIDGET_H
#define ADMINAPPROVALWIDGET_H

#include <QWidget>

class QPushButton;
class QTableWidget;

class AdminApprovalWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AdminApprovalWidget(QWidget *parent = nullptr);

signals:
    // 返回后台首页
    void backClicked();

private slots:
    // 加载待审批申请
    void loadRequests();

private:
    QTableWidget *table;

    // 返回首页
    QPushButton *backButton;
};

#endif
