#ifndef APISERVER_H
#define APISERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include <QByteArray>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QHash>

class ApiServer : public QObject
{
    Q_OBJECT

public:
    explicit ApiServer(QObject *parent = nullptr);

    ~ApiServer();

    // 启动 HTTP Server
    bool start(quint16 port = 8766);

    // 停止 HTTP Server
    void stop();

    // 当前是否正在监听
    bool isRunning() const;

    // 当前监听端口
    quint16 port() const;

private slots:
    // 新客户端连接
    void onNewConnection();

    // 客户端发送数据
    void onReadyRead();

    // 客户端断开
    void onDisconnected();

private:
    // =========================
    // HTTP 请求处理
    // =========================

    void processRequest(QTcpSocket *socket,
                        const QByteArray &request);

    void routeRequest(QTcpSocket *socket,
                      const QString &method,
                      const QString &path,
                      const QMap<QString, QString> &query);

    // =========================
    // HTTP 响应
    // =========================

    void sendJson(QTcpSocket *socket,
                  const QJsonDocument &document,
                  int statusCode = 200,
                  const QString &statusText = "OK");

    void sendJsonObject(QTcpSocket *socket,
                        const QJsonObject &object,
                        int statusCode = 200,
                        const QString &statusText = "OK");

    void sendJsonArray(QTcpSocket *socket,
                       const QJsonArray &array,
                       int statusCode = 200,
                       const QString &statusText = "OK");

    void sendError(QTcpSocket *socket,
                   int statusCode,
                   const QString &message);

    // =========================
    // 工具函数
    // =========================

    QString extractPath(const QString &target) const;

    QMap<QString, QString> extractQuery(const QString &target) const;

    QString urlDecode(const QString &value) const;

    // =========================
    // API 数据构建
    // =========================

    QJsonObject buildOverview() const;

    QJsonObject buildRevenue(int days) const;

    QJsonArray buildPileStatus() const;

    QJsonArray buildTopStations(int limit) const;

    QJsonObject buildCharging24h() const;

    QJsonObject buildUserTrend() const;

    QJsonArray buildUserLevels() const;

    QJsonArray buildStationsHealth() const;

    // 当前暂时不接入 ML，
    // 但保留接口避免 Web 端请求时出现 404。
    QJsonObject buildPrediction(int hours) const;

private:
    QTcpServer *server;

    // HTTP 请求缓冲
    QHash<QTcpSocket *, QByteArray> buffers;

    quint16 listenPort;
};

#endif // APISERVER_H
