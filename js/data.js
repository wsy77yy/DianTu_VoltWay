/* =====================================================================
   SCML 数据大屏 · 数据层（真实 HTTP API 版）
   ---------------------------------------------------------------------
   本文件负责大屏与后端的数据交互。当前指向本地 Mock 服务器
   （mock_server.py，端口 8766），用于联调测试。

   等 PC 服务器端（PRL）真实后端就绪后，只需修改下面的 API_BASE
   为真实服务器地址，大屏代码完全不用改。

   接口契约（与 mock_server.py 一一对应）：
     GET /api/overview                  核心运营指标
     GET /api/revenue?days=7|30        营收趋势
     GET /api/pile-status               电桩状态分布
     GET /api/charging/24h              24h 充电量热力图
     GET /api/stations/top-revenue?limit=10   营收 TOP 电站
     GET /api/users/trend               用户分析（累计/每日新增）
     GET /api/users/levels              用户级别占比
     GET /api/stations/health           电池健康等级
     GET /api/prediction?hours=1|6|24  AI 负荷预测
   ===================================================================== */

// ============ 后端地址（联调时改成真实服务器地址） ============
const API_BASE = "http://127.0.0.1:8766";

// ============ 通用 fetch 封装 ============
/**
 * 发起 GET 请求并解析 JSON。
 * - 成功：返回解析后的数据对象
 * - 失败：console.error 并返回 null（调用方需做降级处理）
 */
async function apiGet(path) {
  try {
    const res = await fetch(API_BASE + path);
    if (!res.ok) throw new Error(`HTTP ${res.status}`);
    return await res.json();
  } catch (err) {
    console.error(`[API] 请求失败 ${path}:`, err);
    return null;
  }
}

/**
 * 接口降级：当后端不可用时，返回一份兜底数据，保证大屏不白屏。
 * 这些兜底数据与 mock_server.py 的基础数据一致。
 */
const FALLBACK = {
  overview: {
    monthOrders: 1895, monthKwh: 52480, monthRevenue: 498600,
    activeStations: 10, healthRate: 46,
    mom: { monthOrders: 9.8, monthKwh: 7.2, monthRevenue: 12.4, activeStations: 4, healthRate: -1.6 },
  },
  revenue7:  { days: 7,  values: [14600, 15200, 14800, 16800, 17900, 18600, 19200] },
  revenue30: { days: 30, values: Array.from({length:30},(_,i)=>12000+i*200) },
  pileStatus: [
    { name: "在用", value: 46, pct: 37.1 },
    { name: "闲置", value: 72, pct: 58.1 },
    { name: "故障", value: 6,  pct: 4.8 },
  ],
  heat: (() => {
    const regions = ["海淀区","朝阳区","东城区","西城区","丰台区","昌平区"];
    const base = [
      [22,18,16,14,12,20,35,58,72,68,55,48,44,39,42,51,63,76,91,88,74,62,50,29],
      [18,15,13,11,10,16,28,46,58,54,44,38,35,31,33,40,50,61,74,70,59,49,40,24],
      [12,10,9,8,7,11,20,33,42,39,32,27,25,22,24,29,36,44,53,50,42,35,29,17],
      [10,9,8,7,6,9,17,28,36,33,27,23,21,19,20,25,31,38,46,43,36,30,25,15],
      [14,12,10,9,8,12,22,36,46,42,35,30,27,24,26,32,40,49,59,55,46,39,32,19],
      [8,7,6,6,5,8,14,23,30,27,22,19,17,15,17,21,26,32,38,36,30,25,20,12],
    ];
    const data = [];
    base.forEach((row,y)=>row.forEach((v,x)=>data.push([x,y,v])));
    return { regions, hours: Array.from({length:24},(_,i)=>`${i}:00`), data };
  })(),
  topStations: [
    { name: "国贸城市补能站", revenue: 103900, orders: 386 },
    { name: "中关村软件园超级充电站", revenue: 84200, orders: 312 },
    { name: "奥林匹克公园快充站", revenue: 61600, orders: 244 },
    { name: "望京智慧充电站", revenue: 55800, orders: 208 },
    { name: "五道口科技充电站", revenue: 47200, orders: 189 },
    { name: "亦庄开发区补能站", revenue: 39800, orders: 156 },
    { name: "石景山万达快充站", revenue: 33400, orders: 128 },
    { name: "通州运河商务充电站", revenue: 28100, orders: 104 },
    { name: "西二旗高新充电站", revenue: 24700, orders: 96 },
    { name: "顺义空港快充站", revenue: 19900, orders: 72 },
  ],
  userTrend: (() => {
    let total = 8800; const cumulative=[], daily=[];
    for(let i=0;i<30;i++){ const inc=120+Math.round(Math.random()*160); daily.push(inc); total+=inc; cumulative.push(total); }
    return { days: Array.from({length:30},(_,i)=>`9/${String(i+1).padStart(2,"0")}`), cumulative, daily };
  })(),
  userLevels: [{name:"普通",value:42},{name:"银卡",value:26},{name:"金卡",value:18},{name:"钻石",value:14}],
  health: [{name:"优秀",value:46},{name:"良好",value:34},{name:"一般",value:15},{name:"差",value:5}],
  prediction: {
    stationName: "中关村软件园超级充电站",
    labels: Array.from({length:24},(_,i)=>`+${i+1}h`),
    load: [120,100,90,80,75,85,130,260,420,480,380,300,260,240,260,320,420,540,610,560,430,330,240,170],
    peaks: [[8,10],[18,20]],
  },
};

/* =====================================================================
   接口函数：每个函数返回 Promise，resolve 为后端返回的 JSON 数据。
   如果后端不可用，自动返回 FALLBACK 兜底数据，保证大屏不白屏。
   ===================================================================== */

/** GET /api/overview —— 核心运营指标 */
async function fetchOverview() {
  const data = await apiGet("/api/overview");
  return data || FALLBACK.overview;
}

/** GET /api/revenue?days=7|30 —— 营收趋势 */
async function fetchRevenue(days) {
  const data = await apiGet(`/api/revenue?days=${days}`);
  return data || (days === 30 ? FALLBACK.revenue30 : FALLBACK.revenue7);
}

/** GET /api/pile-status —— 电桩状态分布 */
async function fetchPileStatus() {
  const data = await apiGet("/api/pile-status");
  return data || FALLBACK.pileStatus;
}

/** GET /api/charging/24h —— 24h 充电量热力图 */
async function fetchHeat24h() {
  const data = await apiGet("/api/charging/24h");
  return data || FALLBACK.heat;
}

/** GET /api/stations/top-revenue?limit=10 —— 营收 TOP10 电站 */
async function fetchTopStations() {
  const data = await apiGet("/api/stations/top-revenue?limit=10");
  return data || FALLBACK.topStations;
}

/** GET /api/users/trend —— 用户分析（累计用户/每日新增） */
async function fetchUserTrend() {
  const data = await apiGet("/api/users/trend");
  return data || FALLBACK.userTrend;
}

/** GET /api/users/levels —— 用户级别占比 */
async function fetchUserLevels() {
  const data = await apiGet("/api/users/levels");
  return data || FALLBACK.userLevels;
}

/** GET /api/stations/health —— 电池健康等级 */
async function fetchHealth() {
  const data = await apiGet("/api/stations/health");
  return data || FALLBACK.health;
}

/** GET /api/prediction?hours=1|6|24 —— AI 负荷预测 */
async function fetchPrediction(hours) {
  const data = await apiGet(`/api/prediction?hours=${hours}`);
  return data || FALLBACK.prediction;
}

/* =====================================================================
   暴露为全局对象，供 app.js 调用
   ===================================================================== */
window.SCML_API = {
  fetchOverview,
  fetchRevenue,
  fetchPileStatus,
  fetchHeat24h,
  fetchTopStations,
  fetchUserTrend,
  fetchUserLevels,
  fetchHealth,
  fetchPrediction,
};
