#include "RevenueManager.h"

RevenueManager& RevenueManager::instance()
{
    static RevenueManager manager;
    return manager;
}

RevenueManager::RevenueManager()
{
    QDate today = QDate::currentDate();

    addRevenue(4200.0, today.addDays(-6));
    addRevenue(5100.0, today.addDays(-5));
    addRevenue(3800.0, today.addDays(-4));
    addRevenue(6200.0, today.addDays(-3));
    addRevenue(5700.0, today.addDays(-2));
    addRevenue(7300.0, today.addDays(-1));
    addRevenue(8600.0, today);
}

void RevenueManager::addRevenue(
        double amount,
        const QDate &date)
{
    if (amount <= 0)
        return;

    RevenueRecord record;

    record.date = date;
    record.amount = amount;

    records.append(record);
}

QList<RevenueRecord> RevenueManager::getRecords() const
{
    return records;
}

double RevenueManager::getTodayRevenue() const
{
    return getDailyRevenue(QDate::currentDate());
}

double RevenueManager::getMonthRevenue() const
{
    QDate today = QDate::currentDate();

    double total = 0.0;

    for (const RevenueRecord &record : records)
    {
        if (record.date.year() == today.year() &&
            record.date.month() == today.month())
        {
            total += record.amount;
        }
    }

    return total;
}

double RevenueManager::getDailyRevenue(
        const QDate &date) const
{
    double total = 0.0;

    for (const RevenueRecord &record : records)
    {
        if (record.date == date)
            total += record.amount;
    }

    return total;
}

QList<double> RevenueManager::getLast7DaysRevenue() const
{
    QList<double> result;

    QDate today = QDate::currentDate();

    for (int i = 6; i >= 0; --i)
    {
        QDate date = today.addDays(-i);

        result.append(
            getDailyRevenue(date)
        );
    }

    return result;
}

void RevenueManager::clearRecords()
{
    records.clear();
}

