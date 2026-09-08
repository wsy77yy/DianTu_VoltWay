#ifndef AIMANAGER_H
#define AIMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QList>
#include <QString>

#include "MLBridge.h"

// ============================================================
// AI 预警等级
// ============================================================
enum class AIWarningLevel
{
    Normal = 0,
    Attention = 1,
    Warning = 2,
    Critical = 3
};

// ============================================================
// AI 预警
// ============================================================
struct AIWarning
{
    int stationId = 0;

    AIWarningLevel level =
        AIWarningLevel::Normal;

    QString title;
    QString message;

    QString peakTime;

    double occupancy = 0.0;

    int idlePiles = 0;

    double peakVolume = 0.0;

    QDateTime createdAt;
};

// ============================================================
// AIManager
//
// 职责：
// 1. 管理 MLBridge
// 2. 调用 Python ML
// 3. 保存最新预测结果
// 4. 根据预测结果生成预警
// 5. 向 Dashboard / ApiServer 提供统一结果
// ============================================================
class AIManager : public QObject
{
    Q_OBJECT

public:

    explicit AIManager(
        QObject *parent = nullptr
    );

    ~AIManager();

    // ========================================================
    // 初始化 Python ML
    // ========================================================
    bool initialize(
        const QString &mlPath,
        const QString &modelPath,
        const QString &xScalerPath,
        const QString &yScalerPath
    );

    bool isInitialized() const;

    QString lastError() const;

    // ========================================================
    // 执行指定电站预测
    //
    // history 必须严格：
    //
    // 24 条
    //
    // 每条 12 维：
    //
    // 1  occupancy
    // 2  duration
    // 3  volume
    // 4  payment
    // 5  temperature
    // 6  humidity
    // 7  rainfall
    // 8  hour_sin
    // 9  hour_cos
    // 10 day_sin
    // 11 day_cos
    // 12 is_holiday
    // ========================================================
    bool predictStation(
        int stationId,
        const QVector<MLHistoryPoint> &history,
        const QDateTime &historyEndTime,
        int chargeCount
    );

    // ========================================================
    // 获取最新预测
    // ========================================================
    const MLPredictionResult &latestPrediction() const;

    bool hasPrediction() const;

    // ========================================================
    // 获取当前预警
    // ========================================================
    const QList<AIWarning> &warnings() const;

    // ========================================================
    // 清除预警
    // ========================================================
    void clearWarnings();

    // ========================================================
    // 预警阈值
    // ========================================================
    void setAttentionThreshold(
        double value
    );

    void setWarningThreshold(
        double value
    );

    void setCriticalThreshold(
        double value
    );

    double attentionThreshold() const;

    double warningThreshold() const;

    double criticalThreshold() const;

signals:

    // 新预测完成
    void predictionFinished(
        const MLPredictionResult &result
    );

    // 新预警
    void warningGenerated(
        const AIWarning &warning
    );

    // AI发生错误
    void aiError(
        const QString &message
    );

private:

    // ========================================================
    // 根据预测结果生成预警
    // ========================================================
    void analyzePrediction(
        const MLPredictionResult &result
    );

    // ========================================================
    // 根据占用率判断等级
    // ========================================================
    AIWarningLevel warningLevel(
        double occupancy
    ) const;

private:

    MLBridge m_bridge;

    MLPredictionResult m_latestPrediction;

    QList<AIWarning> m_warnings;

    bool m_initialized;

    QString m_lastError;

    // ========================================================
    // 默认预警阈值
    // ========================================================
    double m_attentionThreshold;

    double m_warningThreshold;

    double m_criticalThreshold;
};

#endif // AIMANAGER_H