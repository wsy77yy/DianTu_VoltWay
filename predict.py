import pandas as pd
import numpy as np
import torch
import joblib

from model import LSTMModel

# =========================================================
# 配置
# =========================================================

DATA_PATH = "data/processed/processed_data.csv"

MODEL_PATH = "checkpoints/best_model.pth"
X_SCALER_PATH = "checkpoints/x_scaler.pkl"
Y_SCALER_PATH = "checkpoints/y_scaler.pkl"

# 要预测的充电站
STATION_ID = 1001
# 使用过去24小时预测未来24小时
HISTORY_LENGTH = 24
PREDICTION_LENGTH = 24
# 模型输入特征
FEATURES = [
    "occupancy",
    "duration",
    "volume",
    "payment",
    "temperature",
    "humidity",
    "rainfall",
    "hour_sin",
    "hour_cos",
    "day_sin",
    "day_cos",
    "is_holiday"
]

TARGETS = [
    "occupancy",
    "volume"
]

# =========================================================
# 设备
# =========================================================

device = torch.device(
    "cuda" if torch.cuda.is_available() else "cpu"
)

print("使用设备:", device)


# =========================================================
# 1. 读取已经整理好的数据
# =========================================================

print("\n正在读取数据...")

df = pd.read_csv(DATA_PATH)

df["time"] = pd.to_datetime(df["time"])

df = df.sort_values(
    ["station_id", "time"]
).reset_index(drop=True)

# =========================================================
# 2. 获取指定充电站
# =========================================================

station_df = df[
    df["station_id"] == STATION_ID
].copy()

if len(station_df) == 0:
    raise ValueError(
        f"找不到 station_id={STATION_ID}"
    )


station_df = station_df.sort_values("time")


print(
    f"充电站 {STATION_ID} 数据量:",
    len(station_df)
)

# =========================================================
# 3. 获取充电桩总数量
# =========================================================

charge_count = int(
    station_df["charge_count"].iloc[-1]
)

print(
    "充电桩总数:",
    charge_count
)

# =========================================================
# 4. 获取最近24小时数据
# =========================================================

if len(station_df) < HISTORY_LENGTH:
    raise ValueError(
        f"充电站 {STATION_ID} 的历史数据不足24小时"
    )


history = station_df.tail(
    HISTORY_LENGTH
).copy()


print("\n输入数据时间范围:")
print(
    history["time"].iloc[0],
    "->",
    history["time"].iloc[-1]
)

# =========================================================
# 5. 加载 scaler
# =========================================================

print("\n正在加载 scaler...")

x_scaler = joblib.load(
    X_SCALER_PATH
)

y_scaler = joblib.load(
    Y_SCALER_PATH
)


# =========================================================
# 6. 对输入数据进行标准化
# =========================================================

X = history[FEATURES].values.astype(
    np.float32
)

X = x_scaler.transform(X)

X = torch.tensor(
    X,
    dtype=torch.float32
).unsqueeze(0)

# =========================================================
# 7. 加载模型
# =========================================================

print("正在加载模型...")

model = LSTMModel(
    input_size=12,
    hidden_size=128,
    num_layers=2,
    prediction_length=24,
    output_size=2,
    dropout=0.2
)

checkpoint = torch.load(
    MODEL_PATH,
    map_location=device
)

model.load_state_dict(
    checkpoint["model_state_dict"]
)

model = model.to(device)

model.eval()

print(
    "模型加载成功，最佳 Epoch:",
    checkpoint["epoch"]
)


# =========================================================
# 8. 预测未来24小时
# =========================================================

with torch.no_grad():

    X = X.to(device)

    prediction = model(X)

    prediction = prediction.cpu().numpy()


# prediction:
# (1, 24, 2)

prediction = prediction[0]


# =========================================================
# 9. 反标准化
# =========================================================

prediction_real = y_scaler.inverse_transform(
    prediction
)


# =========================================================
# 10. 提取预测结果
# =========================================================

pred_occupancy = prediction_real[:, 0]

pred_volume = prediction_real[:, 1]


# occupancy 理论上应该在0~1之间
pred_occupancy = np.clip(
    pred_occupancy,
    0,
    1
)

# 充电量不能为负
pred_volume = np.maximum(
    pred_volume,
    0
)


# =========================================================
# 11. 生成未来24小时的时间
# =========================================================

last_time = history["time"].iloc[-1]

future_times = pd.date_range(
    start=last_time + pd.Timedelta(hours=1),
    periods=PREDICTION_LENGTH,
    freq="h"
)


# =========================================================
# 12. 计算空闲充电桩数量
# =========================================================

pred_idle_piles = (
    charge_count *
    (1 - pred_occupancy)
)

pred_idle_piles = np.round(
    pred_idle_piles
).astype(int)


# =========================================================
# 13. 计算未来1h / 6h / 24h
# =========================================================

load_1h = pred_volume[:1].sum()

load_6h = pred_volume[:6].sum()

load_24h = pred_volume[:24].sum()


# 平均充电负荷
# 对1小时数据来说：
# kWh / 1h ≈ kW
avg_load_1h = load_1h / 1

avg_load_6h = load_6h / 6

avg_load_24h = load_24h / 24


# =========================================================
# 14. 寻找高峰充电时段
# =========================================================

peak_index = np.argmax(
    pred_occupancy
)

peak_time = future_times[
    peak_index
]

peak_occupancy = pred_occupancy[
    peak_index
]

peak_volume = pred_volume[
    peak_index
]


# =========================================================
# 15. 输出总体预测结果
# =========================================================

print("\n")
print("=" * 60)
print("          电动汽车充电负荷预测结果")
print("=" * 60)

print(
    f"充电站 ID       : {STATION_ID}"
)

print(
    f"充电桩总数      : {charge_count}"
)

print(
    f"历史数据截止时间: {last_time}"
)

print(
    f"预测时间范围    : "
    f"{future_times[0]} -> {future_times[-1]}"
)


print("\n" + "-" * 60)
print("未来充电负荷")
print("-" * 60)

print(
    f"未来 1 小时充电量 : {load_1h:.2f} kWh"
)

print(
    f"未来 6 小时充电量 : {load_6h:.2f} kWh"
)

print(
    f"未来24小时充电量 : {load_24h:.2f} kWh"
)

print()

print(
    f"未来 1 小时平均负荷: {avg_load_1h:.2f} kW"
)

print(
    f"未来 6 小时平均负荷: {avg_load_6h:.2f} kW"
)

print(
    f"未来24小时平均负荷: {avg_load_24h:.2f} kW"
)


print("\n" + "-" * 60)
print("高峰充电时段")
print("-" * 60)

print(
    f"高峰时段         : {peak_time}"
)

print(
    f"预测占用率       : {peak_occupancy:.2%}"
)

print(
    f"预测充电量       : {peak_volume:.2f} kWh"
)

print(
    f"预计空闲充电桩   : {pred_idle_piles[peak_index]} 个"
)


print("\n" + "-" * 60)
print("未来24小时逐小时预测")
print("-" * 60)

print(
    f"{'时间':<22}"
    f"{'占用率':<12}"
    f"{'充电量(kWh)':<16}"
    f"{'空闲桩数':<10}"
)

print("-" * 60)

for i in range(24):

    print(
        f"{str(future_times[i]):<22}"
        f"{pred_occupancy[i]:<12.2%}"
        f"{pred_volume[i]:<16.2f}"
        f"{pred_idle_piles[i]:<10}"
    )


print("=" * 60)