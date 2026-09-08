#ifndef ORDERWIDGET_H
#define ORDERWIDGET_H

#include <QWidget>
#include <QString>
#include <QtGlobal>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>


class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class QTableWidget;
class QScrollArea;
class QEvent;
class QTableWidgetItem;

class OrderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OrderWidget(QWidget *parent = nullptr);

signals:
    void backHome();

    // 订单数据发生变化
    void orderDataChanged();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:

    // =========================
    // 订单
    // =========================

    void refreshTable();
    void searchOrders();
    void showOrderDetail(int row, int column);

    // 管理员取消超时订单
    void adminCancelOrder();

    // =========================
    // 用户取消申请
    // =========================

    void approveCancelRequest();
    void rejectCancelRequest();

    // =========================
    // 图表
    // =========================

    void updateRevenueChart();

    // =========================
    // 选择框
    // =========================

    void onOrderItemChanged(
        QTableWidgetItem *item
    );

    void onCancelRequestItemChanged(
        QTableWidgetItem *item
    );

private:

    // =========================
    // UI
    // =========================

    void createUI();

    // =========================
    // 数据
    // =========================

    void updateStatistics();
    void updateRevenueRanking();

    // =========================
    // 判断选中的订单是否超过12小时
    // =========================

    bool isSelectedOrderOvertime() const;

    // =========================
    // 获取当前选中的订单
    // =========================

    QString selectedOrderId() const;

    QString selectedRequestId() const;

    // =========================
    // 时间格式化
    // =========================

    QString formatDuration(
        qint64 seconds
    ) const;

    // =========================
    // 行高亮
    // =========================

    void highlightRow(
        QTableWidget *targetTable,
        int row
    );

private:

    // ========================================================
    // 统计
    // ========================================================

    QLabel *totalOrderLabel;
    QLabel *chargingOrderLabel;
    QLabel *finishedOrderLabel;
    QLabel *cancelledOrderLabel;

    QLabel *todayRevenueLabel;
    QLabel *monthRevenueLabel;
    QLabel *totalRevenueLabel;
    QLabel *todayOrderLabel;


    // ========================================================
    // 查询
    // ========================================================

    QLineEdit *orderIdEdit;

    QComboBox *stationCombo;
    QComboBox *statusCombo;

    QPushButton *searchButton;
    QPushButton *refreshButton;
    QPushButton *backButton;

    // 管理员取消
    QPushButton *cancelOrderButton;


    // ========================================================
    // 收入趋势
    // ========================================================

    QComboBox *revenueRangeCombo;

    QChartView *revenueChartView;

    QLineSeries *revenueSeries;


    // ========================================================
    // 电站收入排名
    // ========================================================

    QTableWidget *revenueRankingTable;


    // ========================================================
    // 订单表格
    // ========================================================

    QTableWidget *table;


    // ========================================================
    // 用户取消申请
    // ========================================================

    QTableWidget *cancelRequestTable;

    QPushButton *approveCancelButton;
    QPushButton *rejectCancelButton;


    // ========================================================
    // 页面滚动
    // ========================================================

    QScrollArea *scrollArea;


    // ========================================================
    // 内部状态
    // ========================================================

    bool updatingOrderItems = false;
    bool updatingRequestItems = false;
};

#endif