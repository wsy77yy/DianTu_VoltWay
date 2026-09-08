#include "ApiServer.h"

#include "../manager/StationManager.h"
#include "../manager/PileManager.h"
#include "../manager/OrderManager.h"
#include "../manager/RevenueManager.h"
#include "../manager/UserManager.h"

#include <QDate>
#include <QDateTime>
#include <QDebug>
#include <QHostAddress>
#include <QMap>
#include <QUrl>
#include <QUrlQuery>

#include <algorithm>


// ============================================================
// 构造 / 析构
// ============================================================

ApiServer::ApiServer( QObject *parent)
    : QObject(parent),
      server(new QTcpServer(this)),
      listenPort(8766)
{
    connect(server,
            &QTcpServer::newConnection,
            this,
            &ApiServer::onNewConnection);
}

ApiServer::~ApiServer()
{
    stop();
}


// ============================================================
// 启动服务器
// ============================================================

bool ApiServer::start(quint16 port)
{
    if (server->isListening())
    {
        qDebug() << "[ApiServer] Server already running on port"
                 << server->serverPort();

        return true;
    }

    listenPort = port;

    bool ok = server->listen(
        QHostAddress::AnyIPv4,
        listenPort
    );

    if (!ok)
    {
        qWarning() << "[ApiServer] Failed to start:"
                   << server->errorString();

        return false;
    }

    qDebug() << "==========================================";
    qDebug() << "[ApiServer] HTTP server started";
    qDebug() << "[ApiServer] Address:"
             << server->serverAddress().toString();
    qDebug() << "[ApiServer] Port:"
             << server->serverPort();
    qDebug() << "[ApiServer] API base:"
             << QString("http://127.0.0.1:%1")
                    .arg(server->serverPort());
    qDebug() << "==========================================";

    return true;
}


// ============================================================
// 停止服务器
// ============================================================

void ApiServer::stop()
{
    if (!server->isListening())
        return;

    server->close();

    buffers.clear();

    qDebug() << "[ApiServer] HTTP server stopped";
}


// ============================================================
// 是否运行
// ============================================================

bool ApiServer::isRunning() const
{
    return server->isListening();
}


// ============================================================
// 获取端口
// ============================================================

quint16 ApiServer::port() const
{
    return server->serverPort();
}


// ============================================================
// 新客户端连接
// ============================================================

void ApiServer::onNewConnection()
{
    while (server->hasPendingConnections())
    {
        QTcpSocket *socket =
            server->nextPendingConnection();

        if (!socket)
            continue;

        buffers.insert(socket, QByteArray());

        qDebug() << "[ApiServer] Client connected:"
                 << socket->peerAddress().toString()
                 << socket->peerPort();

        connect(socket,
                &QTcpSocket::readyRead,
                this,
                &ApiServer::onReadyRead);

        connect(socket,
                &QTcpSocket::disconnected,
                this,
                &ApiServer::onDisconnected);
    }
}


// ============================================================
// 接收 HTTP 数据
// ============================================================

void ApiServer::onReadyRead()
{
    QTcpSocket *socket =
        qobject_cast<QTcpSocket *>(sender());

    if (!socket)
        return;

    buffers[socket].append(socket->readAll());

    QByteArray &buffer = buffers[socket];

    // HTTP Header 结束标志
    int headerEnd = buffer.indexOf("\r\n\r\n");

    if (headerEnd == -1)
        return;

    QByteArray request =
        buffer.left(headerEnd + 4);

    // 当前版本主要处理 GET 请求
    processRequest(socket, request);

    // 一个请求一个连接
    buffer.clear();
}


// ============================================================
// 客户端断开
// ============================================================

void ApiServer::onDisconnected()
{
    QTcpSocket *socket =
        qobject_cast<QTcpSocket *>(sender());

    if (!socket)
        return;

    qDebug() << "[ApiServer] Client disconnected:"
             << socket->peerAddress().toString();

    buffers.remove(socket);

    socket->deleteLater();
}


// ============================================================
// HTTP 请求解析
// ============================================================

void ApiServer::processRequest(
    QTcpSocket *socket,
    const QByteArray &request)
{
    QList<QByteArray> lines =
        request.split('\n');

    if (lines.isEmpty())
    {
        sendError(
            socket,
            400,
            "Invalid HTTP request"
        );

        return;
    }

    QByteArray requestLine =
        lines.first().trimmed();

    QList<QByteArray> parts =
        requestLine.split(' ');

    if (parts.size() < 2)
    {
        sendError(
            socket,
            400,
            "Invalid HTTP request line"
        );

        return;
    }

    QString method =
        QString::fromUtf8(parts.at(0))
            .trimmed()
            .toUpper();

    QString target =
        QString::fromUtf8(parts.at(1))
            .trimmed();

    QString path =
        extractPath(target);

    QMap<QString, QString> query =
        extractQuery(target);

    qDebug() << "[ApiServer]"
             << method
             << target;

    routeRequest(
        socket,
        method,
        path,
        query
    );
}


// ============================================================
// 路由
// ============================================================

void ApiServer::routeRequest(
    QTcpSocket *socket,
    const QString &method,
    const QString &path,
    const QMap<QString, QString> &query)
{
    // --------------------------------------------------------
    // CORS 预检
    // --------------------------------------------------------

    if (method == "OPTIONS")
    {
        QByteArray body = "{}";

        QByteArray response;

        response += "HTTP/1.1 204 No Content\r\n";
        response += "Content-Type: application/json; charset=utf-8\r\n";
        response += "Access-Control-Allow-Origin: *\r\n";
        response += "Access-Control-Allow-Methods: GET, OPTIONS\r\n";
        response += "Access-Control-Allow-Headers: Content-Type\r\n";
        response += "Connection: close\r\n";
        response += "\r\n";

        socket->write(response);
        socket->disconnectFromHost();

        return;
    }

    // --------------------------------------------------------
    // 当前第一阶段只允许 GET
    // --------------------------------------------------------

    if (method != "GET")
    {
        sendError(
            socket,
            405,
            "Only GET is supported"
        );

        return;
    }


    // --------------------------------------------------------
    // GET /api/overview
    // --------------------------------------------------------

    if (path == "/api/overview")
    {
        sendJsonObject(
            socket,
            buildOverview()
        );

        return;
    }


    // --------------------------------------------------------
    // GET /api/revenue?days=7
    // GET /api/revenue?days=30
    // --------------------------------------------------------

    if (path == "/api/revenue")
    {
        int days = 7;

        if (query.contains("days"))
        {
            bool ok = false;

            int value =
                query.value("days").toInt(&ok);

            if (ok && value > 0)
                days = value;
        }

        if (days != 7 && days != 30)
            days = 7;

        sendJsonObject(
            socket,
            buildRevenue(days)
        );

        return;
    }


    // --------------------------------------------------------
    // GET /api/pile-status
    // --------------------------------------------------------

    if (path == "/api/pile-status")
    {
        sendJsonArray(
            socket,
            buildPileStatus()
        );

        return;
    }


    // --------------------------------------------------------
    // GET /api/stations/top-revenue?limit=10
    // --------------------------------------------------------

    if (path == "/api/stations/top-revenue")
    {
        int limit = 10;

        if (query.contains("limit"))
        {
            bool ok = false;

            int value =
                query.value("limit").toInt(&ok);

            if (ok && value > 0)
                limit = value;
        }

        // 防止一次请求太多
        limit = qBound(1, limit, 100);

        sendJsonArray(
            socket,
            buildTopStations(limit)
        );

        return;
    }


    // --------------------------------------------------------
    // GET /api/charging/24h
    // --------------------------------------------------------

    if (path == "/api/charging/24h")
    {
        sendJsonObject(
            socket,
            buildCharging24h()
        );

        return;
    }


    // --------------------------------------------------------
    // GET /api/users/trend
    // --------------------------------------------------------

    if (path == "/api/users/trend")
    {
        sendJsonObject(
            socket,
            buildUserTrend()
        );

        return;
    }


    // --------------------------------------------------------
    // GET /api/users/levels
    // --------------------------------------------------------

    if (path == "/api/users/levels")
    {
        sendJsonArray(
            socket,
            buildUserLevels()
        );

        return;
    }


    // --------------------------------------------------------
    // GET /api/stations/health
    // --------------------------------------------------------

    if (path == "/api/stations/health")
    {
        sendJsonArray(
            socket,
            buildStationsHealth()
        );

        return;
    }


    // --------------------------------------------------------
    // GET /api/prediction?hours=1|6|24
    // --------------------------------------------------------

    if (path == "/api/prediction")
    {
        int hours = 24;

        if (query.contains("hours"))
        {
            bool ok = false;

            int value =
                query.value("hours").toInt(&ok);

            if (ok && value > 0)
                hours = value;
        }

        if (hours != 1 &&
            hours != 6 &&
            hours != 24)
        {
            hours = 24;
        }

        sendJsonObject(
            socket,
            buildPrediction(hours)
        );

        return;
    }


    // --------------------------------------------------------
    // 404
    // --------------------------------------------------------

    sendError(
        socket,
        404,
        "API endpoint not found"
    );
}


// ============================================================
// /api/overview
// ============================================================

QJsonObject ApiServer::buildOverview() const
{
    QJsonObject result;

    // --------------------------------------------------------
    // 订单
    // --------------------------------------------------------

    QList<ChargingOrder> orders =
        OrderManager::instance().getOrders();

    int monthOrders = 0;

    QDate currentDate =
        QDate::currentDate();

    for (const ChargingOrder &order : orders)
    {
        if (!order.createTime.isValid())
            continue;

        QDate date =
            order.createTime.date();

        if (date.year() == currentDate.year() &&
            date.month() == currentDate.month())
        {
            ++monthOrders;
        }
    }


    // --------------------------------------------------------
    // 月收入
    // --------------------------------------------------------

    double monthRevenue =
        RevenueManager::instance()
            .getMonthRevenue();


    // --------------------------------------------------------
    // 电站
    // --------------------------------------------------------

    QList<ManagedStation> stations =
        StationManager::instance()
            .getStations();

    int activeStations = 0;

    for (const ManagedStation &station : stations)
    {
        QString status =
            station.status.trimmed();

        // 兼容你们项目可能使用的状态名称
        if (status != "停用" &&
            status != "关闭" &&
            status != "故障")
        {
            ++activeStations;
        }
    }


    // --------------------------------------------------------
    // 电桩健康率
    // --------------------------------------------------------

    QList<ChargingPile> piles =
        PileManager::instance()
            .getPiles();

    int healthyCount = 0;

    for (const ChargingPile &pile : piles)
    {
        if (pile.status != "故障" &&
            pile.status != "断电" &&
            pile.online)
        {
            ++healthyCount;
        }
    }

    double healthRate = 0.0;

    if (!piles.isEmpty())
    {
        healthRate =
            healthyCount * 100.0 /
            piles.size();
    }


    // --------------------------------------------------------
    // 当前 Manager 中没有 kWh 字段
    //
    // 所以这里暂时根据订单金额无法可靠计算 kWh。
    // 先返回 0，避免伪造数据。
    // --------------------------------------------------------

    double monthKwh = 0.0;


    // --------------------------------------------------------
    // JSON
    // --------------------------------------------------------

    result["monthOrders"] =
        monthOrders;

    result["monthKwh"] =
        monthKwh;

    result["monthRevenue"] =
        monthRevenue;

    result["activeStations"] =
        activeStations;

    result["healthRate"] =
        qRound(healthRate * 10.0) / 10.0;


    // 当前没有历史快照数据，
    // 因此环比暂时返回 0。
    QJsonObject mom;

    mom["monthOrders"] = 0;
    mom["monthKwh"] = 0;
    mom["monthRevenue"] = 0;
    mom["activeStations"] = 0;
    mom["healthRate"] = 0;

    result["mom"] = mom;

    return result;
}


// ============================================================
// /api/revenue
// ============================================================

QJsonObject ApiServer::buildRevenue(int days) const
{
    QJsonObject result;

    result["days"] = days;

    QJsonArray values;

    QList<RevenueRecord> records =
        RevenueManager::instance()
            .getRecords();

    QMap<QDate, double> daily;

    for (const RevenueRecord &record : records)
    {
        daily[record.date] += record.amount;
    }

    QDate today =
        QDate::currentDate();

    for (int i = days - 1; i >= 0; --i)
    {
        QDate date =
            today.addDays(-i);

        double amount =
            daily.value(date, 0.0);

        values.append(amount);
    }

    result["values"] = values;

    return result;
}


// ============================================================
// /api/pile-status
// ============================================================

QJsonArray ApiServer::buildPileStatus() const
{
    QList<ChargingPile> piles =
        PileManager::instance()
            .getPiles();

    int usingCount = 0;
    int idleCount = 0;
    int faultCount = 0;

    for (const ChargingPile &pile : piles)
    {
        QString status =
            pile.status.trimmed();

        if (status == "在用" ||
            status == "使用中")
        {
            ++usingCount;
        }
        else if (status == "闲置" ||
                 status == "空闲")
        {
            ++idleCount;
        }
        else if (status == "故障" ||
                 status == "断电")
        {
            ++faultCount;
        }
        else
        {
            // 未知状态不计入故障，
            // 防止数据状态扩展导致统计错误。
        }
    }

    int total =
        usingCount +
        idleCount +
        faultCount;

    QJsonArray result;

    auto appendStatus =
        [&result, total](
            const QString &name,
            int value)
        {
            QJsonObject item;

            item["name"] = name;
            item["value"] = value;

            double pct = 0.0;

            if (total > 0)
            {
                pct =
                    value * 100.0 /
                    total;
            }

            item["pct"] =
                qRound(pct * 10.0) / 10.0;

            result.append(item);
        };

    appendStatus(
        "在用",
        usingCount
    );

    appendStatus(
        "闲置",
        idleCount
    );

    appendStatus(
        "故障",
        faultCount
    );

    return result;
}


// ============================================================
// /api/stations/top-revenue
// ============================================================

QJsonArray ApiServer::buildTopStations(int limit) const
{
    QList<ManagedStation> stations =
        StationManager::instance()
            .getStations();

    QList<ChargingOrder> orders =
        OrderManager::instance()
            .getOrders();


    struct StationResult
    {
        ManagedStation station;
        int orders = 0;
    };

    QList<StationResult> results;

    for (const ManagedStation &station : stations)
    {
        StationResult item;

        item.station = station;

        for (const ChargingOrder &order : orders)
        {
            if (order.stationCode ==
                station.code)
            {
                ++item.orders;
            }
        }

        results.append(item);
    }


    // --------------------------------------------------------
    // 按收入降序
    // --------------------------------------------------------

    std::sort(
        results.begin(),
        results.end(),
        [](const StationResult &a,
           const StationResult &b)
        {
            return a.station.revenue >
                   b.station.revenue;
        }
    );


    // --------------------------------------------------------
    // JSON
    // --------------------------------------------------------

    QJsonArray array;

    int count =
        qMin(limit, results.size());

    for (int i = 0; i < count; ++i)
    {
        const StationResult &item =
            results.at(i);

        QJsonObject object;

        object["name"] =
            item.station.name;

        object["revenue"] =
            item.station.revenue;

        object["orders"] =
            item.orders;

        object["stationCode"] =
            item.station.code;

        array.append(object);
    }

    return array;
}


// ============================================================
// /api/charging/24h
// ============================================================

QJsonObject ApiServer::buildCharging24h() const
{
    QJsonObject result;

    QList<ChargingOrder> orders =
        OrderManager::instance()
            .getOrders();

    QList<ManagedStation> stations =
        StationManager::instance()
            .getStations();


    // --------------------------------------------------------
    // 区域
    // --------------------------------------------------------

    QStringList regions;

    for (const ManagedStation &station : stations)
    {
        if (!station.district.isEmpty() &&
            !regions.contains(station.district))
        {
            regions.append(
                station.district
            );
        }
    }

    // 如果没有区域数据，提供默认区域
    if (regions.isEmpty())
    {
        regions << "默认区域";
    }


    // 最多保留 6 个区域，
    // 与当前 Web 图表设计保持一致
    while (regions.size() > 6)
    {
        regions.removeLast();
    }


    // --------------------------------------------------------
    // 小时
    // --------------------------------------------------------

    QJsonArray hours;

    for (int h = 0; h < 24; ++h)
    {
        hours.append(
            QString("%1:00").arg(h)
        );
    }

    result["regions"] = QJsonArray::fromStringList(regions);
    result["hours"] = hours;


    // --------------------------------------------------------
    // 建立区域索引
    // --------------------------------------------------------

    QMap<QString, int> regionIndex;

    for (int i = 0;
         i < regions.size();
         ++i)
    {
        regionIndex[
            regions.at(i)
        ] = i;
    }


    // --------------------------------------------------------
    // 统计订单
    // --------------------------------------------------------

    QMap<QPair<int, int>, int> counter;

    QDate today =
        QDate::currentDate();

    for (const ChargingOrder &order : orders)
    {
        if (!order.createTime.isValid())
            continue;

        QDateTime time =
            order.createTime;

        if (time.date() != today)
            continue;

        int hour =
            time.time().hour();

        QString district =
            "默认区域";

        for (const ManagedStation &station : stations)
        {
            if (station.code ==
                order.stationCode)
            {
                if (!station.district.isEmpty())
                {
                    district =
                        station.district;
                }

                break;
            }
        }

        if (!regionIndex.contains(district))
        {
            district = regions.first();
        }

        int region =
            regionIndex.value(
                district,
                0
            );

        counter[
            qMakePair(region, hour)
        ]++;
    }


    // --------------------------------------------------------
    // Web 要求：
    //
    // data = [
    //   [x, y, value],
    //   ...
    // ]
    // --------------------------------------------------------

    QJsonArray data;

    for (int y = 0;
         y < regions.size();
         ++y)
    {
        for (int x = 0;
             x < 24;
             ++x)
        {
            QJsonArray point;

            point.append(x);
            point.append(y);

            int value =
                counter.value(
                    qMakePair(y, x),
                    0
                );

            point.append(value);

            data.append(point);
        }
    }

    result["data"] = data;

    return result;
}


// ============================================================
// /api/users/trend
// ============================================================

QJsonObject ApiServer::buildUserTrend() const
{
    QJsonObject result;

    QList<ManagedUser> users =
        UserManager::instance()
            .getUsers();


    // --------------------------------------------------------
    // 最近 30 天
    // --------------------------------------------------------

    QDate today =
        QDate::currentDate();

    QJsonArray days;
    QJsonArray cumulative;
    QJsonArray daily;


    int baseUsers = 0;

    // 统计 30 天前已经存在的用户
    QDate startDate =
        today.addDays(-29);

    for (const ManagedUser &user : users)
    {
        if (!user.registerTime.isValid())
            continue;

        if (user.registerTime.date() < startDate)
        {
            ++baseUsers;
        }
    }


    int total =
        baseUsers;


    // --------------------------------------------------------
    // 每一天
    // --------------------------------------------------------

    for (int i = 0; i < 30; ++i)
    {
        QDate date =
            startDate.addDays(i);

        int newUsers = 0;

        for (const ManagedUser &user : users)
        {
            if (!user.registerTime.isValid())
                continue;

            if (user.registerTime.date() ==
                date)
            {
                ++newUsers;
            }
        }

        total += newUsers;

        days.append(
            QString("%1/%2")
                .arg(date.month())
                .arg(date.day(), 2, 10, QChar('0'))
        );

        cumulative.append(total);
        daily.append(newUsers);
    }


    result["days"] = days;
    result["cumulative"] = cumulative;
    result["daily"] = daily;

    return result;
}


// ============================================================
// /api/users/levels
// ============================================================

QJsonArray ApiServer::buildUserLevels() const
{
    QList<ManagedUser> users =
        UserManager::instance().getUsers();

    int normal = 0;
    int silver = 0;
    int gold = 0;
    int diamond = 0;

    /*
     * 当前 ManagedUser 没有“用户等级”字段。
     *
     * 因此这里根据累计消费金额进行等级映射：
     *
     * < 500       普通
     * 500~1999    银卡
     * 2000~4999   金卡
     * >= 5000     钻石
     *
     * 当前用户模型中没有消费总额字段，
     * 因此暂时使用 balance 作为可用的数值指标。
     *
     * 如果后续 UserManager 增加 level 字段，
     * 这里可以直接改成真实等级统计。
     */

    for (const ManagedUser &user : users)
    {
        double value = user.balance;

        if (value >= 5000)
        {
            ++diamond;
        }
        else if (value >= 2000)
        {
            ++gold;
        }
        else if (value >= 500)
        {
            ++silver;
        }
        else
        {
            ++normal;
        }
    }

    int total =
        normal +
        silver +
        gold +
        diamond;

    QJsonArray result;

    auto appendLevel =
        [&result, total](
            const QString &name,
            int count)
        {
            QJsonObject object;

            object["name"] = name;

            double value = 0.0;

            if (total > 0)
            {
                value =
                    count * 100.0 /
                    total;
            }

            object["value"] =
                qRound(value * 10.0) / 10.0;

            result.append(object);
        };

    appendLevel("普通", normal);
    appendLevel("银卡", silver);
    appendLevel("金卡", gold);
    appendLevel("钻石", diamond);

    return result;
}


// ============================================================
// HTTP JSON Response
// ============================================================

void ApiServer::sendJson(
    QTcpSocket *socket,
    const QJsonDocument &document,
    int statusCode,
    const QString &statusText)
{
    if (!socket)
        return;

    QByteArray body =
        document.toJson(
            QJsonDocument::Compact
        );

    QByteArray response;

    response +=
        QString(
            "HTTP/1.1 %1 %2\r\n"
        )
        .arg(statusCode)
        .arg(statusText)
        .toUtf8();

    response +=
        "Content-Type: application/json; charset=utf-8\r\n";

    response +=
        "Access-Control-Allow-Origin: *\r\n";

    response +=
        "Access-Control-Allow-Methods: GET, OPTIONS\r\n";

    response +=
        "Access-Control-Allow-Headers: Content-Type\r\n";

    response +=
        "Cache-Control: no-cache\r\n";

    response +=
        "Connection: close\r\n";

    response +=
        "Content-Length: "
        + QByteArray::number(body.size())
        + "\r\n";

    response +=
        "\r\n";

    response += body;

    socket->write(response);
    socket->disconnectFromHost();
}


// ============================================================
// JSON Object
// ============================================================

void ApiServer::sendJsonObject(
    QTcpSocket *socket,
    const QJsonObject &object,
    int statusCode,
    const QString &statusText)
{
    sendJson(
        socket,
        QJsonDocument(object),
        statusCode,
        statusText
    );
}


// ============================================================
// JSON Array
// ============================================================

void ApiServer::sendJsonArray(
    QTcpSocket *socket,
    const QJsonArray &array,
    int statusCode,
    const QString &statusText)
{
    sendJson(
        socket,
        QJsonDocument(array),
        statusCode,
        statusText
    );
}


// ============================================================
// 错误
// ============================================================

void ApiServer::sendError(
    QTcpSocket *socket,
    int statusCode,
    const QString &message)
{
    QJsonObject object;

    object["error"] =
        true;

    object["status"] =
        statusCode;

    object["message"] =
        message;

    QString statusText;

    switch (statusCode)
    {
    case 400:
        statusText = "Bad Request";
        break;

    case 404:
        statusText = "Not Found";
        break;

    case 405:
        statusText = "Method Not Allowed";
        break;

    case 501:
        statusText = "Not Implemented";
        break;

    default:
        statusText = "Error";
        break;
    }

    sendJsonObject(
        socket,
        object,
        statusCode,
        statusText
    );
}


// ============================================================
// 提取 Path
// ============================================================

QString ApiServer::extractPath(
    const QString &target) const
{
    int question =
        target.indexOf('?');

    if (question == -1)
        return target;

    return target.left(question);
}


// ============================================================
// 提取 Query
// ============================================================

QMap<QString, QString>
ApiServer::extractQuery(
    const QString &target) const
{
    QMap<QString, QString> result;

    int question =
        target.indexOf('?');

    if (question == -1)
        return result;

    QString queryString =
        target.mid(question + 1);

    const QStringList pairs =
        queryString.split(
            '&',
            Qt::SkipEmptyParts
        );

    for (const QString &pair : pairs)
    {
        int equal =
            pair.indexOf('=');

        if (equal == -1)
        {
            result[
                urlDecode(pair)
            ] = "";

            continue;
        }

        QString key =
            pair.left(equal);

        QString value =
            pair.mid(equal + 1);

        result[
            urlDecode(key)
        ] =
            urlDecode(value);
    }

    return result;
}


// ============================================================
// URL Decode
// ============================================================

QString ApiServer::urlDecode(
    const QString &value) const
{
    return QUrl::fromPercentEncoding(
        value.toUtf8()
    );
}

// ============================================================
// /api/stations/health
// ============================================================

QJsonArray ApiServer::buildStationsHealth() const
{
    QList<ChargingPile> piles =
        PileManager::instance().getPiles();

    int excellent = 0;
    int good = 0;
    int normal = 0;
    int poor = 0;

    /*
     * 当前 ChargingPile 没有 batteryHealth 字段，
     * 因此根据电桩在线状态和运行状态做健康等级估算：
     *
     * 优秀：在线且正常
     * 良好：在线但当前闲置
     * 一般：断电
     * 差：故障
     */

    for (const ChargingPile &pile : piles)
    {
        QString status =
            pile.status.trimmed();

        if (status == "故障")
        {
            ++poor;
        }
        else if (status == "断电")
        {
            ++normal;
        }
        else if (status == "闲置" ||
                 status == "空闲")
        {
            ++good;
        }
        else if (pile.online)
        {
            ++excellent;
        }
        else
        {
            ++normal;
        }
    }

    int total =
        excellent +
        good +
        normal +
        poor;

    QJsonArray result;

    auto appendHealth =
        [&result, total](
            const QString &name,
            int count)
        {
            QJsonObject object;

            object["name"] = name;

            double value = 0.0;

            if (total > 0)
            {
                value =
                    count * 100.0 /
                    total;
            }

            object["value"] =
                qRound(value * 10.0) / 10.0;

            result.append(object);
        };

    appendHealth("优秀", excellent);
    appendHealth("良好", good);
    appendHealth("一般", normal);
    appendHealth("差", poor);

    return result;
}

// ============================================================
// /api/prediction
// ============================================================

QJsonObject ApiServer::buildPrediction(
    int hours
) const
{
    QJsonObject result;

    // ========================================================
    // 当前 Linux administrator 验证阶段暂不启用 ML
    //
    // 保留 /api/prediction 接口，
    // 防止 Web 端请求该接口时出现 404。
    //
    // 后续接入 PyBind11 + AIManager 后，
    // 再恢复真实预测逻辑。
    // ========================================================

    result["success"] = false;
    result["enabled"] = false;
    result["hours"] = hours;

    result["message"] =
        "AI预测模块当前未启用";

    // 返回空数据，保证 Web 端可以正常解析
    result["timestamps"] = QJsonArray();
    result["occupancy"] = QJsonArray();
    result["volume"] = QJsonArray();
    result["idlePiles"] = QJsonArray();
    result["warnings"] = QJsonArray();

    // 负荷统计
    result["load1h"] = 0;
    result["load6h"] = 0;
    result["load24h"] = 0;

    result["avgLoad1h"] = 0;
    result["avgLoad6h"] = 0;
    result["avgLoad24h"] = 0;

    // 峰值信息
    result["peakTime"] = "";
    result["peakOccupancy"] = 0;
    result["peakVolume"] = 0;
    result["peakIdlePiles"] = 0;

    return result;
}
