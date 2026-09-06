#!/bin/bash
# ============================================================
# SCML 大屏 · Linux 一键启动脚本
# 用法：chmod +x start.sh && ./start.sh
# 停止：按 Ctrl+C
# ============================================================
cd "$(dirname "$0")"

# 检查 python3 是否可用
if ! command -v python3 &> /dev/null; then
    echo "[错误] 未找到 python3，请先安装 Python 3"
    exit 1
fi

echo "========================================="
echo "  SCML 大屏启动中..."
echo "========================================="

# 启动 Mock 后端（端口 8766）
echo "[1/2] 启动 Mock 后端 (端口 8766)..."
python3 mock_server.py > mock_server.log 2>&1 &
MOCK_PID=$!
sleep 1

# 启动静态服务器（端口 8765）
echo "[2/2] 启动静态服务器 (端口 8765)..."
python3 -m http.server 8765 > http_server.log 2>&1 &
STATIC_PID=$!
sleep 1

echo ""
echo "========================================="
echo "  大屏已启动！"
echo "  浏览器打开: http://127.0.0.1:8765/index.html"
echo ""
echo "  Mock 后端 PID: $MOCK_PID"
echo "  静态服务 PID: $STATIC_PID"
echo ""
echo "  按 Ctrl+C 停止所有服务"
echo "========================================="

# 捕获 Ctrl+C，清理两个后台进程
cleanup() {
    echo ""
    echo "正在停止服务..."
    kill $MOCK_PID 2>/dev/null
    kill $STATIC_PID 2>/dev/null
    echo "已停止。"
    exit 0
}
trap cleanup INT TERM

# 保持脚本运行，等待 Ctrl+C
wait
