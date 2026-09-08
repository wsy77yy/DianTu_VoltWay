#include "AIManager.h"

#include <QDebug>

// ============================================================
// 构造
// ============================================================
AIManager::AIManager(
    QObject *parent
)
    : QObject(parent),
      m_initialized(false),
      m_attentionThreshold(0.70),
      m_warningThreshold(0.85),
      m_criticalThreshold(0.95)
{
}

// ============================================================
// 析构
// ============================================================
AIManager::~AIManager()
{
}

// ============================================================
// 初始化
// ============================================================
bool AIManager::initialize(
    const QString &mlPath,
    const QString &modelPath,
    const QString &xScalerPath,
    const QString &yScalerPath
)
{
    m_lastError.clear();

    m_initialized =
        m_bridge.initialize(
            mlPath,
            modelPath,
            xScalerPath,
            yScalerPath
        );

    if (!m_initialized)
    {
        m_lastError =
            m_bridge.lastError();

        qWarning()
            << "[AIManager] Initialize failed:"
            << m_lastError;

        emit aiError(
            m_lastError
        );

        return false;
    }

    qDebug()
        << "[AIManager] ML service initialized";

    return true;
}

// ============================================================
// 是否初始化
// ============================================================
bool AIManager::isInitialized() const
{
    return m_initialized;
}

// ============================================================
// 错误信息
// ============================================================
QString AIManager::lastError() const
{
    return m_lastError;
}

// ============================================================
// 执行预测
// ============================================================
bool AIManager::predictStation(
    int stationId,
    const QVector<MLHistoryPoint> &history,
    const QDateTime &historyEndTime,
    int chargeCount
)
{
    if (!m_initialized)
    {
        m_lastError =
            "AIManager 尚未初始化";

        emit aiError(
            m_lastError
        );

        return false;
    }

    if (history.size() != 24)
    {
        m_lastError =
            QString(
                "ML输入历史数据必须为24条，当前为%1条"
            )
            .arg(history.size());

        emit aiError(
            m_lastError
        );

        return false;
    }

    MLPredictionResult result;

    bool ok =
        m_bridge.predict(
            stationId,
            history,
            historyEndTime,
            chargeCount,
            result
        );

    if (!ok)
    {
        m_lastError =
            m_bridge.lastError();

        emit aiError(
            m_lastError
        );

        return false;
    }

    if (!result.valid)
    {
        m_lastError =
            result.errorMessage;

        emit aiError(
            m_lastError
        );

        return false;
    }

    // 保存最新预测
    m_latestPrediction =
        result;

    // 重新生成预警
    analyzePrediction(
        m_latestPrediction
    );

    emit predictionFinished(
        m_latestPrediction
    );

    return true;
}

// ============================================================
// 获取最新预测
// ============================================================
const MLPredictionResult &
AIManager::latestPrediction() const
{
    return m_latestPrediction;
}

// ============================================================
// 是否存在预测
// ============================================================
bool AIManager::hasPrediction() const
{
    return m_latestPrediction.valid;
}

// ============================================================
// 获取预警
// ============================================================
const QList<AIWarning> &
AIManager::warnings() const
{
    return m_warnings;
}

// ============================================================
// 清除预警
// ============================================================
void AIManager::clearWarnings()
{
    m_warnings.clear();
}

// ============================================================
// 根据预测结果分析预警
// ============================================================
void AIManager::analyzePrediction(
    const MLPredictionResult &result
)
{
    m_warnings.clear();

    if (!result.valid)
        return;

    // --------------------------------------------------------
    // 使用预测峰值作为主要预警依据
    // --------------------------------------------------------
    AIWarningLevel level =
        warningLevel(
            result.peakOccupancy
        );

    if (level ==
        AIWarningLevel::Normal)
    {
        return;
    }

    AIWarning warning;

    warning.stationId =
        result.stationId;

    warning.level =
        level;

    warning.peakTime =
        result.peakTime;

    warning.occupancy =
        result.peakOccupancy;

    warning.idlePiles =
        result.peakIdlePiles;

    warning.peakVolume =
        result.peakVolume;

    warning.createdAt =
        QDateTime::currentDateTime();

    // --------------------------------------------------------
    // 预警文本
    // --------------------------------------------------------
    switch (level)
    {
    case AIWarningLevel::Attention:

        warning.title =
            "AI负载注意";

        warning.message =
            QString(
                "预计电站 %1 在 %2 "
                "占用率达到 %3%，"
                "预计剩余 %4 个空闲电桩。"
            )
            .arg(result.stationId)
            .arg(result.peakTime)
            .arg(
                result.peakOccupancy * 100.0,
                0,
                'f',
                1
            )
            .arg(
                result.peakIdlePiles
            );

        break;

    case AIWarningLevel::Warning:

        warning.title =
            "AI高负荷预警";

        warning.message =
            QString(
                "预计电站 %1 在 %2 "
                "占用率达到 %3%，"
                "进入高负荷状态，"
                "预计剩余 %4 个空闲电桩。"
            )
            .arg(result.stationId)
            .arg(result.peakTime)
            .arg(
                result.peakOccupancy * 100.0,
                0,
                'f',
                1
            )
            .arg(
                result.peakIdlePiles
            );

        break;

    case AIWarningLevel::Critical:

        warning.title =
            "AI严重拥堵预警";

        warning.message =
            QString(
                "预计电站 %1 在 %2 "
                "占用率达到 %3%，"
                "可能出现严重拥堵，"
                "预计仅剩 %4 个空闲电桩。"
            )
            .arg(result.stationId)
            .arg(result.peakTime)
            .arg(
                result.peakOccupancy * 100.0,
                0,
                'f',
                1
            )
            .arg(
                result.peakIdlePiles
            );

        break;

    default:
        return;
    }

    m_warnings.append(
        warning
    );

    emit warningGenerated(
        warning
    );
}

// ============================================================
// 判断预警等级
// ============================================================
AIWarningLevel
AIManager::warningLevel(
    double occupancy
) const
{
    if (occupancy >=
        m_criticalThreshold)
    {
        return AIWarningLevel::Critical;
    }

    if (occupancy >=
        m_warningThreshold)
    {
        return AIWarningLevel::Warning;
    }

    if (occupancy >=
        m_attentionThreshold)
    {
        return AIWarningLevel::Attention;
    }

    return AIWarningLevel::Normal;
}

// ============================================================
// 阈值设置
// ============================================================
void AIManager::setAttentionThreshold(
    double value
)
{
    m_attentionThreshold =
        qBound(
            0.0,
            value,
            1.0
        );
}

void AIManager::setWarningThreshold(
    double value
)
{
    m_warningThreshold =
        qBound(
            0.0,
            value,
            1.0
        );
}

void AIManager::setCriticalThreshold(
    double value
)
{
    m_criticalThreshold =
        qBound(
            0.0,
            value,
            1.0
        );
}

double
AIManager::attentionThreshold() const
{
    return m_attentionThreshold;
}

double
AIManager::warningThreshold() const
{
    return m_warningThreshold;
}

double
AIManager::criticalThreshold() const
{
    return m_criticalThreshold;
}