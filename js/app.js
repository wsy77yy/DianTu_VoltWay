/* =====================================================================
   SCML 数据大屏 · 渲染逻辑（app.js）
   ---------------------------------------------------------------------
   本文件是 ECharts 的核心教学代码。如果你从没接触过 ECharts，
   请先读文件末尾的【ECharts 五步上手】总结，再回来看每个图表。

   大屏结构：
     loadAll()          → 并行请求所有接口数据
        └ 每个 renderXxx(chart, data) → 用 setOption 画图
   ===================================================================== */

/* ---------------------------------------------------------------------
   ① 全局状态
   --------------------------------------------------------------------- */
const state = {
  revenueDays: 7,   // 营收趋势：7 或 30 天
  predHours: 24,    // AI 预测：1 / 6 / 24 小时
  charts: {},       // 存放所有 echarts 实例，key = 容器 id
};

/* ---------------------------------------------------------------------
   ② ECharts 工具函数
   --------------------------------------------------------------------- */

/**
 * 获取（或创建）一个绑定到指定 DOM 容器的 echarts 实例。
 * - 第一次调用：echarts.init(dom) 创建实例
 * - 之后调用：直接返回已存在的实例（避免重复创建报错）
 * - 页面窗口变化时 resize()，让图表自适应容器
 */
function getChart(id) {
  if (!state.charts[id]) {
    const dom = document.getElementById(id);
    if (!dom) return null;
    state.charts[id] = echarts.init(dom);
  }
  return state.charts[id];
}

/** 全部图表实例绑定窗口 resize 事件 */
function bindResize() {
  window.addEventListener("resize", () => {
    Object.values(state.charts).forEach((chart) => chart.resize());
  });
}

/**
 * 深色大屏的公共文本/提示配置，保证字体在深色背景下清晰
 */
function baseText() {
  return {
    textStyle: { color: "#a78bb8" },          // 坐标轴文字颜色
    tooltip: {                                // 悬浮提示框
      backgroundColor: "rgba(8,25,45,0.9)",
      borderColor: "#e879f9",
      textStyle: { color: "#eafcff", fontSize: 13 },
    },
  };
}

/* ---------------------------------------------------------------------
   ③ 核心运营指标 KPI（需求 #58）
   --------------------------------------------------------------------- */
function renderKpis(data) {
  const cards = [
    { label: "本月订单数",   value: fmt(data.monthOrders) + " 单",   mom: data.mom.monthOrders },
    { label: "充电总度数",   value: fmt(data.monthKwh) + " 度",      mom: data.mom.monthKwh },
    { label: "充电总金额",   value: "¥" + fmt(data.monthRevenue),    mom: data.mom.monthRevenue },
    { label: "活跃充电站数", value: fmt(data.activeStations) + " 座", mom: data.mom.activeStations },
    { label: "电池健康率",   value: fmt(data.healthRate) + "%",      mom: data.mom.healthRate },
  ];
  document.getElementById("kpiRow").innerHTML = cards
    .map((c) => {
      const cls = c.mom > 0 ? "up" : c.mom < 0 ? "down" : "flat";
      const arrow = c.mom > 0 ? "▲" : c.mom < 0 ? "▼" : "—";
      return `
        <div class="kpi-card">
          <div class="kpi-label">${c.label}</div>
          <div class="kpi-value">${c.value}</div>
          <div class="kpi-mom ${cls}">较上月 ${arrow} ${Math.abs(c.mom)}%</div>
        </div>`;
    })
    .join("");
}

/** 千分位格式化 */
function fmt(n) {
  return Number(n).toLocaleString("zh-CN");
}

/* ---------------------------------------------------------------------
   ④ 营收趋势折线图（需求 #56/#57，对应 qryRevenueStat）
   ---------------------------------------------------------------------
   ECharts 折线图要点：
   - xAxis.data 放横轴类别（第1天、第2天...）
   - series: [{ type: "line", data: [...] }]
   - 可以 smooth: true 让线条平滑；areaStyle 给线下加渐变面积
   --------------------------------------------------------------------- */
function renderRevenueChart(values) {
  const chart = getChart("revenueChart");
  chart.setOption({
    ...baseText(),
    tooltip: {
      ...baseText().tooltip,
      trigger: "axis",                       // 十字线联动提示
      formatter: (params) => {
        const p = params[0];
        return `${p.axisValue}<br/><b>营收：¥${fmt(p.value)}</b>`;
      },
    },
    grid: { left: 55, right: 20, top: 30, bottom: 26 },  // 绘图区四边留白
    xAxis: {
      type: "category",
      boundaryGap: false,                    // 折线从最左开始，不留空隙
      data: values.map((_, i) => (state.revenueDays === 7 ? `D${i + 1}` : `${i + 1}日`)),
      axisLine: { lineStyle: { color: "#3a2158" } },
      axisLabel: { color: "#a78bb8" },
    },
    yAxis: {
      type: "value",
      name: "元",
      splitLine: { lineStyle: { color: "rgba(96,52,140,0.35)" } },
      axisLabel: { color: "#a78bb8", formatter: (v) => (v >= 10000 ? `${v / 10000}万` : v) },
    },
    series: [
      {
        name: "营收",
        type: "line",
        data: values,
        smooth: true,
        symbol: "circle",
        symbolSize: 6,
        lineStyle: { color: "#e879f9", width: 2.5 },
        itemStyle: { color: "#e879f9" },
        areaStyle: {
          // 线下渐变面积
          color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [
            { offset: 0, color: "rgba(232,121,249,0.35)" },
            { offset: 1, color: "rgba(232,121,249,0.02)" },
          ]),
        },
      },
    ],
  });
}

/* ---------------------------------------------------------------------
   ⑤ 电桩状态分布 环形饼图（需求 #56，对应 qryPileStatus）
   ---------------------------------------------------------------------
   饼图要点：
   - series: [{ type: "pie", radius: ["45%","70%"], data: [{name,value}...] }]
   - radius 用数组 ["内径","外径"] 就成了环形图（donut）
   --------------------------------------------------------------------- */
function renderPileStatusChart(list) {
  const chart = getChart("pileStatusChart");
  const colors = ["#e879f9", "#c084fc", "#fb7185"];   // 在用/闲置/故障
  chart.setOption({
    ...baseText(),
    tooltip: { ...baseText().tooltip, formatter: "{b}：{c} 台（{d}%）" },
    color: colors,
    legend: {
      bottom: 0,
      textStyle: { color: "#a78bb8", fontSize: 12 },
      icon: "circle",
    },
    series: [
      {
        type: "pie",
        radius: ["42%", "68%"],              // 环形图
        center: ["50%", "45%"],
        label: { color: "#eafcff", formatter: "{b}\n{d}%" },
        labelLine: { lineStyle: { color: "#2a5a7a" } },
        data: list.map((item) => ({ name: item.name, value: item.value })),
      },
    ],
  });
}

/* ---------------------------------------------------------------------
   ⑥ 用户级别分类 环形饼图（需求 #59）
   --------------------------------------------------------------------- */
function renderLevelChart(list) {
  const chart = getChart("levelChart");
  const colors = ["#c084fc", "#e879f9", "#fbbf24", "#f472b6"]; // 普通/银卡/金卡/钻石
  chart.setOption({
    ...baseText(),
    tooltip: { ...baseText().tooltip, formatter: "{b}：{c}%（占比）" },
    color: colors,
    legend: {
      bottom: 0,
      textStyle: { color: "#a78bb8", fontSize: 12 },
      icon: "circle",
    },
    series: [
      {
        type: "pie",
        radius: ["42%", "68%"],
        center: ["50%", "45%"],
        label: { color: "#eafcff", formatter: "{b} {d}%" },
        labelLine: { lineStyle: { color: "#2a5a7a" } },
        data: list.map((item) => ({ name: item.name, value: item.value })),
      },
    ],
  });
}

/* ---------------------------------------------------------------------
   ⑦ 24h 充电量分布 热力图（需求 #60）
   ---------------------------------------------------------------------
   热力图（heatmap）要点：
   - 需要两个坐标轴：x 轴 = 24 小时，y 轴 = 区域
   - 需要 visualMap：把数值映射成颜色深浅
   - series.data 的每一项是 [x下标, y下标, 值]
   --------------------------------------------------------------------- */
function renderHeatChart(data) {
  const chart = getChart("heatChart");
  chart.setOption({
    ...baseText(),
    tooltip: {
      ...baseText().tooltip,
      position: "top",
      formatter: (p) => `${data.regions[p.value[1]]} ${data.hours[p.value[0]]}<br/><b>${p.value[2]} 度</b>`,
    },
    grid: { left: 70, right: 20, top: 20, bottom: 60 },
    xAxis: {
      type: "category",
      data: data.hours,
      splitArea: { show: true },
      axisLine: { lineStyle: { color: "#3a2158" } },
      axisLabel: { color: "#a78bb8", interval: 3 },  // 每隔3个显示一个，避免太密
    },
    yAxis: {
      type: "category",
      data: data.regions,
      splitArea: { show: true },
      axisLine: { lineStyle: { color: "#3a2158" } },
      axisLabel: { color: "#a78bb8" },
    },
    visualMap: {
      min: 0,
      max: 100,
      calculable: true,                      // 出现可拖动的颜色条
      orient: "horizontal",
      left: "center",
      bottom: 5,
      text: ["高", "低"],
      textStyle: { color: "#a78bb8" },
      inRange: { color: ["#150b28", "#3b1d6e", "#5b21b6", "#8b5cf6", "#c084fc", "#e879f9", "#fbbf24"] },
    },
    series: [
      {
        type: "heatmap",
        data: data.data,
        label: { show: false },
        emphasis: { itemStyle: { shadowBlur: 10, shadowColor: "rgba(0,0,0,0.5)" } },
      },
    ],
  });
}

/* ---------------------------------------------------------------------
   ⑧ AI 充电负荷预测 折线 + 高峰预警（需求 #63，对应 qryLoadPred）
   ---------------------------------------------------------------------
   markArea 用于在图中框出"高峰预警时段"：
   - series 里的 markArea.data 每一项是 [{xAxis:起点},{xAxis:终点}]
   - 会在两点之间画一个半透明色块，表示该时段是高峰预警区
   --------------------------------------------------------------------- */
function renderPredChart(data) {
  const chart = getChart("predChart");
  // 把高峰区间转成 markArea 需要的格式
  const markAreas = data.peaks.map(([start, end]) => [
    { xAxis: data.labels[start], itemStyle: { color: "rgba(251,113,133,0.20)" } },
    { xAxis: data.labels[end - 1] },
  ]);

  chart.setOption({
    ...baseText(),
    tooltip: {
      ...baseText().tooltip,
      trigger: "axis",
      formatter: (params) => {
        const p = params[0];
        return `${p.axisValue}<br/><b>预测负荷：${p.value} kW</b>`;
      },
    },
    grid: { left: 60, right: 30, top: 40, bottom: 26 },
    xAxis: {
      type: "category",
      data: data.labels,
      boundaryGap: false,
      axisLine: { lineStyle: { color: "#3a2158" } },
      axisLabel: { color: "#a78bb8" },
    },
    yAxis: {
      type: "value",
      name: "kW",
      splitLine: { lineStyle: { color: "rgba(96,52,140,0.35)" } },
      axisLabel: { color: "#a78bb8" },
    },
    series: [
      {
        name: "负荷预测",
        type: "line",
        data: data.load,
        smooth: true,
        symbol: "circle",
        symbolSize: 5,
        lineStyle: { color: "#f472b6", width: 2.5 },
        itemStyle: { color: "#f472b6" },
        areaStyle: {
          color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [
            { offset: 0, color: "rgba(244,114,182,0.35)" },
            { offset: 1, color: "rgba(244,114,182,0.02)" },
          ]),
        },
        // 高峰预警区域（红色半透明色块）
        markArea: {
          silent: true,
          data: markAreas,
          label: { show: true, position: "insideTop", color: "#fb7185", formatter: "⚠ 高峰预警" },
        },
        // 在最高点标出数值
        markPoint: {
          data: [{ type: "max", name: "峰值" }],
          itemStyle: { color: "#fb7185" },
          label: { color: "#eafcff", formatter: (p) => `${p.value}` },
        },
      },
    ],
  });
}

/* ---------------------------------------------------------------------
   ⑨ 营收 TOP10 电站 横向柱状图（需求 #62）
   ---------------------------------------------------------------------
   横向柱状图要点：
   - xAxis 和 yAxis 对调：把站名放 y 轴（category），数值放 x 轴（value）
   - 数据量大的时候横向条更易读，站名也不会被挤掉
   --------------------------------------------------------------------- */
function renderTopStationChart(list) {
  const chart = getChart("topStationChart");
  const names = list.map((s) => s.name);
  const revenues = list.map((s) => s.revenue);
  chart.setOption({
    ...baseText(),
    tooltip: {
      ...baseText().tooltip,
      trigger: "axis",
      axisPointer: { type: "shadow" },
      formatter: (params) => {
        const p = params[0];
        const item = list[p.dataIndex];
        return `${p.name}<br/>营收：¥${fmt(p.value)}<br/>订单数：${item.orders} 单`;
      },
    },
    grid: { left: 130, right: 50, top: 10, bottom: 20 },
    xAxis: {
      type: "value",
      splitLine: { lineStyle: { color: "rgba(96,52,140,0.35)" } },
      axisLabel: { color: "#a78bb8", formatter: (v) => (v >= 10000 ? `${v / 10000}万` : v) },
    },
    yAxis: {
      type: "category",
      data: names.reverse(),                 // 反转让营收最高的在最上面
      axisLine: { lineStyle: { color: "#3a2158" } },
      axisLabel: { color: "#a78bb8", fontSize: 12, width: 110, overflow: "truncate" },
    },
    series: [
      {
        type: "bar",
        data: revenues.reverse(),
        barWidth: 12,
        itemStyle: {
          borderRadius: [0, 6, 6, 0],
          // 用渐变色让条形更好看
          color: new echarts.graphic.LinearGradient(0, 0, 1, 0, [
            { offset: 0, color: "#5b21b6" },
            { offset: 1, color: "#e879f9" },
          ]),
        },
        label: { show: true, position: "right", color: "#a78bb8", formatter: (p) => `¥${fmt(p.value)}` },
      },
    ],
  });
}

/* ---------------------------------------------------------------------
   ⑩ 用户分析 双轴图（需求 #59）：累计用户(折线) + 每日新增(柱状)
   ---------------------------------------------------------------------
   双轴图要点：
   - yAxis 写成数组 [{左侧轴}, {右侧轴}]，右侧用 axisPosition:"right"
   - 两个 series 分别指定 yAxisIndex: 0 和 1
   --------------------------------------------------------------------- */
function renderUserChart(data) {
  const chart = getChart("userChart");
  chart.setOption({
    ...baseText(),
    tooltip: {
      ...baseText().tooltip,
      trigger: "axis",
      formatter: (params) => {
        let html = `<b>${params[0].axisValue}</b>`;
        params.forEach((p) => {
          html += `<br/>${p.marker}${p.seriesName}：${fmt(p.value)}`;
        });
        return html;
      },
    },
    legend: { top: 0, textStyle: { color: "#a78bb8", fontSize: 12 }, icon: "circle" },
    grid: { left: 60, right: 60, top: 32, bottom: 26 },
    xAxis: {
      type: "category",
      data: data.days,
      axisLine: { lineStyle: { color: "#3a2158" } },
      axisLabel: { color: "#a78bb8", interval: 4 },
    },
    yAxis: [
      {
        type: "value",
        name: "累计用户",
        splitLine: { lineStyle: { color: "rgba(96,52,140,0.35)" } },
        axisLabel: { color: "#a78bb8", formatter: (v) => (v >= 10000 ? `${(v / 10000).toFixed(1)}万` : v) },
      },
      {
        type: "value",
        name: "每日新增",
        axisPosition: "right",               // 第二个 y 轴放到右侧
        splitLine: { show: false },
        axisLabel: { color: "#a78bb8" },
      },
    ],
    series: [
      {
        name: "累计用户",
        type: "line",
        yAxisIndex: 0,                       // 用第 0 个 y 轴
        data: data.cumulative,
        smooth: true,
        symbol: "none",
        lineStyle: { color: "#e879f9", width: 2.5 },
      },
      {
        name: "每日新增",
        type: "bar",
        yAxisIndex: 1,                       // 用第 1 个（右侧）y 轴
        data: data.daily,
        barWidth: 6,
        itemStyle: {
          color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [
            { offset: 0, color: "rgba(168,85,247,0.9)" },
            { offset: 1, color: "rgba(168,85,247,0.1)" },
          ]),
        },
      },
    ],
  });
}

/* ---------------------------------------------------------------------
   ⑪ 电池健康等级分布 横向条形图（需求 #61）
   ---------------------------------------------------------------------
   与 TOP10 类似，是简化版横向柱状图，但条形用不同颜色区分等级。
   --------------------------------------------------------------------- */
function renderHealthChart(list) {
  const chart = getChart("healthChart");
  const colorMap = { 优秀: "#4ade80", 良好: "#c084fc", 一般: "#fbbf24", 差: "#fb7185" };
  chart.setOption({
    ...baseText(),
    tooltip: { ...baseText().tooltip, formatter: "{b}：{c}%" },
    grid: { left: 60, right: 40, top: 10, bottom: 20 },
    xAxis: {
      type: "value",
      max: 60,
      splitLine: { lineStyle: { color: "rgba(96,52,140,0.35)" } },
      axisLabel: { color: "#a78bb8", formatter: "{value}%" },
    },
    yAxis: {
      type: "category",
      data: list.map((h) => h.name).reverse(),
      axisLine: { lineStyle: { color: "#3a2158" } },
      axisLabel: { color: "#a78bb8", fontSize: 13 },
    },
    series: [
      {
        type: "bar",
        data: list
          .slice()
          .reverse()
          .map((h) => ({
            value: h.value,
            itemStyle: { color: colorMap[h.name] || "#e879f9", borderRadius: [0, 6, 6, 0] },
          })),
        barWidth: 14,
        label: { show: true, position: "right", color: "#eafcff", formatter: "{c}%" },
      },
    ],
  });
}

/* ---------------------------------------------------------------------
   ⑫ 主流程：并行拉取所有接口数据并渲染
   ---------------------------------------------------------------------
   Promise.all 让所有接口请求同时发出，全部返回后一次性刷新页面。
   这就是"HTTP API 定时轮询"的轮询核心。
   --------------------------------------------------------------------- */
async function loadAll() {
  try {
    const [
      overview,
      revenue,
      pileStatus,
      heat,
      topStations,
      userTrend,
      userLevels,
      health,
      prediction,
    ] = await Promise.all([
      SCML_API.fetchOverview(),
      SCML_API.fetchRevenue(state.revenueDays),
      SCML_API.fetchPileStatus(),
      SCML_API.fetchHeat24h(),
      SCML_API.fetchTopStations(),
      SCML_API.fetchUserTrend(),
      SCML_API.fetchUserLevels(),
      SCML_API.fetchHealth(),
      SCML_API.fetchPrediction(state.predHours),
    ]);

    renderKpis(overview);
    renderRevenueChart(revenue.values);
    renderPileStatusChart(pileStatus);
    renderHeatChart(heat);
    renderTopStationChart(topStations);
    renderUserChart(userTrend);
    renderLevelChart(userLevels);
    renderHealthChart(health);
    renderPredChart(prediction);

    // 更新底部"数据更新时间"
    const now = new Date();
    document.getElementById("updateTime").textContent =
      now.toLocaleString("zh-CN", { hour12: false });
  } catch (err) {
    console.error("数据加载失败：", err);
  }
}

/* ---------------------------------------------------------------------
   ⑬ 顶部时钟
   --------------------------------------------------------------------- */
function startClock() {
  const week = ["日", "一", "二", "三", "四", "五", "六"];
  function tick() {
    const now = new Date();
    document.getElementById("clock").textContent = now.toLocaleTimeString("zh-CN", { hour12: false });
    document.getElementById("weekday").textContent =
      `${now.getFullYear()}年${now.getMonth() + 1}月${now.getDate()}日 星期${week[now.getDay()]}`;
  }
  tick();
  setInterval(tick, 1000);
}

/* ---------------------------------------------------------------------
   ⑭ 事件绑定：分段切换按钮 + 立即刷新 + 定时轮询
   --------------------------------------------------------------------- */
function bindEvents() {
  // 营收 7/30 切换
  document.getElementById("revenueSeg").addEventListener("click", async (e) => {
    const btn = e.target.closest("button");
    if (!btn) return;
    state.revenueDays = Number(btn.dataset.days);
    document.querySelectorAll("#revenueSeg button").forEach((b) => b.classList.toggle("active", b === btn));
    const revenue = await SCML_API.fetchRevenue(state.revenueDays);
    renderRevenueChart(revenue.values);
  });

  // AI 预测 1/6/24 切换
  document.getElementById("predSeg").addEventListener("click", async (e) => {
    const btn = e.target.closest("button");
    if (!btn) return;
    state.predHours = Number(btn.dataset.hours);
    document.querySelectorAll("#predSeg button").forEach((b) => b.classList.toggle("active", b === btn));
    const pred = await SCML_API.fetchPrediction(state.predHours);
    renderPredChart(pred);
  });

  // 立即刷新
  document.getElementById("refreshBtn").addEventListener("click", loadAll);

  // 定时轮询（模拟近实时刷新）：每 30 秒拉一次全部数据
  setInterval(loadAll, 30000);
}

/* ---------------------------------------------------------------------
   ⑮ 启动
   --------------------------------------------------------------------- */
bindResize();
startClock();
bindEvents();
loadAll();

/* =====================================================================
   【ECharts 五步上手】写给零基础的你
   =====================================================================
   1. 引入：<script src="echarts.min.js"></script>（本项目已放本地）
   2. 准备容器：HTML 里放一个 <div id="chartId" style="width/height"></div>
   3. 初始化：const chart = echarts.init(div)  ← 拿到一个"画布"对象
   4. 配置：chart.setOption({...})  ← 给画布一份完整的"配置说明书"
   5. 更新：再调 setOption({...新配置}) 即可刷新数据（图表自动重绘）
   ---------------------------------------------------------------
   核心概念（setOption 里的字段）：
   - xAxis / yAxis ：坐标轴（category=类别轴 / value=数值轴）
   - series        ：数据系列，type 决定图表类型
                    line 折线 | bar 柱状 | pie 饼图 | heatmap 热力图
   - tooltip       ：悬浮提示
   - legend        ：图例
   - grid          ：绘图区边距（避免文字被裁掉）
   - color         ：全局配色数组
   - visualMap     ：热力图/散点图专用的"数值→颜色"映射
   - markArea/markPoint：在图上标注区域/标记点（本项目用于"高峰预警"）
   ---------------------------------------------------------------
   本项目 8 张图 = 你要用到的全部图表类型：
   折线(line) ×3  | 饼图/环形(pie) ×2 | 柱状(bar) ×2 | 热力图(heatmap) ×1
   ===================================================================== */
