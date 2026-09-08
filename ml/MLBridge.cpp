#include "MLBridge.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include <stdexcept>

MLBridge::MLBridge()
    : m_interpreter(nullptr),
      m_service(py::none()),
      m_initialized(false)
{
}


// ============================================================
// 析构
// ============================================================
MLBridge::~MLBridge()
{
    try
    {
        // 必须先释放 Python 对象
        // 再销毁 Python interpreter
        if (m_interpreter)
        {
            m_service = py::none();
        }
    }
    catch (...)
    {
        // 析构阶段不能让异常继续向外抛
    }

    m_initialized = false;
}


// ============================================================
// initialize
// ============================================================
bool MLBridge::initialize(
    const QString &mlPath,
    const QString &modelPath,
    const QString &xScalerPath,
    const QString &yScalerPath
)
{
    m_lastError.clear();
    m_initialized = false;

    try
    {
        // ----------------------------------------------------
        // 1. 检查 ML 路径
        // ----------------------------------------------------
        if (!QDir(mlPath).exists())
        {
            m_lastError =
                QString("ML Python目录不存在：%1").arg(mlPath);

            qDebug() << "[MLBridge]" << m_lastError;

            return false;
        }


        // ----------------------------------------------------
        // 2. 检查模型文件
        // ----------------------------------------------------
        if (!QFileInfo::exists(modelPath))
        {
            m_lastError =
                QString("模型文件不存在：%1").arg(modelPath);

            qDebug() << "[MLBridge]" << m_lastError;

            return false;
        }


        // ----------------------------------------------------
        // 3. 检查 X scaler
        // ----------------------------------------------------
        if (!QFileInfo::exists(xScalerPath))
        {
            m_lastError =
                QString("X scaler文件不存在：%1").arg(xScalerPath);

            qDebug() << "[MLBridge]" << m_lastError;

            return false;
        }


        // ----------------------------------------------------
        // 4. 检查 Y scaler
        // ----------------------------------------------------
        if (!QFileInfo::exists(yScalerPath))
        {
            m_lastError =
                QString("Y scaler文件不存在：%1").arg(yScalerPath);

            qDebug() << "[MLBridge]" << m_lastError;

            return false;
        }


        // ----------------------------------------------------
        // 5. 启动 Python interpreter
        // ----------------------------------------------------
        if (!m_interpreter)
        {
            m_interpreter =
                std::make_unique<py::scoped_interpreter>();
        }


        // ----------------------------------------------------
        // 6. 导入 Python sys
        // ----------------------------------------------------
        py::module_ sys = py::module_::import("sys");


        // ----------------------------------------------------
        // 7. 将 ML Python 目录加入 sys.path
        // ----------------------------------------------------
        QString absoluteMlPath =
            QDir(mlPath).absolutePath();

        sys.attr("path").attr("insert")(
            0,
            absoluteMlPath.toStdString()
        );


        // ----------------------------------------------------
        // 8. 导入 ml_service.py
        //
        // 你的 Python 目录最终建议：
        //
        // python_ml/
        // ├── ml_service.py
        // ├── model.py
        // └── checkpoints/
        //     ├── best_model.pth
        //     ├── x_scaler.pkl
        //     └── y_scaler.pkl
        // ----------------------------------------------------
        py::module_ mlServiceModule =
            py::module_::import("ml_service");


        // ----------------------------------------------------
        // 9. 获取 MLService 类
        // ----------------------------------------------------
        py::object serviceClass =
            mlServiceModule.attr("MLService");


        // ----------------------------------------------------
        // 10. 创建 Python MLService
        // ----------------------------------------------------
        m_service = serviceClass(
            modelPath.toStdString(),
            xScalerPath.toStdString(),
            yScalerPath.toStdString()
        );


        // ----------------------------------------------------
        // 11. 初始化成功
        // ----------------------------------------------------
        m_initialized = true;

        qDebug()
            << "[MLBridge] Python ML初始化成功";

        qDebug()
            << "[MLBridge] ML Path:"
            << absoluteMlPath;

        qDebug()
            << "[MLBridge] Model:"
            << modelPath;

        return true;
    }
    catch (const py::error_already_set &e)
    {
        m_lastError =
            QString("Python初始化失败：%1")
                .arg(QString::fromUtf8(e.what()));

        qDebug() << "[MLBridge]" << m_lastError;

        m_initialized = false;

        return false;
    }
    catch (const std::exception &e)
    {
        m_lastError =
            QString("ML初始化异常：%1")
                .arg(QString::fromUtf8(e.what()));

        qDebug() << "[MLBridge]" << m_lastError;

        m_initialized = false;

        return false;
    }
    catch (...)
    {
        m_lastError =
            "ML初始化发生未知异常";

        qDebug() << "[MLBridge]" << m_lastError;

        m_initialized = false;

        return false;
    }
}


// ============================================================
// predict
// ============================================================
bool MLBridge::predict(
    int stationId,
    const QVector<MLHistoryPoint> &history,
    const QDateTime &historyEndTime,
    int chargeCount,
    MLPredictionResult &result
)
{
    result = MLPredictionResult();

    m_lastError.clear();

    // --------------------------------------------------------
    // 1. 检查 Python ML 是否初始化
    // --------------------------------------------------------
    if (!m_initialized)
    {
        result.valid = false;
        result.errorMessage =
            "MLBridge尚未初始化";

        m_lastError =
            result.errorMessage;

        return false;
    }


    // --------------------------------------------------------
    // 2. LSTM 历史窗口必须是 24 小时
    // --------------------------------------------------------
    if (history.size() != 24)
    {
        result.valid = false;

        result.errorMessage =
            QString(
                "历史数据长度错误：需要24个小时，"
                "实际为%1个小时"
            ).arg(history.size());

        m_lastError =
            result.errorMessage;

        qDebug()
            << "[MLBridge]"
            << result.errorMessage;

        return false;
    }


    try
    {
        // ====================================================
        // 创建 Python history list
        //
        // 最终形式：
        //
        // [
        //   [12个特征],
        //   [12个特征],
        //   ...
        // ]
        //
        // 一共 24 行 × 12 列
        // ====================================================
        py::list historyList;


        for (const MLHistoryPoint &point : history)
        {
            py::list row;


            // ------------------------------------------------
            // 必须严格按照 Python dataset.py 的 FEATURES
            // 顺序传递
            // ------------------------------------------------

            // 1 occupancy
            row.append(point.occupancy);

            // 2 duration
            row.append(point.duration);

            // 3 volume
            row.append(point.volume);

            // 4 payment
            row.append(point.payment);

            // 5 temperature
            row.append(point.temperature);

            // 6 humidity
            row.append(point.humidity);

            // 7 rainfall
            row.append(point.rainfall);

            // 8 hour_sin
            row.append(point.hourSin);

            // 9 hour_cos
            row.append(point.hourCos);

            // 10 day_sin
            row.append(point.daySin);

            // 11 day_cos
            row.append(point.dayCos);

            // 12 is_holiday
            row.append(point.isHoliday);


            // 加入 history
            historyList.append(row);
        }


        // ====================================================
        // 调用 Python：
        //
        // MLService.predict(
        //     stationId,
        //     history,
        //     historyEndTime,
        //     chargeCount
        // )
        // ====================================================
        py::object pyResult =
            m_service.attr("predict")(
                stationId,
                historyList,
                historyEndTime
                    .toString("yyyy-MM-dd HH:mm:ss")
                    .toStdString(),
                chargeCount
            );


        // ====================================================
        // Python 返回值必须是 dict
        // ====================================================
        if (!py::isinstance<py::dict>(pyResult))
        {
            result.valid = false;

            result.errorMessage =
                "Python MLService.predict()返回值不是dict";

            m_lastError =
                result.errorMessage;

            return false;
        }


        py::dict data =
            pyResult.cast<py::dict>();


        // ====================================================
        // 读取基础信息
        // ====================================================
        if (data.contains("station_id"))
        {
            result.stationId =
                data["station_id"].cast<int>();
        }
        else
        {
            result.stationId = stationId;
        }


        if (data.contains("charge_count"))
        {
            result.chargeCount =
                data["charge_count"].cast<int>();
        }
        else
        {
            result.chargeCount = chargeCount;
        }


        // ====================================================
        // 时间
        // ====================================================
        if (data.contains("history_end_time"))
        {
            result.historyEndTime =
                QString::fromStdString(
                    data["history_end_time"].cast<std::string>()
                );
        }


        if (data.contains("prediction_start_time"))
        {
            result.predictionStartTime =
                QString::fromStdString(
                    data["prediction_start_time"].cast<std::string>()
                );
        }


        if (data.contains("prediction_end_time"))
        {
            result.predictionEndTime =
                QString::fromStdString(
                    data["prediction_end_time"].cast<std::string>()
                );
        }


        // ====================================================
        // occupancy
        // ====================================================
        if (data.contains("occupancy"))
        {
            py::list values =
                data["occupancy"].cast<py::list>();

            for (const py::handle &value : values)
            {
                result.occupancy.append(
                    value.cast<double>()
                );
            }
        }


        // ====================================================
        // volume
        // ====================================================
        if (data.contains("volume"))
        {
            py::list values =
                data["volume"].cast<py::list>();

            for (const py::handle &value : values)
            {
                result.volume.append(
                    value.cast<double>()
                );
            }
        }


        // ====================================================
        // timestamps
        // ====================================================
        if (data.contains("timestamps"))
        {
            py::list values =
                data["timestamps"].cast<py::list>();

            for (const py::handle &value : values)
            {
                result.timestamps.append(
                    QString::fromStdString(
                        value.cast<std::string>()
                    )
                );
            }
        }


        // ====================================================
        // idle_piles
        // ====================================================
        if (data.contains("idle_piles"))
        {
            py::list values =
                data["idle_piles"].cast<py::list>();

            for (const py::handle &value : values)
            {
                result.idlePiles.append(
                    value.cast<int>()
                );
            }
        }


        // ====================================================
        // 负载统计
        // ====================================================
        if (data.contains("load_1h"))
        {
            result.load_1h =
                data["load_1h"].cast<double>();
        }

        if (data.contains("load_6h"))
        {
            result.load_6h =
                data["load_6h"].cast<double>();
        }

        if (data.contains("load_24h"))
        {
            result.load_24h =
                data["load_24h"].cast<double>();
        }


        if (data.contains("avg_load_1h"))
        {
            result.avg_load_1h =
                data["avg_load_1h"].cast<double>();
        }

        if (data.contains("avg_load_6h"))
        {
            result.avg_load_6h =
                data["avg_load_6h"].cast<double>();
        }

        if (data.contains("avg_load_24h"))
        {
            result.avg_load_24h =
                data["avg_load_24h"].cast<double>();
        }


        // ====================================================
        // 峰值
        // ====================================================
        if (data.contains("peak_time"))
        {
            result.peakTime =
                QString::fromStdString(
                    data["peak_time"].cast<std::string>()
                );
        }

        if (data.contains("peak_occupancy"))
        {
            result.peakOccupancy =
                data["peak_occupancy"].cast<double>();
        }

        if (data.contains("peak_volume"))
        {
            result.peakVolume =
                data["peak_volume"].cast<double>();
        }

        if (data.contains("peak_idle_piles"))
        {
            result.peakIdlePiles =
                data["peak_idle_piles"].cast<int>();
        }


        // ====================================================
        // 最终检查
        // ====================================================
        if (result.occupancy.isEmpty())
        {
            result.valid = false;

            result.errorMessage =
                "Python没有返回occupancy预测结果";

            m_lastError =
                result.errorMessage;

            return false;
        }


        if (result.volume.isEmpty())
        {
            result.valid = false;

            result.errorMessage =
                "Python没有返回volume预测结果";

            m_lastError =
                result.errorMessage;

            return false;
        }


        // ====================================================
        // 正常
        // ====================================================
        result.valid = true;

        qDebug()
            << "[MLBridge] ML预测成功";

        qDebug()
            << "[MLBridge] stationId:"
            << result.stationId;

        qDebug()
            << "[MLBridge] occupancy size:"
            << result.occupancy.size();

        qDebug()
            << "[MLBridge] volume size:"
            << result.volume.size();

        qDebug()
            << "[MLBridge] peak occupancy:"
            << result.peakOccupancy;

        return true;
    }
    catch (const py::error_already_set &e)
    {
        result.valid = false;

        result.errorMessage =
            QString("Python预测失败：%1")
                .arg(QString::fromUtf8(e.what()));

        m_lastError =
            result.errorMessage;

        qDebug()
            << "[MLBridge]"
            << result.errorMessage;

        return false;
    }
    catch (const std::exception &e)
    {
        result.valid = false;

        result.errorMessage =
            QString("ML预测异常：%1")
                .arg(QString::fromUtf8(e.what()));

        m_lastError =
            result.errorMessage;

        qDebug()
            << "[MLBridge]"
            << result.errorMessage;

        return false;
    }
    catch (...)
    {
        result.valid = false;

        result.errorMessage =
            "ML预测发生未知异常";

        m_lastError =
            result.errorMessage;

        qDebug()
            << "[MLBridge]"
            << result.errorMessage;

        return false;
    }
}


// ============================================================
// isInitialized
// ============================================================
bool MLBridge::isInitialized() const
{
    return m_initialized;
}


// ============================================================
// lastError
// ============================================================
QString MLBridge::lastError() const
{
    return m_lastError;
}