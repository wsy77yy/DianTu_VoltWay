const stations = [
  {
    id: 1,
    name: "中关村软件园超级充电站",
    district: "海淀区",
    address: "北京市海淀区东北旺西路 8 号",
    price: 1.18,
    distance: 1.2,
    lng: 116.28261,
    lat: 40.05185,
    rating: 4.8,
    total: 16,
    revenue: 84200,
    orders: 312,
    onlineRate: 0.94,
    predFree1h: 7,
    predFree2h: 8,
    heat: 83,
    health: 92,
  },
  {
    id: 2,
    name: "奥林匹克公园快充站",
    district: "朝阳区",
    address: "北京市朝阳区国家体育场北路 1 号",
    price: 1.05,
    distance: 2.8,
    lng: 116.39748,
    lat: 39.99287,
    rating: 4.5,
    total: 12,
    revenue: 61600,
    orders: 244,
    onlineRate: 0.88,
    predFree1h: 3,
    predFree2h: 2,
    heat: 91,
    health: 86,
  },
  {
    id: 3,
    name: "国贸城市补能站",
    district: "朝阳区",
    address: "北京市朝阳区建国门外大街 1 号",
    price: 1.32,
    distance: 4.1,
    lng: 116.46135,
    lat: 39.90918,
    rating: 4.9,
    total: 20,
    revenue: 103900,
    orders: 386,
    onlineRate: 0.91,
    predFree1h: 11,
    predFree2h: 10,
    heat: 76,
    health: 90,
  },
];

const piles = [
  { id: "ZGC-A01", stationId: 1, type: "直流快充", form: "一桩双充 / 立式", power: 120, status: "闲置", count: 286, hours: 932, price: 1.18 },
  { id: "ZGC-A02", stationId: 1, type: "直流超快充", form: "一桩一充 / 立式", power: 180, status: "使用中", count: 311, hours: 1004, price: 1.28 },
  { id: "ZGC-B01", stationId: 1, type: "交流慢充", form: "一桩一充 / 壁挂式", power: 7, status: "闲置", count: 168, hours: 1388, price: 0.92 },
  { id: "AOP-C03", stationId: 2, type: "直流快充", form: "一桩双充 / 立式", power: 90, status: "闲置", count: 231, hours: 771, price: 1.05 },
  { id: "AOP-C04", stationId: 2, type: "直流快充", form: "一桩双充 / 立式", power: 90, status: "故障", count: 198, hours: 688, price: 1.05 },
  { id: "AOP-D01", stationId: 2, type: "换电柜", form: "多仓位 / 立式", power: 60, status: "闲置", count: 119, hours: 402, price: 1.16 },
  { id: "CBD-E01", stationId: 3, type: "直流超快充", form: "一桩一充 / 立式", power: 240, status: "闲置", count: 358, hours: 1192, price: 1.32 },
  { id: "CBD-E02", stationId: 3, type: "直流快充", form: "一桩双充 / 立式", power: 120, status: "已预约", count: 245, hours: 852, price: 1.22 },
  { id: "CBD-F01", stationId: 3, type: "交流慢充", form: "一桩一充 / 壁挂式", power: 11, status: "闲置", count: 143, hours: 991, price: 0.88 },
];

const reviews = [
  { stationId: 1, user: "用户 8000", score: 5, text: "空闲桩数量准，导航过去很快。" },
  { stationId: 1, user: "用户 2391", score: 4, text: "快充速度不错，高峰期建议提前预约。" },
  { stationId: 2, user: "用户 1186", score: 4, text: "价格便宜，晚高峰会排队。" },
  { stationId: 3, user: "用户 5520", score: 5, text: "位置方便，超快充体验很好。" },
];

const users = [
  { id: 101, phone: "13800138000", password: "123456", name: "用户8000", avatarColor: "#f17796", balance: 126.5, level: "金卡", status: "正常", registerAt: "2026-09-01", totalSpend: 1268, hours: 76 },
  { id: 102, phone: "13700001234", password: "123456", name: "用户1234", avatarColor: "#58a6df", balance: 32.8, level: "普通", status: "正常", registerAt: "2026-09-02", totalSpend: 188, hours: 12 },
  { id: 103, phone: "13688886666", password: "123456", name: "用户6666", avatarColor: "#5f6570", balance: 6.2, level: "银卡", status: "冻结", registerAt: "2026-08-28", totalSpend: 529, hours: 33 },
];

const avatarPalette = ["#f17796", "#58a6df", "#efb7c6", "#7aa7c8", "#5f6570"];

const adminRequests = [
  { id: 1, account: "ops_wsy", role: "电站管理员", applyAt: "2026-09-03 19:20", status: "待审批" },
  { id: 2, account: "station_hn", role: "运维值守", applyAt: "2026-09-03 20:05", status: "待审批" },
];

const revenueSeries = {
  7: [8200, 9100, 7600, 11200, 12300, 13100, 14600],
  30: [68, 74, 71, 82, 88, 93, 86, 79, 84, 91, 96, 102],
};

const pmRequirements = [
  ["定位（区域选择/手动输入地址）", "用户进入系统后，通过下拉选择区域或手动输入地址进行定位，软件层面模拟 GPS。", "△"],
  ["地址转经纬度", "调用腾讯地图 Web API，将定位地址/区域转换为经纬度坐标。", "△"],
  ["多维排序选项", "支持距离优先、价格优先、好评优先多种排序方式切换。", "×"],
  ["按距离排序展示电站列表", "列表展示站名、充电价格、电桩总数/空闲数量、距离。", "×"],
  ["电站详情查看", "点击充电站查看该站所有电桩的编号、类型、状态、功率。", "×"],
  ["电桩类型详细分类", "展示交流/直流、充电桩/换电柜、一桩一充/多充、立式/壁挂式、慢充/快充/超快充。", "×"],
  ["地图标记充电站", "在地图上以图标标记各充电站位置，直观查看周边站点分布。", "×"],
  ["7日内用户评价", "详情页展示 7 日内评价，用户可对已完成订单的电站评价。", "×"],
  ["起终点设置与路线规划", "在站点列表点击距离/导航，设置起点和终点并跳转路线规划。", "×"],
  ["出行方式选择", "提供驾车、步行多种出行方式选择。", "×"],
  ["手机号免密登录", "输入 11 位手机号，已存在直接登录，不存在自动创建用户。", "△"],
  ["手机号+密码注册", "注册要求手机号+密码，支持验证码验证，不存在则创建默认头像/昵称用户。", "×"],
  ["自动注册（默认昵称）", "手机号不存在时自动创建新用户，默认昵称为用户+手机号后 4 位。", "△"],
  ["信息展示与修改", "登录后展示头像、昵称、钱包余额，并支持本地头像和昵称修改。", "△"],
  ["用户级别体系", "根据累计消费自动计算普通/银卡/金卡/钻石，并享受不同优惠。", "×"],
  ["余额充值", "输入充值金额，点击充值按钮模拟支付成功，余额实时更新。", "×"],
  ["退出账号", "用户退出当前账号，清除本地登录状态，返回登录页面。", "△"],
  ["充电前检查", "进入充电页时查询未完成订单，如有则强制跳转结算页。", "×"],
  ["预约—充电—计费—结算全流程", "选择空闲电桩完成预约、充电、计费、结算流程。", "×"],
  ["开启充电订单", "扫码或手动输入桩号开启订单，选择充满/按时长/按价格/按度数。", "×"],
  ["进行中订单查看", "查看位置、时间、度数、价格，可结束订单、延时结束或请求代结算。", "×"],
  ["已完成订单与评价", "查看历史订单的位置、时间、价格、度数，并可评价。", "×"],
  ["预约订单", "预约空闲电桩，要求 2h 内到达扫码确认，超时自动取消。", "×"],
  ["多种支付方式", "支持支付宝/微信支付模拟、余额支付，结算时显示详细账单。", "×"],
  ["主页/订单/我的Tab导航", "用户端底部设置主页、订单、我的三个 Tab，实现页面切换。", "△"],
  ["AI智能推荐", "根据好评、价格、距离和 PE2 预测 1-2h 空闲状态推荐最优电站。", "×"],
];

const apiList = [
  ["verifyUserLogin", "int verifyUserLogin(char *phone, USER_INFO *user)", "手机号免密登录，自动注册新用户"],
  ["qryStationList", "int qryStationList(double lng, double lat, STATION_INFO *list, int *count)", "按位置查询附近充电站"],
  ["qryStationDetail", "int qryStationDetail(int stationId, PILE_INFO *list, int *count)", "查询站内电桩明细"],
  ["startCharging", "int startCharging(int userId, int pileId)", "生成订单并下发开始充电指令"],
  ["qryUnfinishedOrder", "int qryUnfinishedOrder(int userId, ORDER_INFO *order)", "充电前检查未完成订单"],
  ["settleOrder", "int settleOrder(int orderId, double *amount)", "完成结算和余额扣费"],
  ["recharge", "int recharge(int userId, double amount, double *balance)", "模拟支付充值并返回余额"],
  ["updateUserInfo", "int updateUserInfo(int userId, char *nickname, char *avatar)", "修改昵称和头像"],
  ["adminLogin", "int adminLogin(char *account, char *pwd)", "管理员身份验证"],
  ["qryRevenueStat", "int qryRevenueStat(int days, REVENUE_STAT *stat)", "统计营收趋势和核心指标"],
  ["qryPileStatus", "int qryPileStatus(PILE_STATUS_STAT *stat)", "统计在用、闲置、故障数量"],
  ["qryPileList", "int qryPileList(PILE_INFO *list, int *count)", "查询全部电桩列表"],
  ["rebootPile", "int rebootPile(int pileId)", "远程重启电桩"],
  ["qryAllStations", "int qryAllStations(STATION_INFO *list, int *count)", "查询全部充电站"],
  ["addStation", "int addStation(STATION_INFO *station)", "新增充电站"],
  ["qryUserList", "int qryUserList(char *keyword, USER_INFO *list, int *count)", "按手机号模糊查询用户"],
  ["setUserStatus", "int setUserStatus(int userId, int status)", "冻结或解冻用户账号"],
  ["qryLoadPred", "int qryLoadPred(int stationId, int hours, LOAD_PRED *pred)", "获取未来负荷和空闲桩预测"],
];

const state = {
  view: "userView",
  phoneScreen: "login",
  loginMode: "quick",
  sort: "distance",
  selectedStationId: 1,
  revenueDays: 7,
  currentUser: users[0],
  currentLocation: { address: "北京海淀区中关村软件园", lng: 116.28196, lat: 40.05202 },
  orderSearch: "",
  orderStatus: "全部",
  activeOrder: null,
  reservation: null,
  history: [
    { id: "O20260903001", userId: 101, phone: "13800138000", stationId: 3, station: "国贸城市补能站", pile: "CBD-E01", duration: 1.4, kwh: 42.1, amount: 55.6, status: "已完成", reviewed: false },
  ],
  logs: [],
};

const titles = {
  userView: "充电用户端 Demo",
  adminView: "运营后台联动 Demo",
  screenView: "数据大屏 Demo",
  apiView: "接口与数据流",
  pmView: "PM 需求拆解",
};

const scenarioFlows = {
  login: [
    ["登录方式", "免密登录 / 密码注册"],
    ["手机号", "输入 11 位手机号"],
    ["验证码", "注册时校验密码和验证码"],
    ["返回 USER_INFO", "昵称、余额、级别"],
    ["进入首页", "主页/订单/我的"],
  ],
  home: [
    ["mockGps", "区域或地址定位"],
    ["qryStationList", "查询附近站点"],
    ["qryLoadPred", "读取 1-2h 预测"],
    ["推荐排序", "展示低拥堵电站"],
  ],
  detail: [
    ["qryStationDetail", "读取电桩状态"],
    ["评价表", "展示 7 日评价"],
    ["预约或充电", "创建订单"],
    ["Socket 同步", "后台状态更新"],
  ],
  orders: [
    ["qryUnfinishedOrder", "检测进行中订单"],
    ["startCharging", "开启订单"],
    ["实时计费", "度数、时长、金额"],
    ["settleOrder", "扣费并完成"],
  ],
  profile: [
    ["USER_INFO", "展示头像昵称余额"],
    ["updateUserInfo", "保存昵称头像"],
    ["recharge", "模拟支付充值"],
    ["退出账号", "清除本地状态"],
  ],
};

const $ = (selector) => document.querySelector(selector);
const $$ = (selector) => Array.from(document.querySelectorAll(selector));
const STORAGE_KEY = "volt-way-demo-state-v2-beijing";

function escapeHtml(value) {
  return String(value ?? "")
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;")
    .replaceAll("'", "&#039;");
}

function replaceArray(target, source) {
  if (!Array.isArray(source)) return;
  target.splice(0, target.length, ...source);
}

function saveState() {
  const currentUserId = state.currentUser?.id ?? users[0]?.id;
  const payload = {
    stations,
    piles,
    reviews,
    users,
    adminRequests,
    state: {
      view: state.view,
      phoneScreen: state.phoneScreen,
      loginMode: state.loginMode,
      sort: state.sort,
      selectedStationId: state.selectedStationId,
      revenueDays: state.revenueDays,
      currentLocation: state.currentLocation,
      orderSearch: state.orderSearch,
      orderStatus: state.orderStatus,
      currentUserId,
      activeOrder: state.activeOrder,
      reservation: state.reservation,
      history: state.history,
      logs: state.logs,
    },
  };
  try {
    localStorage.setItem(STORAGE_KEY, JSON.stringify(payload));
  } catch {
    // localStorage may be unavailable in strict browser settings; the demo still works in memory.
  }
}

function loadState() {
  try {
    const raw = localStorage.getItem(STORAGE_KEY);
    if (!raw) return;
    const payload = JSON.parse(raw);
    replaceArray(stations, payload.stations);
    replaceArray(piles, payload.piles);
    replaceArray(reviews, payload.reviews);
    replaceArray(users, payload.users);
    replaceArray(adminRequests, payload.adminRequests);
    Object.assign(state, payload.state || {});
    state.currentUser = users.find((user) => user.id === payload.state?.currentUserId) || users[0];
    if (!stations.some((station) => station.id === state.selectedStationId)) {
      state.selectedStationId = stations[0]?.id || 1;
    }
  } catch {
    try {
      localStorage.removeItem(STORAGE_KEY);
    } catch {
      // Ignore storage cleanup failures.
    }
  }
}

function resetDemoData() {
  try {
    localStorage.removeItem(STORAGE_KEY);
  } catch {
    // Ignore storage cleanup failures.
  }
  window.location.reload();
}

function formatMoney(value) {
  return `￥${Number(value).toLocaleString("zh-CN", { minimumFractionDigits: 0, maximumFractionDigits: 1 })}`;
}

function freeCount(stationId) {
  return piles.filter((pile) => pile.stationId === stationId && pile.status === "闲置").length;
}

function statusClass(status) {
  if (status === "闲置" || status === "正常" || status === "优秀" || status === "良好") return "status-ok";
  if (status === "使用中" || status === "已预约" || status === "待结算") return "status-running";
  return "status-alert";
}

function aiScore(station) {
  const freeScore = station.predFree2h / station.total;
  const priceScore = 1.45 - station.price;
  const distanceScore = 1 / (station.distance + 0.6);
  return station.rating * 0.28 + freeScore * 2.2 + priceScore * 1.1 + distanceScore;
}

function geocodeAddress(address) {
  const text = String(address || "北京海淀区中关村软件园").trim();
  const known = [
    ["中关村软件园", 116.28196, 40.05202],
    ["中关村", 116.31721, 39.98385],
    ["海淀", 116.29845, 39.95933],
    ["奥林匹克公园", 116.39748, 39.99287],
    ["奥体", 116.39748, 39.99287],
    ["国贸", 116.46135, 39.90918],
    ["望京", 116.47078, 39.99664],
    ["朝阳", 116.44355, 39.92190],
    ["北京", 116.40739, 39.90421],
  ];
  const hit = known.find((item) => text.includes(item[0]));
  if (hit) return { lng: hit[1], lat: hit[2] };

  let hash = 0;
  for (const char of text) hash = (hash * 31 + char.charCodeAt(0)) % 100000;
  return {
    lng: Number((116.25 + (hash % 1800) / 10000).toFixed(6)),
    lat: Number((39.82 + (Math.floor(hash / 1800) % 1400) / 10000).toFixed(6)),
  };
}

function distanceKm(from, to) {
  if (!from || !to) return 0;
  const toRad = (degree) => (degree * Math.PI) / 180;
  const earthRadius = 6371;
  const dLat = toRad(to.lat - from.lat);
  const dLng = toRad(to.lng - from.lng);
  const lat1 = toRad(from.lat);
  const lat2 = toRad(to.lat);
  const a = Math.sin(dLat / 2) ** 2 + Math.sin(dLng / 2) ** 2 * Math.cos(lat1) * Math.cos(lat2);
  return earthRadius * 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
}

function safeAvatarColor(value) {
  return /^#[0-9a-f]{6}$/i.test(String(value || "")) ? value : avatarPalette[0];
}

function levelForSpend(totalSpend) {
  if (totalSpend >= 3000) return "钻石";
  if (totalSpend >= 1000) return "金卡";
  if (totalSpend >= 500) return "银卡";
  return "普通";
}

function discountForLevel(level) {
  return {
    普通: 1,
    银卡: 0.98,
    金卡: 0.95,
    钻石: 0.9,
  }[level] || 1;
}

function discountText(level) {
  const discount = discountForLevel(level);
  return discount === 1 ? "无折扣" : `${Math.round(discount * 100)} 折`;
}

function hydrateDemoState() {
  if (!state.currentLocation?.lng || !state.currentLocation?.lat) {
    state.currentLocation = { address: "北京海淀区中关村软件园", ...geocodeAddress("北京海淀区中关村软件园") };
  }
  stations.forEach((station) => {
    if (!Number.isFinite(station.lng) || !Number.isFinite(station.lat)) {
      Object.assign(station, geocodeAddress(`${station.district}${station.address}${station.name}`));
    }
  });
  users.forEach((user, index) => {
    user.avatarColor = safeAvatarColor(user.avatarColor || avatarPalette[index % avatarPalette.length]);
    user.password = user.password || "123456";
    user.level = levelForSpend(Number(user.totalSpend || 0));
  });
  updateStationDistances();
}

function updateStationDistances() {
  stations.forEach((station) => {
    station.distance = Number(distanceKm(state.currentLocation, station).toFixed(1));
  });
}

function routeEstimate(station) {
  const distance = Math.max(0.4, station.distance || distanceKm(state.currentLocation, station));
  return {
    distance,
    drive: Math.max(4, Math.round(distance * 5 + 3)),
    walk: Math.max(10, Math.round(distance * 13 + 6)),
  };
}

function showToast(text) {
  const toast = $("#toast");
  toast.textContent = text;
  toast.classList.add("show");
  window.setTimeout(() => toast.classList.remove("show"), 2200);
}

function openModal(title, bodyHtml) {
  $("#modalTitle").textContent = title;
  $("#modalBody").innerHTML = bodyHtml;
  $("#modalBackdrop").classList.add("show");
  $("#modalBackdrop").setAttribute("aria-hidden", "false");
  const firstField = $("#modalBody input, #modalBody select, #modalBody textarea, #modalBody button");
  firstField?.focus();
}

function closeModal() {
  $("#modalBackdrop").classList.remove("show");
  $("#modalBackdrop").setAttribute("aria-hidden", "true");
  $("#modalBody").innerHTML = "";
}

function addLog(title, body) {
  const time = new Date().toLocaleTimeString("zh-CN", { hour: "2-digit", minute: "2-digit", second: "2-digit" });
  state.logs.unshift({ title, body, time });
  state.logs = state.logs.slice(0, 8);
  renderLogs();
  saveState();
}

function setView(viewId) {
  state.view = viewId;
  $$(".view-panel").forEach((panel) => panel.classList.toggle("active", panel.id === viewId));
  $$(".nav-item").forEach((item) => item.classList.toggle("active", item.dataset.view === viewId));
  $("#viewTitle").textContent = titles[viewId];
  if (viewId === "adminView") renderAdmin();
  if (viewId === "screenView") renderScreen();
  if (viewId === "apiView") renderApis();
  if (viewId === "pmView") renderPm();
}

function setPhoneScreen(screen) {
  state.phoneScreen = screen;
  $$("[data-phone-screen]").forEach((panel) => panel.classList.toggle("active", panel.dataset.phoneScreen === screen));
  $$(".phone-tabs button").forEach((tab) => tab.classList.toggle("active", tab.dataset.phoneTab === screen));
  const title = {
    login: "登录与自动注册",
    home: "站点查询与 AI 推荐",
    detail: "电站详情与预约",
    orders: "订单状态机",
    profile: "账户资料与钱包",
  }[screen];
  $("#scenarioTitle").textContent = title;
  renderFlow();
}

function setLoginMode(mode) {
  state.loginMode = mode === "register" ? "register" : "quick";
  $$("[data-login-mode]").forEach((button) => button.classList.toggle("active", button.dataset.loginMode === state.loginMode));
  $("#registerFields").hidden = state.loginMode !== "register";
  $("#loginBtn").textContent = state.loginMode === "register" ? "注册并登录" : "登录/自动注册";
}

function renderFlow() {
  const steps = scenarioFlows[state.phoneScreen] || scenarioFlows.home;
  $("#flowStrip").innerHTML = steps
    .map((step, index) => `
      <div class="flow-step">
        <strong>${index + 1}. ${step[0]}</strong>
        <span>${step[1]}</span>
      </div>
    `)
    .join("");
}

function renderLogs() {
  $("#eventLog").innerHTML = state.logs
    .map((log) => `
      <div class="log-item">
        <strong>${log.title}</strong>
        <span>${log.time} · ${log.body}</span>
      </div>
    `)
    .join("");
}

function renderCoordinatePanel() {
  const location = state.currentLocation || { address: "当前位置", lng: 0, lat: 0 };
  $("#locationLabel").textContent = location.address || "当前位置";
  $("#coordinatePanel").innerHTML = `
    <span>lng ${Number(location.lng).toFixed(5)}</span>
    <span>lat ${Number(location.lat).toFixed(5)}</span>
  `;
}

function renderMapPins() {
  const selectedIndex = Math.max(0, stations.findIndex((station) => station.id === state.selectedStationId));
  $("#mapPins").innerHTML = stations
    .map((station, index) => {
      const angle = (index / Math.max(1, stations.length)) * Math.PI * 2 - Math.PI / 4;
      const left = 50 + Math.cos(angle) * 28;
      const top = 50 + Math.sin(angle) * 24;
      const tone = index % 3 === 1 ? "s2" : index % 3 === 2 ? "s3" : "s1";
      const selected = index === selectedIndex ? "selected" : "";
      return `<button class="map-pin station ${tone} ${selected}" data-action="mapSelect" data-station-id="${station.id}" style="left: ${left.toFixed(1)}%; top: ${top.toFixed(1)}%;" title="${escapeHtml(station.name)}" type="button">${index + 1}</button>`;
    })
    .join("");
}

function sortedStations() {
  const list = [...stations];
  if (state.sort === "distance") list.sort((a, b) => a.distance - b.distance);
  if (state.sort === "price") list.sort((a, b) => a.price - b.price);
  if (state.sort === "rating") list.sort((a, b) => b.rating - a.rating);
  if (state.sort === "ai") list.sort((a, b) => aiScore(b) - aiScore(a));
  return list;
}

function renderStationList() {
  $("#stationList").innerHTML = sortedStations()
    .map((station) => `
      <article class="station-card ${station.id === state.selectedStationId ? "active" : ""}">
        <div class="station-head">
          <div>
            <strong>${escapeHtml(station.name)}</strong>
            <span>${escapeHtml(station.district)} · ${escapeHtml(station.address)}</span>
            <span>lng ${Number(station.lng).toFixed(5)} / lat ${Number(station.lat).toFixed(5)}</span>
          </div>
          <span class="score-pill rating-pill">${station.rating.toFixed(1)} 分</span>
        </div>
        <div class="station-metrics">
          <div class="metric-box"><span>距离</span><strong>${station.distance.toFixed(1)} km</strong></div>
          <div class="metric-box"><span>价格</span><strong>${station.price.toFixed(2)} 元/度</strong></div>
          <div class="metric-box"><span>空闲</span><strong>${freeCount(station.id)}/${station.total}</strong></div>
        </div>
        <div class="station-actions">
          <button data-action="detail" data-station-id="${station.id}" type="button">查看详情</button>
          <button data-action="navigate" data-station-id="${station.id}" type="button">去这里</button>
          <button data-action="reserve" data-station-id="${station.id}" type="button">预约</button>
        </div>
      </article>
    `)
    .join("");
}

function renderStationDetail() {
  const station = stations.find((item) => item.id === state.selectedStationId) || stations[0];
  if (!station) return;
  state.selectedStationId = station.id;
  const stationPiles = piles.filter((pile) => pile.stationId === station.id);
  $("#detailName").textContent = station.name;
  $("#detailAddress").textContent = `${station.address} · ${station.distance.toFixed(1)} km`;
  $("#stationDetail").innerHTML = `
    <article class="detail-card">
      <div class="detail-metrics">
        <div class="metric-box"><span>当前空闲</span><strong>${freeCount(station.id)}/${station.total}</strong></div>
        <div class="metric-box"><span>2h 后预测</span><strong>${station.predFree2h} 个</strong></div>
        <div class="metric-box"><span>AI 分</span><strong>${aiScore(station).toFixed(2)}</strong></div>
      </div>
      <p class="muted">坐标 lng ${Number(station.lng).toFixed(5)} / lat ${Number(station.lat).toFixed(5)}</p>
      <div class="station-actions">
        <button data-action="navigate" data-station-id="${station.id}" type="button">去这里</button>
        <button data-action="reserve" data-station-id="${station.id}" type="button">预约空闲桩</button>
      </div>
    </article>
    <article class="detail-card">
      <h3>电桩明细</h3>
      <div class="pile-list">
        ${stationPiles.map((pile) => `
          <div class="pile-row">
            <strong>${escapeHtml(pile.id)}</strong>
            <span>${escapeHtml(pile.type)}<br>${escapeHtml(pile.form)}</span>
            <span>${pile.power} kW</span>
            <span class="status-pill ${statusClass(pile.status)}">${pile.status}</span>
            ${pile.status === "闲置" ? `<button data-action="start" data-pile-id="${pile.id}" type="button">开始</button>` : ""}
          </div>
        `).join("")}
      </div>
    </article>
    <article class="detail-card">
      <h3>7 日内用户评价</h3>
      <div class="review-list">
        ${reviews.filter((review) => review.stationId === station.id).map((review) => `
          <div><strong>${escapeHtml(review.user)} · ${review.score} 分</strong><br>${escapeHtml(review.text)}</div>
        `).join("") || "<div>暂无评价</div>"}
      </div>
    </article>
  `;
}

function renderOrderPanel() {
  const active = state.activeOrder;
  const reservation = state.reservation;
  const firstFreePile = piles.find((pile) => pile.status === "闲置");
  const startHtml = !active
    ? `
      <article class="order-card">
        <h3>扫码 / 手动输入桩号</h3>
        <label>充电桩编号<input id="manualPileInput" value="${escapeHtml(firstFreePile?.id || "")}" placeholder="例如 HN-A01" /></label>
        <div class="form-grid compact-form">
          <label>充电模式
            <select id="chargeModeSelect">
              <option value="充满">充满</option>
              <option value="按时长" selected>按时长</option>
              <option value="按价格">按价格</option>
              <option value="按度数">按度数</option>
            </select>
          </label>
          <label>目标值<input id="chargeTargetInput" type="number" min="1" value="60" /></label>
        </div>
        <div class="order-actions">
          <button data-action="manualStart" type="button">开启充电</button>
        </div>
      </article>
    `
    : "";
  const activeHtml = active
    ? `
      <article class="order-card">
        <div class="station-head">
          <h3>进行中订单</h3>
          <span class="status-pill ${statusClass(active.status || "进行中")}">${escapeHtml(active.status || "进行中")}</span>
        </div>
        <div class="detail-metrics">
          <div class="metric-box"><span>电桩</span><strong>${escapeHtml(active.pile)}</strong></div>
          <div class="metric-box"><span>充电量</span><strong>${Number(active.kwh).toFixed(1)} 度</strong></div>
          <div class="metric-box"><span>金额</span><strong>${formatMoney(active.amount)}</strong></div>
        </div>
        <p class="muted">${escapeHtml(active.station)} · ${escapeHtml(active.mode)} · 已充电 ${active.minutes} 分钟</p>
        <div class="order-actions">
          <button data-action="settle" type="button">结束并结算</button>
          <button data-action="delay" type="button">15 分钟后结束</button>
          <button data-action="adminSettle" type="button">请求代结算</button>
        </div>
      </article>
    `
    : `
      <article class="order-card">
        <h3>暂无进行中订单</h3>
        <p class="muted">点击扫码按钮或在站点详情选择空闲电桩，可模拟开启订单。</p>
      </article>
    `;

  const reserveHtml = reservation
    ? `
      <article class="order-card">
        <h3>预约订单</h3>
        <p>${escapeHtml(reservation.station)} · ${escapeHtml(reservation.pile)}</p>
        <p class="muted">需在 2 小时内到达并扫码确认；模拟剩余 ${Math.max(0, reservation.expiresIn ?? 120)} 分钟。</p>
        <div class="order-actions">
          <button data-action="confirmReserve" type="button">扫码确认</button>
          <button data-action="cancelReserve" type="button">取消预约</button>
        </div>
      </article>
    `
    : "";

  const historyHtml = `
    <article class="order-card">
      <h3>已完成订单</h3>
      <div class="review-list">
        ${state.history.map((order) => `
          <div><strong>${escapeHtml(order.id)} · ${escapeHtml(order.status)}</strong><br>${escapeHtml(order.station)} · ${Number(order.kwh).toFixed(1)} 度 · ${formatMoney(order.amount)}
          <div class="row-actions"><button data-action="review" data-order-id="${escapeHtml(order.id)}" type="button">${order.reviewed ? "已评价" : "评价"}</button></div></div>
        `).join("")}
      </div>
    </article>
  `;

  $("#orderPanel").innerHTML = startHtml + activeHtml + reserveHtml + historyHtml;
}

function renderProfilePanel() {
  $("#profilePanel").innerHTML = `
    <article class="profile-card">
      <div class="station-head">
        <div class="profile-identity">
          <span class="user-avatar ${state.currentUser.avatarImage ? "image" : ""}" style="--avatar-color: ${safeAvatarColor(state.currentUser.avatarColor)};">${state.currentUser.avatarImage ? `<img src="${escapeHtml(state.currentUser.avatarImage)}" alt="用户头像" />` : escapeHtml(state.currentUser.name.slice(0, 1))}</span>
          <div>
            <strong>${escapeHtml(state.currentUser.name)}</strong>
            <span>${escapeHtml(state.currentUser.phone)} · ${escapeHtml(state.currentUser.status)}</span>
          </div>
        </div>
        <span class="score-pill">${state.currentUser.level} · ${discountText(state.currentUser.level)}</span>
      </div>
      <div class="detail-metrics">
        <div class="metric-box"><span>钱包余额</span><strong>${formatMoney(state.currentUser.balance)}</strong></div>
        <div class="metric-box"><span>累计消费</span><strong>${formatMoney(state.currentUser.totalSpend)}</strong></div>
        <div class="metric-box"><span>已充电时长</span><strong>${state.currentUser.hours} h</strong></div>
      </div>
    </article>
    <article class="profile-card">
      <h3>信息修改</h3>
      <label>昵称<input id="nicknameInput" value="${escapeHtml(state.currentUser.name)}" /></label>
      <label>本地头像<input id="avatarInput" type="file" accept="image/*" /></label>
      <div class="profile-actions">
        <button data-action="saveNickname" type="button">保存昵称</button>
        <button data-action="avatar" type="button">切换默认色</button>
      </div>
    </article>
    <article class="profile-card">
      <h3>余额充值</h3>
      <label>充值金额<input id="rechargeInput" type="number" min="1" value="50" /></label>
      <div class="profile-actions">
        <button data-action="recharge" type="button">模拟支付成功</button>
      </div>
    </article>
  `;
}

function startOrder(pileId, mode = "按时长") {
  if (state.activeOrder) {
    showToast("您有未完成的充电订单，请先结算");
    addLog("充电前检查", "qryUnfinishedOrder 返回进行中订单，已跳转订单页");
    setPhoneScreen("orders");
    renderOrderPanel();
    return;
  }
  const pile = piles.find((item) => item.id === pileId);
  if (!pile || pile.status === "故障" || (pile.status !== "闲置" && pile.id !== state.reservation?.pile)) {
    showToast("该电桩暂不可用");
    return;
  }
  pile.status = "使用中";
  const station = stations.find((item) => item.id === pile.stationId);
  state.activeOrder = {
    id: `O${Date.now().toString().slice(-8)}`,
    userId: state.currentUser.id,
    phone: state.currentUser.phone,
    userName: state.currentUser.name,
    stationId: station.id,
    station: station.name,
    pile: pile.id,
    price: pile.price,
    mode,
    status: "进行中",
    minutes: 1,
    kwh: 1.6,
    amount: pile.price * 1.6,
  };
  state.reservation = null;
  addLog("订单创建", `startCharging(${state.currentUser.id}, ${pile.id}) 已生成进行中订单`);
  showToast("充电订单已开始");
  setPhoneScreen("orders");
  renderAll();
}

function settleOrder() {
  if (!state.activeOrder) return;
  const order = state.activeOrder;
  const payMethod = order.payMethod || "余额支付";
  const originalAmount = order.amount;
  const discount = discountForLevel(state.currentUser.level);
  const payable = Number((originalAmount * discount).toFixed(2));
  if (payMethod === "余额支付" && state.currentUser.balance < payable) {
    showToast("余额不足，请先充值或选择微信/支付宝模拟支付");
    return;
  }
  if (payMethod === "余额支付") {
    state.currentUser.balance = Math.max(0, state.currentUser.balance - payable);
  }
  state.currentUser.totalSpend += payable;
  state.currentUser.level = levelForSpend(state.currentUser.totalSpend);
  state.currentUser.hours += Math.max(1, Math.round(order.minutes / 60));
  const pile = piles.find((item) => item.id === order.pile);
  if (pile) {
    pile.status = "闲置";
    pile.count += 1;
    pile.hours += Math.round(order.minutes / 60);
  }
  const station = stations.find((item) => item.id === order.stationId);
  if (station) {
    station.revenue += payable;
    station.orders += 1;
  }
  state.history.unshift({ ...order, originalAmount, amount: payable, discount, payMethod, duration: order.minutes / 60, status: "已完成", reviewed: false });
  state.activeOrder = null;
  addLog("订单结算", `settleOrder(${order.id}) 使用${payMethod}支付 ${formatMoney(payable)}`);
  showToast("订单已结算，账单已进入历史订单");
  renderAll();
}

function openPaymentModal() {
  if (!state.activeOrder) return;
  const order = state.activeOrder;
  const discount = discountForLevel(state.currentUser.level);
  const payable = Number((order.amount * discount).toFixed(2));
  openModal(
    "订单结算",
    `
      <div class="route-summary">
        <p><strong>订单：</strong>${escapeHtml(order.id)} / ${escapeHtml(order.station)} / ${escapeHtml(order.pile)}</p>
        <p><strong>账单：</strong>${Number(order.kwh).toFixed(1)} 度 × ${order.price.toFixed(2)} 元/度，充电 ${order.minutes} 分钟</p>
        <p><strong>会员：</strong>${escapeHtml(state.currentUser.level)}，${discountText(state.currentUser.level)}</p>
      </div>
      <div class="detail-metrics">
        <div class="metric-box"><span>原价</span><strong>${formatMoney(order.amount)}</strong></div>
        <div class="metric-box"><span>优惠后</span><strong>${formatMoney(payable)}</strong></div>
        <div class="metric-box"><span>余额</span><strong>${formatMoney(state.currentUser.balance)}</strong></div>
      </div>
      <div class="route-options">
        <button data-action="payOrder" data-method="余额支付" type="button">余额支付</button>
        <button data-action="payOrder" data-method="微信支付" type="button">微信支付</button>
        <button data-action="payOrder" data-method="支付宝支付" type="button">支付宝支付</button>
      </div>
    `,
  );
}

function reserveStation(stationId) {
  const station = stations.find((item) => item.id === Number(stationId));
  const pile = piles.find((item) => item.stationId === station.id && item.status === "闲置");
  if (!pile) {
    showToast("该站暂无可预约空闲桩");
    return;
  }
  pile.status = "已预约";
  state.reservation = { id: `R${Date.now().toString().slice(-8)}`, userId: state.currentUser.id, phone: state.currentUser.phone, stationId: station.id, station: station.name, pile: pile.id, status: "预约中", expiresIn: 120 };
  addLog("预约订单", `${station.name} 已锁定 ${pile.id}，等待 2 小时内扫码确认`);
  showToast("预约成功");
  setPhoneScreen("orders");
  renderAll();
}

function navigateToStation(stationId) {
  const station = stations.find((item) => item.id === Number(stationId));
  if (!station) return;
  const route = routeEstimate(station);
  addLog("路线规划", `navRoute 已设置 ${state.currentLocation.address} 到 ${station.name} 的驾车/步行路线`);
  openModal(
    "一键导航",
    `
      <div class="route-summary">
        <p><strong>起点：</strong>${escapeHtml(state.currentLocation.address)} · lng ${Number(state.currentLocation.lng).toFixed(5)} / lat ${Number(state.currentLocation.lat).toFixed(5)}</p>
        <p><strong>终点：</strong>${escapeHtml(station.name)} · ${escapeHtml(station.address)}</p>
        <p><strong>坐标：</strong>lng ${Number(station.lng).toFixed(5)} / lat ${Number(station.lat).toFixed(5)}</p>
      </div>
      <div class="detail-metrics">
        <div class="metric-box"><span>路线距离</span><strong>${route.distance.toFixed(1)} km</strong></div>
        <div class="metric-box"><span>驾车</span><strong>${route.drive} min</strong></div>
        <div class="metric-box"><span>步行</span><strong>${route.walk} min</strong></div>
      </div>
      <div class="route-options">
        <button data-action="routeConfirm" data-mode="驾车" data-station-id="${station.id}" type="button">开始驾车导航</button>
        <button data-action="routeConfirm" data-mode="步行" data-station-id="${station.id}" type="button">开始步行导航</button>
      </div>
    `,
  );
  showToast(`已生成到 ${station.name} 的路线方案`);
}

function openAddStationModal() {
  openModal(
    "新增电站",
    `
      <form id="addStationForm">
        <div class="form-grid">
          <label>电站名称<input name="name" value="望京智慧充电站" required /></label>
          <label>所在区域<input name="district" value="朝阳区" required /></label>
          <label class="wide">详细地址<input name="address" value="北京市朝阳区望京街 9 号" required /></label>
          <label>充电价格<input name="price" type="number" min="0.1" step="0.01" value="1.16" required /></label>
          <label>电桩数量<input name="total" type="number" min="1" max="60" step="1" value="10" required /></label>
          <label>距离 km<input name="distance" type="number" min="0.1" step="0.1" value="3.4" required /></label>
          <label>预测空闲<input name="predFree2h" type="number" min="0" step="1" value="6" required /></label>
        </div>
        <div class="form-actions">
          <button data-action="closeModal" type="button">取消</button>
          <button class="primary-btn" type="submit">保存电站</button>
        </div>
      </form>
    `,
  );
}

function addStationFromForm(form) {
  const data = new FormData(form);
  const id = Math.max(0, ...stations.map((station) => station.id)) + 1;
  const total = Math.max(1, Number(data.get("total")) || 1);
  const price = Math.max(0.1, Number(data.get("price")) || 1.1);
  const addressText = String(data.get("address") || "").trim();
  const coordinate = geocodeAddress(`${data.get("district")}${addressText}${data.get("name")}`);
  const station = {
    id,
    name: String(data.get("name") || "").trim(),
    district: String(data.get("district") || "").trim(),
    address: addressText,
    price,
    distance: Math.max(0.1, Number(data.get("distance")) || 1),
    lng: coordinate.lng,
    lat: coordinate.lat,
    rating: 4.6,
    total,
    revenue: 0,
    orders: 0,
    onlineRate: 1,
    predFree1h: Math.min(total, Math.max(0, Number(data.get("predFree2h")) || 1)),
    predFree2h: Math.min(total, Math.max(0, Number(data.get("predFree2h")) || 1)),
    heat: 55,
    health: 96,
  };
  if (!station.name || !station.district || !station.address) {
    showToast("请填写完整电站信息");
    return;
  }
  stations.push(station);
  updateStationDistances();
  const pileCount = total;
  for (let index = 1; index <= pileCount; index += 1) {
    piles.push({
      id: `NEW-${id}${String(index).padStart(2, "0")}`,
      stationId: id,
      type: index === 1 ? "直流超快充" : index % 2 ? "直流快充" : "交流慢充",
      form: index === 1 ? "一桩一充 / 立式" : "一桩双充 / 立式",
      power: index === 1 ? 180 : index % 2 ? 120 : 11,
      status: "闲置",
      count: 0,
      hours: 0,
      price,
    });
  }
  state.selectedStationId = id;
  addLog("新增电站", `addStation 已保存 ${station.name}，并生成 ${pileCount} 条模拟电桩`);
  showToast("电站已新增");
  closeModal();
  renderAll();
}

function openReviewModal(orderId) {
  const order = state.history.find((item) => item.id === orderId);
  if (!order) return;
  openModal(
    "提交评价",
    `
      <form id="reviewForm" data-order-id="${escapeHtml(order.id)}">
        <div class="form-grid">
          <label>评分
            <select name="score">
              <option value="5">5 分</option>
              <option value="4">4 分</option>
              <option value="3">3 分</option>
              <option value="2">2 分</option>
              <option value="1">1 分</option>
            </select>
          </label>
          <label>评价电站<input value="${escapeHtml(order.station)}" disabled /></label>
          <label class="wide">评价内容<textarea name="text" required>充电过程顺利，站点信息和空闲桩显示比较准确。</textarea></label>
        </div>
        <div class="form-actions">
          <button data-action="closeModal" type="button">取消</button>
          <button class="primary-btn" type="submit">提交评价</button>
        </div>
      </form>
    `,
  );
}

function submitReview(form) {
  const orderId = form.dataset.orderId;
  const order = state.history.find((item) => item.id === orderId);
  if (!order) return;
  const data = new FormData(form);
  const text = String(data.get("text") || "").trim();
  if (!text) {
    showToast("请填写评价内容");
    return;
  }
  reviews.unshift({
    stationId: order.stationId,
    user: state.currentUser?.name || "当前用户",
    score: Number(data.get("score")) || 5,
    text,
  });
  order.reviewed = true;
  addLog("评价提交", `评价表新增 ${order.station} 的 ${data.get("score")} 分评价`);
  showToast("评价已提交，电站详情页已更新");
  closeModal();
  renderAll();
}

function renderAdminKpis() {
  const totalRevenue = stations.reduce((sum, station) => sum + station.revenue, 0);
  const totalOrders = stations.reduce((sum, station) => sum + station.orders, 0);
  const faultCount = piles.filter((pile) => pile.status === "故障").length;
  const activeCount = piles.filter((pile) => pile.status === "使用中").length;
  $("#adminKpis").innerHTML = [
    ["今日营收", formatMoney(14600), "+8.1%"],
    ["本月营收", formatMoney(totalRevenue), "+12.4%"],
    ["今日订单", `${Math.round(totalOrders / 18)} 单`, "+5.7%"],
    ["故障电桩", `${faultCount} 个`, activeCount ? `${activeCount} 个在用` : "无在用"],
  ]
    .map((item, index) => `
      <div class="kpi-card">
        <span>${item[0]}</span>
        <strong>${item[1]}</strong>
        <small class="${index === 3 && faultCount ? "negative" : "positive"}">${item[2]}</small>
      </div>
    `)
    .join("");
}

function renderRevenueChart() {
  const values = revenueSeries[state.revenueDays];
  const max = Math.max(...values);
  $("#revenueChart").innerHTML = values
    .map((value, index) => `
      <div class="line-bar" title="${value}" style="height: ${Math.max(18, (value / max) * 220)}px">
        <span>${state.revenueDays === 7 ? index + 1 : `${index * 3 + 1}`}</span>
      </div>
    `)
    .join("");
}

function renderAdminStations() {
  const keyword = ($("#stationSearch")?.value || "").trim();
  const filtered = stations
    .filter((station) => {
      if (!keyword) return true;
      const hasFault = piles.some((pile) => pile.stationId === station.id && pile.status === "故障");
      return station.name.includes(keyword) || station.district.includes(keyword) || (keyword.includes("故障") && hasFault);
    })
    .sort((a, b) => b.revenue - a.revenue);
  $("#adminStations").innerHTML = filtered
    .map((station) => `
      <article class="admin-card">
        <div class="station-head">
          <strong>${escapeHtml(station.name)}</strong>
          <span class="status-pill ${station.onlineRate > 0.9 ? "status-ok" : "status-running"}">${Math.round(station.onlineRate * 100)}%</span>
        </div>
        <p>${escapeHtml(station.address)}</p>
        <div class="station-metrics">
          <div class="metric-box"><span>总桩数</span><strong>${station.total}</strong></div>
          <div class="metric-box"><span>使用中</span><strong>${piles.filter((pile) => pile.stationId === station.id && pile.status === "使用中").length}</strong></div>
          <div class="metric-box"><span>本月营收</span><strong>${formatMoney(station.revenue)}</strong></div>
        </div>
        <div class="row-actions">
          <button data-action="adminDetail" data-station-id="${station.id}" type="button">查看明细</button>
        </div>
      </article>
    `)
    .join("");
}

function renderPileTable() {
  $("#pileTable").innerHTML = piles
    .map((pile) => {
      const station = stations.find((item) => item.id === pile.stationId);
      return `
        <tr>
          <td>${escapeHtml(pile.id)}</td>
          <td>${escapeHtml(station.name)}</td>
          <td>${escapeHtml(pile.type)}</td>
          <td>${pile.power} kW</td>
          <td><span class="status-pill ${statusClass(pile.status)}">${pile.status}</span></td>
          <td>${pile.count} 次 / ${pile.hours} h</td>
          <td class="row-actions">
            <button data-action="reboot" data-pile-id="${pile.id}" type="button">重启</button>
            <button data-action="powerOff" data-pile-id="${pile.id}" type="button">断电</button>
          </td>
        </tr>
      `;
    })
    .join("");
}

function renderUserTable() {
  const keyword = ($("#userSearch")?.value || "").trim();
  const rows = users.filter((user) => !keyword || user.phone.includes(keyword));
  $("#userTable").innerHTML = rows
    .map((user) => `
      <tr>
        <td>${escapeHtml(user.name)}</td>
        <td>${escapeHtml(user.phone)}</td>
        <td>${formatMoney(user.balance)}</td>
        <td>${user.level}</td>
        <td><span class="status-pill ${statusClass(user.status)}">${user.status}</span></td>
        <td class="row-actions">
          <button data-action="toggleUser" data-user-id="${user.id}" type="button">${user.status === "正常" ? "冻结" : "解冻"}</button>
        </td>
      </tr>
    `)
    .join("");
}

function buildAdminOrders() {
  const rows = [];
  if (state.activeOrder) rows.push({ ...state.activeOrder, status: state.activeOrder.status || "进行中" });
  if (state.reservation) rows.push({ ...state.reservation, id: state.reservation.id || "预约订单", amount: 0, status: "预约中" });
  rows.push(...state.history);
  return rows.map((order) => {
    const user = users.find((item) => item.id === order.userId || item.phone === order.phone) || state.currentUser;
    return {
      ...order,
      userName: order.userName || user?.name || "未知用户",
      phone: order.phone || user?.phone || "",
      amount: Number(order.amount || 0),
    };
  });
}

function renderAdminOrders() {
  const keyword = ($("#orderSearch")?.value || state.orderSearch || "").trim();
  const status = $("#orderStatusFilter")?.value || state.orderStatus || "全部";
  state.orderSearch = keyword;
  state.orderStatus = status;
  if ($("#orderSearch") && $("#orderSearch").value !== keyword) $("#orderSearch").value = keyword;
  if ($("#orderStatusFilter") && $("#orderStatusFilter").value !== status) $("#orderStatusFilter").value = status;
  const rows = buildAdminOrders().filter((order) => {
    const text = `${order.id} ${order.userName} ${order.phone} ${order.station} ${order.pile}`;
    const keywordMatch = !keyword || text.includes(keyword);
    const statusMatch = status === "全部" || order.status === status;
    return keywordMatch && statusMatch;
  });

  $("#adminOrderTable").innerHTML = rows.length
    ? rows
        .map((order) => `
          <tr>
            <td>${escapeHtml(order.id)}</td>
            <td>${escapeHtml(order.userName)}<br><span class="muted">${escapeHtml(order.phone)}</span></td>
            <td>${escapeHtml(order.station)}<br><span class="muted">${escapeHtml(order.pile)}</span></td>
            <td>${order.amount ? formatMoney(order.amount) : "-"}</td>
            <td><span class="status-pill ${statusClass(order.status)}">${escapeHtml(order.status)}</span></td>
            <td class="row-actions">
              ${order.status === "进行中" ? `<button data-action="adminMarkPending" data-order-id="${escapeHtml(order.id)}" type="button">转待结算</button>` : ""}
              ${order.status === "待结算" ? `<button data-action="adminSettleNow" data-order-id="${escapeHtml(order.id)}" type="button">代结算</button>` : ""}
              ${order.status === "预约中" ? `<button data-action="expireReserve" data-order-id="${escapeHtml(order.id)}" type="button">置为超时</button>` : ""}
              ${order.status === "已完成" ? `<button data-action="viewOrder" data-order-id="${escapeHtml(order.id)}" type="button">查看</button>` : ""}
            </td>
          </tr>
        `)
        .join("")
    : `<tr><td colspan="6"><div class="empty-state">没有匹配的订单。</div></td></tr>`;
  saveState();
}

function renderAdminRequests() {
  const rows = adminRequests.filter((request) => request.status === "待审批");
  $("#adminRequestList").innerHTML = rows.length
    ? rows
        .map((request) => `
          <div class="warning-card">
            <strong>${escapeHtml(request.account)} · ${escapeHtml(request.role)}</strong>
            <span>${escapeHtml(request.applyAt)} 提交注册申请</span>
            <div class="row-actions">
              <button data-action="approveAdmin" data-request-id="${request.id}" type="button">通过</button>
              <button data-action="rejectAdmin" data-request-id="${request.id}" type="button">拒绝</button>
            </div>
          </div>
        `)
        .join("")
    : `<div class="empty-state">暂无待审批管理员。</div>`;
}

function openOrderDetailModal(orderId) {
  const order = buildAdminOrders().find((item) => item.id === orderId);
  if (!order) return;
  openModal(
    "订单详情",
    `
      <div class="detail-card">
        <div class="detail-metrics">
          <div class="metric-box"><span>订单号</span><strong>${escapeHtml(order.id)}</strong></div>
          <div class="metric-box"><span>状态</span><strong>${escapeHtml(order.status)}</strong></div>
          <div class="metric-box"><span>金额</span><strong>${order.amount ? formatMoney(order.amount) : "-"}</strong></div>
        </div>
        <p><strong>用户：</strong>${escapeHtml(order.userName)} / ${escapeHtml(order.phone)}</p>
        <p><strong>电站：</strong>${escapeHtml(order.station)}</p>
        <p><strong>电桩：</strong>${escapeHtml(order.pile)}</p>
        <p><strong>充电量：</strong>${Number(order.kwh || 0).toFixed(1)} 度</p>
      </div>
      <div class="form-actions">
        <button class="primary-btn" data-action="closeModal" type="button">知道了</button>
      </div>
    `,
  );
}

function renderWarnings() {
  const warnings = [
    ...piles.filter((pile) => pile.status === "故障").map((pile) => ({ title: `${pile.id} 需检修`, body: "设备故障预测命中，建议现场排查" })),
    ...stations.filter((station) => station.heat > 85).map((station) => ({ title: `${station.name} 高峰预警`, body: `未来 2h 预测空闲 ${station.predFree2h} 个，建议调配值守` })),
  ];
  $("#alertCount").textContent = `${warnings.length} 条`;
  $("#warningList").innerHTML = warnings
    .map((warning) => `<div class="warning-card"><strong>${warning.title}</strong><span>${warning.body}</span></div>`)
    .join("");
}

function renderAdmin() {
  renderAdminKpis();
  renderRevenueChart();
  renderAdminStations();
  renderPileTable();
  renderUserTable();
  renderAdminOrders();
  renderAdminRequests();
  renderWarnings();
}

function renderScreen() {
  const totalOrders = stations.reduce((sum, station) => sum + station.orders, 0);
  const totalRevenue = stations.reduce((sum, station) => sum + station.revenue, 0);
  const activeStations = stations.filter((station) => station.orders > 20).length;
  const faultRate = piles.filter((pile) => pile.status === "故障").length / piles.length;
  $("#screenKpis").innerHTML = [
    ["本月订单数", `${totalOrders} 单`, "+9.8%"],
    ["充电总度数", "52,480 度", "+7.2%"],
    ["充电总金额", formatMoney(totalRevenue), "+12.4%"],
    ["电池健康率", `${Math.round((1 - faultRate) * 100)}%`, "-1.6%"],
  ]
    .map((item, index) => `
      <div class="kpi-card">
        <span>${item[0]}</span>
        <strong>${item[1]}</strong>
        <small class="${index === 3 ? "negative" : "positive"}">${item[2]}</small>
      </div>
    `)
    .join("");

  const heatValues = [22, 18, 16, 14, 20, 35, 58, 72, 68, 55, 48, 44, 39, 42, 51, 63, 76, 91, 88, 74, 62, 50, 38, 29];
  $("#heatGrid").innerHTML = heatValues
    .map((value, index) => {
      const level = Math.min(1, value / 100);
      const color = `rgb(${Math.round(88 + 153 * level)}, ${Math.round(166 - 52 * level)}, ${Math.round(223 - 75 * level)})`;
      return `<div class="heat-cell" style="background:${color}"><span>${index}:00</span><strong>${value}</strong></div>`;
    })
    .join("");

  const maxRevenue = Math.max(...stations.map((station) => station.revenue));
  $("#topStations").innerHTML = [...stations]
    .sort((a, b) => b.revenue - a.revenue)
    .map((station) => `
      <div class="bar-row">
        <span>${escapeHtml(station.name.replace("城市", ""))}</span>
        <div class="bar-track"><div class="bar-fill" style="width:${(station.revenue / maxRevenue) * 100}%"></div></div>
        <strong>${Math.round(station.revenue / 1000)}k</strong>
      </div>
    `)
    .join("");

  $("#levelLegend").innerHTML = [
    ["普通", 42, "var(--blue)"],
    ["银卡", 26, "var(--teal)"],
    ["金卡", 18, "var(--green)"],
    ["钻石", 14, "var(--red)"],
  ]
    .map((item) => `<div class="legend-item"><span><i class="legend-dot" style="background:${item[2]}"></i>${item[0]}</span><strong>${item[1]}%</strong></div>`)
    .join("");

  $("#predictionList").innerHTML = stations
    .map((station) => `
      <article class="prediction-card">
        <strong>${escapeHtml(station.name)}</strong>
        <span>1h 后空闲 ${station.predFree1h} 个，24h 负荷指数 ${station.heat}，高峰预警 ${station.heat > 85 ? "是" : "否"}</span>
      </article>
    `)
    .join("");

  $("#healthBars").innerHTML = [
    ["优秀", 46, "var(--green)"],
    ["良好", 34, "var(--blue)"],
    ["一般", 15, "var(--amber)"],
    ["差", 5, "var(--red)"],
  ]
    .map((item) => `
      <div class="health-row">
        <span>${item[0]}</span>
        <div class="bar-track"><div class="bar-fill" style="width:${item[1]}%; background:${item[2]}"></div></div>
        <strong>${item[1]}%</strong>
      </div>
    `)
    .join("");
}

function renderApis() {
  $("#apiList").innerHTML = apiList
    .map((api) => `
      <article class="api-card">
        <strong>${api[0]}</strong>
        <code>${api[1]}</code>
        <span>${api[2]}</span>
      </article>
    `)
    .join("");
}

function renderPm() {
  const doing = pmRequirements.filter((item) => item[2] === "△").length;
  const todo = pmRequirements.filter((item) => item[2] === "×").length;
  $("#pmSummary").innerHTML = [
    ["PM 需求项", pmRequirements.length, "矩阵第 1-26 条"],
    ["矩阵进行中", doing, "来自需求矩阵"],
    ["矩阵未着手", todo, "来自需求矩阵"],
    ["Demo 覆盖", `${pmRequirements.length}/${pmRequirements.length}`, "用户端原型已覆盖"],
  ]
    .map((item) => `
      <div class="kpi-card">
        <span>${item[0]}</span>
        <strong>${item[1]}</strong>
        <small>${item[2]}</small>
      </div>
    `)
    .join("");

  $("#requirementList").innerHTML = pmRequirements
    .map((item, index) => `
      <article class="requirement-card">
        <span class="no">${index + 1}</span>
        <div>
          <strong>${item[0]}</strong>
          <p>${item[1]}</p>
        </div>
        <div class="status-stack">
          <span class="status-pill ${item[2] === "△" ? "status-running" : item[2] === "×" ? "status-alert" : "status-ok"}">矩阵 ${item[2]}</span>
          <span class="status-pill status-ok">Demo ○</span>
        </div>
      </article>
    `)
    .join("");
}

function simulateTick() {
  if (state.reservation) {
    state.reservation.expiresIn = Math.max(0, (state.reservation.expiresIn ?? 120) - 5);
    if (state.reservation.expiresIn === 0) {
      const pile = piles.find((item) => item.id === state.reservation.pile);
      if (pile) pile.status = "闲置";
      state.history.unshift({ ...state.reservation, amount: 0, kwh: 0, status: "已超时", reviewed: true });
      addLog("预约自动超时", `${state.reservation.pile} 超过 2h 未确认，系统已释放电桩`);
      state.reservation = null;
    }
  }
  if (state.activeOrder) {
    state.activeOrder.minutes += 1;
    state.activeOrder.kwh += 0.8;
    state.activeOrder.amount = state.activeOrder.kwh * state.activeOrder.price;
  }
  saveState();
  if (state.phoneScreen === "orders") renderOrderPanel();
}

function renderAll() {
  renderCoordinatePanel();
  renderMapPins();
  renderStationList();
  renderStationDetail();
  renderOrderPanel();
  renderProfilePanel();
  renderFlow();
  renderLogs();
  if (state.view === "adminView") renderAdmin();
  if (state.view === "screenView") renderScreen();
  if (state.view === "apiView") renderApis();
  if (state.view === "pmView") renderPm();
  $("#sidebarUpdated").textContent = new Date().toLocaleTimeString("zh-CN", { hour: "2-digit", minute: "2-digit" });
}

function bindEvents() {
  $$(".nav-item").forEach((button) => button.addEventListener("click", () => setView(button.dataset.view)));
  $$("[data-login-mode]").forEach((button) => button.addEventListener("click", () => setLoginMode(button.dataset.loginMode)));
  $("#openPmBtn").addEventListener("click", () => setView("pmView"));
  $("#resetBtn").addEventListener("click", resetDemoData);
  $("#syncBtn").addEventListener("click", () => {
    stations.forEach((station) => {
      station.predFree1h = Math.max(1, station.predFree1h + Math.round(Math.random() * 4 - 2));
      station.predFree2h = Math.max(1, station.predFree2h + Math.round(Math.random() * 4 - 2));
      station.heat = Math.min(96, Math.max(45, station.heat + Math.round(Math.random() * 10 - 5)));
    });
    addLog("数据刷新", "fetchData 拉取营收、电桩状态、订单和 AI 预测结果");
    showToast("模拟数据已刷新");
    renderAll();
  });

  $("#loginBtn").addEventListener("click", () => {
    const phone = $("#phoneInput").value.trim();
    if (!/^1\d{10}$/.test(phone)) {
      showToast("请输入 11 位手机号");
      return;
    }
    const isRegister = state.loginMode === "register";
    const password = $("#passwordInput")?.value.trim() || "";
    const code = $("#codeInput")?.value.trim() || "";
    if (isRegister && password.length < 6) {
      showToast("密码至少 6 位");
      return;
    }
    if (isRegister && !/^\d{4,6}$/.test(code)) {
      showToast("请输入 4-6 位验证码");
      return;
    }
    const existing = users.find((user) => user.phone === phone);
    if (existing) {
      if (isRegister) existing.password = password;
      state.currentUser = existing;
      addLog(isRegister ? "注册登录" : "用户登录", `verifyUserLogin 返回已有用户 ${existing.name}`);
    } else {
      const created = { id: Date.now(), phone, password: password || "123456", name: `用户${phone.slice(-4)}`, avatarColor: avatarPalette[users.length % avatarPalette.length], balance: 0, level: "普通", status: "正常", registerAt: "2026-09-04", totalSpend: 0, hours: 0 };
      users.push(created);
      state.currentUser = created;
      addLog(isRegister ? "密码注册" : "自动注册", `手机号 ${phone} 不存在，已创建默认用户 ${created.name}`);
    }
    state.currentUser.level = levelForSpend(Number(state.currentUser.totalSpend || 0));
    setPhoneScreen("home");
    renderAll();
  });

  $("#locateBtn").addEventListener("click", () => {
    const location = $("#locationInput").value.trim() || "当前位置";
    const coordinate = geocodeAddress(location);
    state.currentLocation = { address: location, ...coordinate };
    updateStationDistances();
    addLog("定位完成", `addrToLngLat(${location}) -> lng ${coordinate.lng.toFixed(5)}, lat ${coordinate.lat.toFixed(5)}`);
    showToast("位置已更新，站点列表已按距离刷新");
    state.sort = "distance";
    renderAll();
  });

  $("#aiRecommendBtn").addEventListener("click", () => {
    state.sort = "ai";
    const best = sortedStations()[0];
    state.selectedStationId = best.id;
    addLog("AI 推荐", `qryLoadPred 结合评分、价格、距离，推荐 ${best.name}`);
    showToast(`AI 推荐：${best.name}`);
    updateSortButtons();
    renderAll();
  });

  $("#backHomeBtn").addEventListener("click", () => setPhoneScreen("home"));
  $("#scanPileBtn").addEventListener("click", () => {
    const pile = piles.find((item) => item.status === "闲置");
    startOrder(pile?.id);
  });
  $("#logoutBtn").addEventListener("click", () => {
    addLog("退出账号", "本地登录状态已清除");
    setPhoneScreen("login");
  });

  $("#stationSearch").addEventListener("input", renderAdminStations);
  $("#userSearch").addEventListener("input", renderUserTable);
  $("#orderSearch").addEventListener("input", renderAdminOrders);
  $("#orderStatusFilter").addEventListener("change", renderAdminOrders);
  $("#closeModalBtn").addEventListener("click", closeModal);
  $("#modalBackdrop").addEventListener("click", (event) => {
    if (event.target.id === "modalBackdrop") closeModal();
  });
  document.addEventListener("keydown", (event) => {
    if (event.key === "Escape") closeModal();
  });

  document.addEventListener("submit", (event) => {
    if (event.target.id === "addStationForm") {
      event.preventDefault();
      addStationFromForm(event.target);
    }
    if (event.target.id === "reviewForm") {
      event.preventDefault();
      submitReview(event.target);
    }
  });

  document.addEventListener("change", (event) => {
    if (event.target.id !== "avatarInput") return;
    const file = event.target.files?.[0];
    if (!file) return;
    const reader = new FileReader();
    reader.addEventListener("load", () => {
      state.currentUser.avatarImage = String(reader.result || "");
      addLog("头像上传", `updateUserInfo 已读取本地图片 ${file.name}`);
      showToast("头像图片已更新");
      renderProfilePanel();
    });
    reader.readAsDataURL(file);
  });

  document.addEventListener("click", (event) => {
    const button = event.target.closest("button");
    if (!button) return;

    if (button.dataset.phoneTab) {
      setPhoneScreen(button.dataset.phoneTab);
      return;
    }

    if (button.dataset.sort) {
      state.sort = button.dataset.sort;
      updateSortButtons();
      addLog("排序切换", `站点列表按 ${button.textContent.trim()} 策略展示`);
      renderStationList();
      return;
    }

    if (button.dataset.revenueDays) {
      state.revenueDays = Number(button.dataset.revenueDays);
      $("[data-revenue-days='7']").classList.toggle("active", state.revenueDays === 7);
      $("[data-revenue-days='30']").classList.toggle("active", state.revenueDays === 30);
      renderRevenueChart();
      return;
    }

    const action = button.dataset.action;
    if (!action) return;
    if (action === "closeModal") closeModal();
    if (action === "openAddStation") openAddStationModal();
    if (action === "mapSelect") {
      const station = stations.find((item) => item.id === Number(button.dataset.stationId));
      if (station) {
        state.selectedStationId = station.id;
        addLog("地图选站", `地图标记已选中 ${station.name}`);
        setPhoneScreen("detail");
        renderAll();
      }
    }
    if (action === "detail") {
      state.selectedStationId = Number(button.dataset.stationId);
      renderStationDetail();
      setPhoneScreen("detail");
    }
    if (action === "navigate") navigateToStation(button.dataset.stationId);
    if (action === "reserve") reserveStation(button.dataset.stationId);
    if (action === "start") startOrder(button.dataset.pileId);
    if (action === "manualStart") {
      const pileId = $("#manualPileInput")?.value.trim();
      const mode = $("#chargeModeSelect")?.value || "按时长";
      const target = Number($("#chargeTargetInput")?.value || 0);
      const modeText = mode === "充满" ? "充满" : `${mode}${target || ""}`;
      startOrder(pileId, modeText);
    }
    if (action === "settle") openPaymentModal();
    if (action === "delay") {
      addLog("延时结束", "用户设置 15 分钟后自动结束订单");
      showToast("已设置延时结束");
    }
    if (action === "adminSettle") {
      if (state.activeOrder) state.activeOrder.status = "待结算";
      addLog("代结算请求", "用户已请求管理员代为结算，PRL 后台订单状态已更新");
      showToast("已提交管理员代结算请求");
      renderAll();
    }
    if (action === "confirmReserve" && state.reservation) {
      const reservedPile = piles.find((pile) => pile.id === state.reservation.pile);
      if (reservedPile) reservedPile.status = "闲置";
      startOrder(state.reservation.pile, "预约确认");
    }
    if (action === "cancelReserve" && state.reservation) {
      const reservedPile = piles.find((pile) => pile.id === state.reservation.pile);
      if (reservedPile) reservedPile.status = "闲置";
      state.reservation = null;
      addLog("取消预约", "预约订单已取消，电桩释放为空闲");
      renderAll();
    }
    if (action === "review") {
      const order = state.history.find((item) => item.id === button.dataset.orderId);
      if (order?.reviewed) {
        showToast("该订单已评价");
      } else {
        openReviewModal(button.dataset.orderId);
      }
    }
    if (action === "saveNickname") {
      const nickname = $("#nicknameInput").value.trim() || state.currentUser.name;
      state.currentUser.name = nickname;
      addLog("信息修改", `updateUserInfo 已保存昵称 ${nickname}`);
      showToast("昵称已保存");
      renderProfilePanel();
    }
    if (action === "avatar") {
      const currentIndex = avatarPalette.indexOf(safeAvatarColor(state.currentUser.avatarColor));
      state.currentUser.avatarColor = avatarPalette[(currentIndex + 1) % avatarPalette.length];
      state.currentUser.avatarImage = "";
      addLog("头像维护", `updateUserInfo 已更新 ${state.currentUser.name} 的头像色`);
      showToast("头像已更新");
      renderProfilePanel();
    }
    if (action === "routeConfirm") {
      const station = stations.find((item) => item.id === Number(button.dataset.stationId));
      addLog("导航启动", `${button.dataset.mode}路线已启动：${state.currentLocation.address} -> ${station?.name || "目标电站"}`);
      showToast(`${button.dataset.mode}导航已启动`);
      closeModal();
    }
    if (action === "payOrder") {
      if (state.activeOrder) state.activeOrder.payMethod = button.dataset.method || "余额支付";
      closeModal();
      settleOrder();
    }
    if (action === "recharge") {
      const amount = Number($("#rechargeInput").value || 0);
      if (amount <= 0) {
        showToast("请输入有效充值金额");
        return;
      }
      state.currentUser.balance += amount;
      addLog("余额充值", `recharge 返回新余额 ${formatMoney(state.currentUser.balance)}`);
      showToast("充值成功");
      renderProfilePanel();
      renderUserTable();
    }
    if (action === "reboot") {
      addLog("远程重启", `rebootPile(${button.dataset.pileId}) 已发送模拟指令`);
      showToast(`${button.dataset.pileId} 重启指令已发送`);
    }
    if (action === "powerOff") {
      const pile = piles.find((item) => item.id === button.dataset.pileId);
      if (pile) pile.status = "故障";
      addLog("紧急断电", `${button.dataset.pileId} 状态已置为故障/断电`);
      showToast("紧急断电已模拟");
      renderAll();
    }
    if (action === "toggleUser") {
      const user = users.find((item) => item.id === Number(button.dataset.userId));
      user.status = user.status === "正常" ? "冻结" : "正常";
      addLog("用户状态", `setUserStatus(${user.id}) 已更新为 ${user.status}`);
      renderUserTable();
      renderAdminOrders();
    }
    if (action === "adminDetail") {
      const station = stations.find((item) => item.id === Number(button.dataset.stationId));
      addLog("后台查看明细", `qryStationDetail 已读取 ${station.name} 电桩实时状态`);
      showToast("站内电桩明细已同步到电桩管理表");
    }
    if (action === "adminMarkPending") {
      if (state.activeOrder?.id === button.dataset.orderId) {
        state.activeOrder.status = "待结算";
        addLog("后台状态流转", `${state.activeOrder.id} 已由进行中转为待结算`);
        showToast("订单已转为待结算");
        renderAll();
      } else {
        showToast("该订单当前不支持状态流转");
      }
    }
    if (action === "adminSettleNow") {
      if (state.activeOrder?.id === button.dataset.orderId) {
        addLog("管理员代结算", `PRL 已对 ${state.activeOrder.id} 执行待结算处理`);
        state.activeOrder.payMethod = "后台代结算";
        settleOrder();
      } else {
        showToast("该订单当前不支持代结算");
      }
    }
    if (action === "expireReserve" && state.reservation?.id === button.dataset.orderId) {
      const pile = piles.find((item) => item.id === state.reservation.pile);
      if (pile) pile.status = "闲置";
      state.reservation.status = "已超时";
      state.history.unshift({ ...state.reservation, amount: 0, kwh: 0, status: "已超时", reviewed: true });
      addLog("预约超时", `${state.reservation.pile} 预约超时，电桩已释放`);
      state.reservation = null;
      showToast("预约已置为超时");
      renderAll();
    }
    if (action === "approveAdmin" || action === "rejectAdmin") {
      const request = adminRequests.find((item) => item.id === Number(button.dataset.requestId));
      if (request) {
        request.status = action === "approveAdmin" ? "已通过" : "已拒绝";
        addLog("管理员审批", `${request.account} ${request.status}`);
        showToast(`申请${request.status}`);
        renderAdminRequests();
      }
    }
    if (action === "mockAdminRequest") {
      const id = Math.max(0, ...adminRequests.map((item) => item.id)) + 1;
      const request = {
        id,
        account: `new_admin_${id}`,
        role: id % 2 ? "电站管理员" : "运维值守",
        applyAt: new Date().toLocaleString("zh-CN", { hour12: false }),
        status: "待审批",
      };
      adminRequests.unshift(request);
      addLog("管理员注册", `${request.account} 已提交注册申请`);
      showToast("已生成一条待审批申请");
      renderAdminRequests();
    }
    if (action === "viewOrder") openOrderDetailModal(button.dataset.orderId);
  });
}

function updateSortButtons() {
  $$("[data-sort]").forEach((button) => button.classList.toggle("active", button.dataset.sort === state.sort));
}

bindEvents();
loadState();
hydrateDemoState();
addLog("Demo 初始化", "已载入 PM 用户端、PRL 后台、SCML 大屏和 PE1/PE2 接口映射");
setLoginMode(state.loginMode || "quick");
setPhoneScreen(state.phoneScreen || "login");
setView(state.view || "userView");
renderAll();
window.setInterval(simulateTick, 3000);
