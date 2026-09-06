# -*- coding: utf-8 -*-
"""
SCML 大屏 · Mock 后端服务器
====================================================================
模拟 PC 服务器端（PRL）+ PE1 数据库 + PE2 机器学习提供的 HTTP API，
用于大屏联调测试。等真实后端就绪后，大屏只需把 API_BASE 换成真实地址即可。

启动方式：
    py mock_server.py
默认端口：8766
API 基础地址：http://127.0.0.1:8766

提供的 9 个接口（与 js/data.js 一一对应）：
    GET /api/overview                  核心运营指标
    GET /api/revenue?days=7|30        营收趋势
    GET /api/pile-status               电桩状态分布
    GET /api/charging/24h              24h 充电量热力图
    GET /api/stations/top-revenue?limit=10   营收 TOP 电站
    GET /api/users/trend               用户分析（累计/每日新增）
    GET /api/users/levels              用户级别占比
    GET /api/stations/health           电池健康等级
    GET /api/prediction?hours=1|6|24  AI 负荷预测
====================================================================
"""
from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse, parse_qs
import json
import random
import time

PORT = 8766

# ============ 基础假数据 ============

STATIONS = [
    {"id": 1,  "name": "中关村软件园超级充电站", "region": "海淀区",   "base_revenue": 84200, "base_orders": 312},
    {"id": 2,  "name": "奥林匹克公园快充站",     "region": "朝阳区",   "base_revenue": 61600, "base_orders": 244},
    {"id": 3,  "name": "国贸城市补能站",         "region": "朝阳区",   "base_revenue": 103900, "base_orders": 386},
    {"id": 4,  "name": "望京智慧充电站",         "region": "朝阳区",   "base_revenue": 55800, "base_orders": 208},
    {"id": 5,  "name": "五道口科技充电站",       "region": "海淀区",   "base_revenue": 47200, "base_orders": 189},
    {"id": 6,  "name": "亦庄开发区补能站",       "region": "大兴区",   "base_revenue": 39800, "base_orders": 156},
    {"id": 7,  "name": "石景山万达快充站",       "region": "石景山区", "base_revenue": 33400, "base_orders": 128},
    {"id": 8,  "name": "通州运河商务充电站",     "region": "通州区",   "base_revenue": 28100, "base_orders": 104},
    {"id": 9,  "name": "西二旗高新充电站",       "region": "海淀区",   "base_revenue": 24700, "base_orders": 96},
    {"id": 10, "name": "顺义空港快充站",         "region": "顺义区",   "base_revenue": 19900, "base_orders": 72},
]

REGIONS = ["海淀区", "朝阳区", "东城区", "西城区", "丰台区", "昌平区"]

# 24h 充电量基础矩阵（行=区域，列=0~23点），早晚双峰
HEAT_BASE = [
    [22, 18, 16, 14, 12, 20, 35, 58, 72, 68, 55, 48, 44, 39, 42, 51, 63, 76, 91, 88, 74, 62, 50, 29],
    [18, 15, 13, 11, 10, 16, 28, 46, 58, 54, 44, 38, 35, 31, 33, 40, 50, 61, 74, 70, 59, 49, 40, 24],
    [12, 10, 9,  8,  7,  11, 20, 33, 42, 39, 32, 27, 25, 22, 24, 29, 36, 44, 53, 50, 42, 35, 29, 17],
    [10, 9,  8,  7,  6,  9,  17, 28, 36, 33, 27, 23, 21, 19, 20, 25, 31, 38, 46, 43, 36, 30, 25, 15],
    [14, 12, 10, 9,  8,  12, 22, 36, 46, 42, 35, 30, 27, 24, 26, 32, 40, 49, 59, 55, 46, 39, 32, 19],
    [8,  7,  6,  6,  5,  8,  14, 23, 30, 27, 22, 19, 17, 15, 17, 21, 26, 32, 38, 36, 30, 25, 20, 12],
]

USER_LEVELS = [
    {"name": "普通", "value": 42},
    {"name": "银卡", "value": 26},
    {"name": "金卡", "value": 18},
    {"name": "钻石", "value": 14},
]

HEALTH_LEVELS = [
    {"name": "优秀", "value": 46},
    {"name": "良好", "value": 34},
    {"name": "一般", "value": 15},
    {"name": "差",   "value": 5},
]

PILE_STATUS_BASE = {"using": 46, "idle": 72, "fault": 6}

PRED_STATION = "中关村软件园超级充电站"
PRED_LOAD_BASE = [120, 100, 90, 80, 75, 85, 130, 260, 420, 480, 380, 300,
                  260, 240, 260, 320, 420, 540, 610, 560, 430, 330, 240, 170]
PRED_PEAKS = [[8, 10], [18, 20]]


def jitter(base, pct=0.05):
    """在 base 基础上做 ±pct 的随机波动，模拟实时数据变化"""
    return max(0, int(base * (1 + random.uniform(-pct, pct))))


# ============ 各接口数据生成 ============

def api_overview():
    total_orders = sum(jitter(s["base_orders"], 0.08) for s in STATIONS)
    total_revenue = sum(jitter(s["base_revenue"], 0.06) for s in STATIONS)
    active = sum(1 for s in STATIONS if s["base_orders"] > 20)
    healthy = HEALTH_LEVELS[0]["value"] + random.randint(-2, 2)
    return {
        "monthOrders": total_orders,
        "monthKwh": jitter(52480, 0.04),
        "monthRevenue": total_revenue,
        "activeStations": active,
        "healthRate": healthy,
        "mom": {
            "monthOrders": round(random.uniform(6, 12), 1),
            "monthKwh": round(random.uniform(4, 9), 1),
            "monthRevenue": round(random.uniform(8, 15), 1),
            "activeStations": round(random.uniform(2, 6), 1),
            "healthRate": round(random.uniform(-3, 1), 1),
        },
    }


def api_revenue(days):
    days = int(days) if days else 7
    if days == 7:
        base = [14600, 15200, 14800, 16800, 17900, 18600, 19200]
    else:
        base = []
        v = 12000
        for _ in range(30):
            v = jitter(v, 0.04)
            base.append(v)
    return {"days": days, "values": [jitter(v, 0.03) for v in base]}


def api_pile_status():
    using = jitter(PILE_STATUS_BASE["using"], 0.1)
    idle = jitter(PILE_STATUS_BASE["idle"], 0.08)
    fault = jitter(PILE_STATUS_BASE["fault"], 0.3)
    total = using + idle + fault
    return [
        {"name": "在用", "value": using, "pct": round(using / total * 100, 1)},
        {"name": "闲置", "value": idle,  "pct": round(idle / total * 100, 1)},
        {"name": "故障", "value": fault, "pct": round(fault / total * 100, 1)},
    ]


def api_charging_24h():
    data = []
    for y, row in enumerate(HEAT_BASE):
        for x, val in enumerate(row):
            data.append([x, y, jitter(val, 0.1)])
    return {
        "regions": REGIONS,
        "hours": [f"{h}:00" for h in range(24)],
        "data": data,
    }


def api_top_revenue(limit):
    limit = int(limit) if limit else 10
    ranked = sorted(STATIONS, key=lambda s: s["base_revenue"], reverse=True)[:limit]
    return [
        {"name": s["name"], "revenue": jitter(s["base_revenue"], 0.05), "orders": jitter(s["base_orders"], 0.08)}
        for s in ranked
    ]


def api_users_trend():
    cumulative = []
    daily = []
    total = 8800
    for _ in range(30):
        inc = random.randint(120, 280)
        daily.append(inc)
        total += inc
        cumulative.append(total)
    return {
        "days": [f"9/{str(i+1).zfill(2)}" for i in range(30)],
        "cumulative": cumulative,
        "daily": daily,
    }


def api_users_levels():
    # 小幅波动但总和保持 100
    vals = [max(5, l["value"] + random.randint(-2, 2)) for l in USER_LEVELS]
    total = sum(vals)
    return [{"name": USER_LEVELS[i]["name"], "value": round(vals[i] / total * 100, 1)} for i in range(4)]


def api_stations_health():
    vals = [max(2, h["value"] + random.randint(-2, 2)) for h in HEALTH_LEVELS]
    total = sum(vals)
    return [{"name": HEALTH_LEVELS[i]["name"], "value": round(vals[i] / total * 100, 1)} for i in range(4)]


def api_prediction(hours):
    hours = int(hours) if hours else 24
    load = [jitter(v, 0.08) for v in PRED_LOAD_BASE[:hours]]
    labels = [f"+{i+1}h" for i in range(hours)]
    peaks = [[s, min(e, hours)] for s, e in PRED_PEAKS if s < hours]
    return {
        "stationName": PRED_STATION,
        "labels": labels,
        "load": load,
        "peaks": peaks,
    }


# ============ HTTP Handler ============

ROUTES = {
    "/api/overview":          lambda q: api_overview(),
    "/api/revenue":           lambda q: api_revenue(q.get("days", ["7"])[0]),
    "/api/pile-status":       lambda q: api_pile_status(),
    "/api/charging/24h":      lambda q: api_charging_24h(),
    "/api/stations/top-revenue": lambda q: api_top_revenue(q.get("limit", ["10"])[0]),
    "/api/users/trend":       lambda q: api_users_trend(),
    "/api/users/levels":      lambda q: api_users_levels(),
    "/api/stations/health":   lambda q: api_stations_health(),
    "/api/prediction":        lambda q: api_prediction(q.get("hours", ["24"])[0]),
}


class MockHandler(BaseHTTPRequestHandler):
    def _send_json(self, data, status=200):
        body = json.dumps(data, ensure_ascii=False).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()
        self.wfile.write(body)

    def do_OPTIONS(self):
        self._send_json({"ok": True})

    def do_GET(self):
        parsed = urlparse(self.path)
        path = parsed.path
        query = parse_qs(parsed.query)

        if path in ROUTES:
            try:
                data = ROUTES[path](query)
                self._send_json(data)
                print(f"  [{time.strftime('%H:%M:%S')}] 200 {path}")
            except Exception as e:
                self._send_json({"error": str(e)}, status=500)
                print(f"  [{time.strftime('%H:%M:%S')}] 500 {path} - {e}")
        elif path == "/":
            self._send_json({
                "service": "SCML 大屏 Mock 后端",
                "endpoints": list(ROUTES.keys()),
                "note": "每个接口每次请求都会返回带随机波动的假数据，模拟实时刷新。",
            })
        else:
            self._send_json({"error": "not found", "path": path}, status=404)

    def log_message(self, format, *args):
        pass  # 静默默认日志，用自定义打印


if __name__ == "__main__":
    # 监听 0.0.0.0：本地用 127.0.0.1 访问，远程用服务器 IP 访问都可以
    # 如果只想本地访问，改成 "127.0.0.1"
    HOST = "0.0.0.0"
    server = HTTPServer((HOST, PORT), MockHandler)
    print("=" * 60)
    print(f"  SCML 大屏 Mock 后端已启动")
    print(f"  本地访问: http://127.0.0.1:{PORT}")
    print(f"  远程访问: http://<服务器IP>:{PORT}")
    print(f"  接口数: {len(ROUTES)}")
    print(f"  按 Ctrl+C 停止")
    print("=" * 60)
    for ep in ROUTES:
        print(f"    GET {ep}")
    print("=" * 60)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\n服务器已停止。")
        server.server_close()
