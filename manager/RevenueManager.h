#ifndef REVENUEMANAGER_H
#define REVENUEMANAGER_H

#include <QList>
#include <QDate>

struct RevenueRecord
{
    QDate date;
    double amount = 0.0;
};

class RevenueManager
{
public:
    static RevenueManager& instance();

    void addRevenue(
        double amount,
        const QDate &date = QDate::currentDate()
    );

    QList<RevenueRecord> getRecords() const;

    double getTodayRevenue() const;

    double getMonthRevenue() const;

    double getDailyRevenue(
        const QDate &date
    ) const;

    QList<double> getLast7DaysRevenue() const;

    void clearRecords();

private:
    RevenueManager();

    QList<RevenueRecord> records;
};

#endif

