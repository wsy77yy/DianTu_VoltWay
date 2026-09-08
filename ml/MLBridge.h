#ifndef MLBRIDGE_H
#define MLBRIDGE_H

#include <QString>
#include <QVector>
#include <QDateTime>

#include <memory>

// ============================================================
// Qt 的 slots 宏与 Python 3.13 C API 中的 slots 字段冲突
// 在引入 pybind11 之前暂时取消 Qt slots 宏
// ============================================================
#ifdef slots
#undef slots
#define VOLTWAY_RESTORE_QT_SLOTS
#endif

#include <pybind11/embed.h>

#ifdef VOLTWAY_RESTORE_QT_SLOTS
#define slots Q_SLOTS
#undef VOLTWAY_RESTORE_QT_SLOTS
#endif

namespace py = pybind11;


// ============================================================
// ML历史数据
//
// 严格12维
// ============================================================
struct MLHistoryPoint
{
    double occupancy = 0.0;
    double duration = 0.0;
    double volume = 0.0;
    double payment = 0.0;

    double temperature = 0.0;
    double humidity = 0.0;
    double rainfall = 0.0;

    double hourSin = 0.0;
    double hourCos = 0.0;

    double daySin = 0.0;
    double dayCos = 0.0;

    double isHoliday = 0.0;
};


// ============================================================
// ML预测结果
// ============================================================
struct MLPredictionResult
{
    int stationId = 0;
    int chargeCount = 0;

    QString historyEndTime;
    QString predictionStartTime;
    QString predictionEndTime;

    QVector<QString> timestamps;

    QVector<double> occupancy;
    QVector<double> volume;

    QVector<int> idlePiles;

    double load_1h = 0.0;
    double load_6h = 0.0;
    double load_24h = 0.0;

    double avg_load_1h = 0.0;
    double avg_load_6h = 0.0;
    double avg_load_24h = 0.0;

    QString peakTime;

    double peakOccupancy = 0.0;
    double peakVolume = 0.0;

    int peakIdlePiles = 0;

    bool valid = false;

    QString errorMessage;
};


// ============================================================
// MLBridge
// ============================================================
class MLBridge
{
public:

    MLBridge();

    ~MLBridge();


    bool initialize(
        const QString &mlPath,
        const QString &modelPath,
        const QString &xScalerPath,
        const QString &yScalerPath
    );


    bool predict(
        int stationId,
        const QVector<MLHistoryPoint> &history,
        const QDateTime &historyEndTime,
        int chargeCount,
        MLPredictionResult &result
    );


    bool isInitialized() const;

    QString lastError() const;


private:

    std::unique_ptr<py::scoped_interpreter> m_interpreter;

    py::object m_service;

    bool m_initialized;

    QString m_lastError;
};

#endif // MLBRIDGE_H