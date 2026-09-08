#ifndef AIWARNINGWIDGET_H
#define AIWARNINGWIDGET_H

#include <QWidget>
#include <QString>
#include <QVector>

// 前向声明
class QLabel;
class QFrame;
class QScrollArea;
class QVBoxLayout;


/*
 * =========================================================
 * AI 预警等级
 * =========================================================
 *
 * Normal     = 一般
 * Attention  = 待关注
 * Dangerous  = 危险
 * Critical   = 特别危险
 *
 * 数字越大代表风险越高。
 */
enum class AIWarningLevel
{
    Normal = 0,
    Attention = 1,
    Dangerous = 2,
    Critical = 3
};


/*
 * =========================================================
 * 单个电站的 AI 风险信息
 * =========================================================
 *
 * 每一个电站对应一条 AI 预测/预警结果。
 *
 * 后续接入 Python + PyBind11 时，
 * Python 模型可以直接生成对应的数据，
 * 再由 AIManager 传给 AIWarningWidget。
 */
struct AIStationWarning
{
    // 电站名称
    QString stationName;

    // 所属区域
    //
    // 例如：
    // 华东地区
    // 华北地区
    // 华南地区
    // 华中地区
    QString region;

    // AI 预警等级
    AIWarningLevel level;

    // 风险描述
    QString description;

    // AI 建议
    QString suggestion;

    // 预测高峰占用率
    //
    // 例如：
    // 91.5
    // 表示 91.5%
    double predictedOccupancy;
};


/*
 * =========================================================
 * AI 预警整体结果
 * =========================================================
 *
 * AIWarningResult 表示当前整个系统的 AI 分析结果。
 *
 * 后续 Python 模型可以输出：
 *
 * 当前总体预警等级
 * ↓
 * 模型运行状态
 * ↓
 * 各等级预警数量
 * ↓
 * 高峰预测
 * ↓
 * 电站风险预测
 *
 * 然后通过：
 *
 * setWarningResult(result);
 *
 * 更新整个 AI 预警页面。
 */
struct AIWarningResult
{
    /*
     * 当前系统总体预警等级
     */
    AIWarningLevel level;


    /*
     * AI 模型是否正在运行
     */
    bool modelRunning;


    /*
     * 最后一次 AI 分析时间
     *
     * 例如：
     * 2026-09-08 09:20
     */
    QString lastUpdateTime;


    /*
     * 当前预警总数量
     */
    int warningCount;


    /*
     * 各等级预警数量
     */
    int criticalCount;

    int dangerousCount;

    int attentionCount;

    int normalCount;


    /*
     * AI 预测高峰时间
     *
     * 例如：
     * 18:00 - 20:00
     */
    QString peakTime;


    /*
     * AI 预测高峰占用率
     *
     * 例如：
     * 91.5
     *
     * 实际显示时：
     * 91.5%
     */
    double peakOccupancy;


    /*
     * AI 预测高峰订单量
     *
     * 例如：
     * 186
     */
    int peakOrderVolume;


    /*
     * AI 预测高峰期间空闲充电桩数量
     */
    int predictedIdlePiles;


    /*
     * AI 总体分析说明
     */
    QString mainMessage;


    /*
     * 电站风险列表
     *
     * 每个 AIStationWarning 对应一个电站。
     */
    QVector<AIStationWarning> stationWarnings;
};


/*
 * =========================================================
 * AI 智能预警页面
 * =========================================================
 *
 * 功能：
 *
 * 1. 显示当前总体 AI 预警等级
 *
 * 2. 显示 AI 模型运行状态
 *
 * 3. 显示最后分析时间
 *
 * 4. 显示四种预警等级数量
 *
 *    特别危险
 *    危险
 *    待关注
 *    一般
 *
 * 5. 显示 AI 预测高峰时间
 *
 * 6. 显示预测高峰占用率
 *
 * 7. 显示预测高峰订单量
 *
 * 8. 显示预测空闲充电桩数量
 *
 * 9. 显示电站风险排行
 *
 * 10. 支持区域筛选
 *
 * 11. 支持预警等级筛选
 *
 * 12. 支持电站名称/区域关键词搜索
 *
 * 13. 支持重置筛选
 *
 * 14. 显示 AI 预警详细分析
 *
 *
 * 当前版本：
 *
 * 使用模拟 AI 数据。
 *
 *
 * 后续版本：
 *
 * Python 机器学习模型
 *        ↓
 *      PyBind11
 *        ↓
 *    AIManager
 *        ↓
 * AIWarningWidget
 *
 */
class AIWarningWidget : public QWidget
{
    Q_OBJECT

public:

    /*
     * 构造函数
     */
    explicit AIWarningWidget(
        QWidget *parent = nullptr
    );


    /*
     * 析构函数
     */
    ~AIWarningWidget() override;


public slots:

    /*
     * =====================================================
     * 刷新 AI 数据
     * =====================================================
     *
     * 当前版本：
     * 使用模拟数据刷新页面。
     *
     * 后续：
     * 可以从 AIManager 获取 Python 模型结果。
     */
    void refreshData();


    /*
     * =====================================================
     * 设置 AI 预测结果
     * =====================================================
     *
     * 后续 AIManager 获取 Python 模型结果后，
     * 可以直接：
     *
     * aiWarningWidget->setWarningResult(result);
     *
     */
    void setWarningResult(
        const AIWarningResult &result
    );


private:

    /*
     * =====================================================
     * UI 创建
     * =====================================================
     */

    /*
     * 创建整个页面
     */
    void createUI();


    /*
     * 创建顶部标题
     */
    void createHeader();


    /*
     * 创建当前总体预警卡片
     */
    void createCurrentWarningCard();


    /*
     * 创建 AI 模型状态卡片
     */
    void createModelStatusCard();


    /*
     * 创建预警数量统计
     */
    void createWarningStatistics();


    /*
     * 创建 AI 预测信息
     */
    void createPredictionCards();


    /*
     * 创建电站风险预警区域
     */
    void createStationWarningSection();


    /*
     * 创建 AI 预警详情
     */
    void createWarningDetailSection();


private:

    /*
     * =====================================================
     * 数据更新
     * =====================================================
     */

    /*更新当前总体预警 */
    void updateCurrentWarning();


    /*更新 AI 模型状态*/
    void updateModelStatus();


    /*更新预警数量统计*/
    void updateWarningStatistics();


    /*更新 AI 预测数据*/
    void updatePredictionCards();


    /*
     * 更新电站风险列表
     */
    void updateStationWarnings();


    /*
     * 更新 AI 预警详细分析
     */
    void updateWarningDetails();


private:

    /*
     * =====================================================
     * 预警等级相关辅助函数
     * =====================================================
     */

    /*
     * 获取预警等级文字
     *
     * Critical  -> 特别危险
     * Dangerous -> 危险
     * Attention -> 待关注
     * Normal    -> 一般
     */
    QString warningLevelText(
        AIWarningLevel level
    ) const;


    /*
     * 获取预警等级图标
     *
     * Critical  -> 🔴
     * Dangerous -> 🟠
     * Attention -> 🔵
     * Normal    -> 🟢
     */
    QString warningLevelIcon(
        AIWarningLevel level
    ) const;


    /*
     * 获取预警等级描述
     */
    QString warningLevelDescription(
        AIWarningLevel level
    ) const;


    /*
     * 获取预警等级样式
     */
    QString warningLevelStyle(
        AIWarningLevel level
    ) const;


private:

    /*
     * =====================================================
     * 创建普通信息卡片
     * =====================================================
     *
     * title：
     * 卡片标题
     *
     * valueLabel：
     * 用于接收数值 QLabel
     *
     * descriptionLabel：
     * 用于接收说明 QLabel
     */
    QFrame *createInfoCard(
        const QString &title,
        QLabel **valueLabel,
        QLabel **descriptionLabel
    );


    /*
     * =====================================================
     * 创建电站预警卡片
     * =====================================================
     */
    QFrame *createStationWarningCard(
        const AIStationWarning &warning
    );


private:

    /*
     * =====================================================
     * 页面滚动区域
     * =====================================================
     */

    /*
     * 整个滚动区域
     */
    QScrollArea *scrollArea;


    /*
     * 滚动区域内部的内容容器
     */
    QWidget *contentWidget;


    /*
     * 页面主布局
     */
    QVBoxLayout *mainLayout;


private:

    /*
     * =====================================================
     * 页面顶部
     * =====================================================
     */

    /*
     * 页面标题
     *
     * 🤖 AI智能预警
     */
    QLabel *titleLabel;


    /*
     * 页面副标题
     */
    QLabel *subtitleLabel;


private:

    /*
     * =====================================================
     * 当前总体预警区域
     * =====================================================
     */

    /*
     * 当前预警卡片
     */
    QFrame *currentWarningFrame;


    /*
     * 当前预警图标
     */
    QLabel *currentWarningIcon;


    /*
     * 当前预警标题
     */
    QLabel *currentWarningTitle;


    /*
     * 当前预警等级
     *
     * 例如：
     *
     * 🔴 特别危险
     */
    QLabel *currentWarningLevel;


    /*
     * 当前预警说明
     */
    QLabel *currentWarningDescription;


private:

    /*
     * =====================================================
     * AI 模型状态
     * =====================================================
     */

    /*
     * AI模型运行状态
     *
     * 例如：
     *
     * ● AI模型状态：运行中
     */
    QLabel *modelStatusLabel;


    /*
     * 最后分析时间
     */
    QLabel *lastUpdateLabel;


    /*
     * AI模型状态所在卡片
     */
    QFrame *modelStatusFrame;


private:

    /*
     * =====================================================
     * 预警数量统计
     * =====================================================
     */

    /*
     * 预警统计区域
     */
    QFrame *warningStatisticsFrame;


    /*
     * 特别危险数量
     */
    QLabel *criticalCountLabel;


    /*
     * 危险数量
     */
    QLabel *dangerousCountLabel;


    /*
     * 待关注数量
     */
    QLabel *attentionCountLabel;


    /*
     * 一般数量
     */
    QLabel *normalCountLabel;


private:

    /*
     * =====================================================
     * AI预测信息
     * =====================================================
     */

    /*
     * AI预测区域
     */
    QFrame *predictionFrame;


    /*
     * 预测高峰时段
     */
    QLabel *peakTimeLabel;


    /*
     * 预测峰值占用率
     */
    QLabel *peakOccupancyLabel;


    /*
     * 预测峰值订单量
     */
    QLabel *peakVolumeLabel;


    /*
     * 预测空闲充电桩数量
     */
    QLabel *idlePilesLabel;


private:

    /*
     * =====================================================
     * 电站风险预警
     * =====================================================
     */

    /*
     * 电站风险区域
     */
    QFrame *stationWarningFrame;


    /*
     * 电站风险列表布局
     */
    QVBoxLayout *stationWarningLayout;


private:

    /*
     * =====================================================
     * AI预警详情
     * =====================================================
     */

    /*
     * AI预警详细分析区域
     */
    QFrame *warningDetailFrame;


    /*
     * AI预警详情布局
     */
    QVBoxLayout *warningDetailLayout;


private:

    /*
     * =====================================================
     * 当前 AI 数据
     * =====================================================
     *
     * 当前页面正在显示的数据。
     *
     * 后续 Python 模型更新数据时，
     * 通过 setWarningResult() 更新。
     */
    AIWarningResult currentResult;
};


#endif // AIWARNINGWIDGET_H

