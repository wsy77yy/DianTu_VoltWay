import pandas as pd
import numpy as np
import torch

from torch.utils.data import Dataset
from sklearn.preprocessing import StandardScaler


# ============================================================
# 配置
# ============================================================

DATA_PATH = "data/processed/processed_data.csv"

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

HISTORY_LENGTH = 24
PREDICTION_LENGTH = 24


# ============================================================
# Dataset
# ============================================================

class EVTimeSeriesDataset(Dataset):

    def __init__(self, X, Y):

        self.X = torch.tensor(
            X,
            dtype=torch.float32
        )

        self.Y = torch.tensor(
            Y,
            dtype=torch.float32
        )

    def __len__(self):
        return len(self.X)

    def __getitem__(self, index):

        return self.X[index], self.Y[index]


# ============================================================
# 创建滑动窗口
# ============================================================

def create_sequences(
    data,
    X_scaled,
    Y_scaled
):

    X_list = []
    Y_list = []

    total_length = (
        HISTORY_LENGTH
        +
        PREDICTION_LENGTH
    )

    # station_id 使用原始 data，
    # X/Y 使用对应的 scaled array
    station_ids = data["station_id"].values

    # 每个 station 单独构造序列
    unique_stations = data["station_id"].unique()

    for station_id in unique_stations:

        indices = np.where(
            station_ids == station_id
        )[0]

        # 这些 index 对应当前 station 的连续时间数据
        X_station = X_scaled[indices]
        Y_station = Y_scaled[indices]

        for i in range(
            len(indices) - total_length + 1
        ):

            X = X_station[
                i:
                i + HISTORY_LENGTH
            ]

            Y = Y_station[
                i + HISTORY_LENGTH:
                i + HISTORY_LENGTH + PREDICTION_LENGTH
            ]

            X_list.append(X)
            Y_list.append(Y)

    X_array = np.asarray(
        X_list,
        dtype=np.float32
    )

    Y_array = np.asarray(
        Y_list,
        dtype=np.float32
    )

    return X_array, Y_array


# ============================================================
# 构建数据集
# ============================================================

def build_datasets():

    print("正在读取数据...")

    df = pd.read_csv(
        DATA_PATH
    )

    df["time"] = pd.to_datetime(
        df["time"]
    )

    df = df.sort_values(
        ["station_id", "time"]
    ).reset_index(drop=True)

    print(
        "数据量:",
        len(df)
    )

    print(
        "充电站数量:",
        df["station_id"].nunique()
    )

    print(
        "时间范围:",
        df["time"].min(),
        "->",
        df["time"].max()
    )

    # ========================================================
    # Train / Val / Test
    # ========================================================

    train_end = pd.Timestamp(
        "2023-01-15 23:00:00"
    )

    val_end = pd.Timestamp(
        "2023-02-01 23:00:00"
    )

    train_df = df[
        df["time"] <= train_end
    ].copy()

    val_df = df[
        (df["time"] > train_end)
        &
        (df["time"] <= val_end)
    ].copy()

    test_df = df[
        df["time"] > val_end
    ].copy()

    print("\n数据集划分：")

    print(
        "Train:",
        train_df["time"].min(),
        "->",
        train_df["time"].max(),
        "数量:",
        len(train_df)
    )

    print(
        "Val:",
        val_df["time"].min(),
        "->",
        val_df["time"].max(),
        "数量:",
        len(val_df)
    )

    print(
        "Test:",
        test_df["time"].min(),
        "->",
        test_df["time"].max(),
        "数量:",
        len(test_df)
    )

    # ========================================================
    # 原始 X / Y
    # ========================================================

    X_train_raw = train_df[
        FEATURES
    ].values

    X_val_raw = val_df[
        FEATURES
    ].values

    X_test_raw = test_df[
        FEATURES
    ].values

    Y_train_raw = train_df[
        TARGETS
    ].values

    Y_val_raw = val_df[
        TARGETS
    ].values

    Y_test_raw = test_df[
        TARGETS
    ].values

    # ========================================================
    # X Scaler
    # ========================================================

    x_scaler = StandardScaler()

    X_train_scaled = x_scaler.fit_transform(
        X_train_raw
    )

    X_val_scaled = x_scaler.transform(
        X_val_raw
    )

    X_test_scaled = x_scaler.transform(
        X_test_raw
    )

    # ========================================================
    # Y Scaler
    # ========================================================

    y_scaler = StandardScaler()

    Y_train_scaled = y_scaler.fit_transform(
        Y_train_raw
    )

    Y_val_scaled = y_scaler.transform(
        Y_val_raw
    )

    Y_test_scaled = y_scaler.transform(
        Y_test_raw
    )

    # ========================================================
    # Scaler 检查
    # ========================================================

    print("\n========== Y Scaler ==========")

    print(
        "mean:",
        y_scaler.mean_
    )

    print(
        "scale:",
        y_scaler.scale_
    )

    # ========================================================
    # 创建序列
    # ========================================================

    print("\n正在创建 Train 序列...")

    X_train, Y_train = create_sequences(
        train_df,
        X_train_scaled,
        Y_train_scaled
    )

    print(
        "Train 完成:",
        X_train.shape,
        Y_train.shape
    )

    print("\n正在创建 Val 序列...")

    X_val, Y_val = create_sequences(
        val_df,
        X_val_scaled,
        Y_val_scaled
    )

    print(
        "Val 完成:",
        X_val.shape,
        Y_val.shape
    )

    print("\n正在创建 Test 序列...")

    X_test, Y_test = create_sequences(
        test_df,
        X_test_scaled,
        Y_test_scaled
    )

    print(
        "Test 完成:",
        X_test.shape,
        Y_test.shape
    )

    # ========================================================
    # Dataset
    # ========================================================

    train_dataset = EVTimeSeriesDataset(
        X_train,
        Y_train
    )

    val_dataset = EVTimeSeriesDataset(
        X_val,
        Y_val
    )

    test_dataset = EVTimeSeriesDataset(
        X_test,
        Y_test
    )

    # ========================================================
    # 最终检查
    # ========================================================

    print("\n========== 数据集 ==========")

    print(
        "Train:",
        len(train_dataset)
    )

    print(
        "Val:",
        len(val_dataset)
    )

    print(
        "Test:",
        len(test_dataset)
    )

    # ========================================================
    # 检查 Y 是否可以正确恢复
    # ========================================================

    sample_y = Y_train[0, 0].reshape(
        1, -1
    )

    sample_y_real = y_scaler.inverse_transform(
        sample_y
    )

    print("\n========== Y 反标准化检查 ==========")

    print(
        "标准化 Y:",
        sample_y
    )

    print(
        "恢复后的 Y:",
        sample_y_real
    )

    return (
        train_dataset,
        val_dataset,
        test_dataset,
        x_scaler,
        y_scaler
    )


# ============================================================
# 测试
# ============================================================

if __name__ == "__main__":

    (
        train_dataset,
        val_dataset,
        test_dataset,
        x_scaler,
        y_scaler
    ) = build_datasets()

    print("\n==============================")
    print("Dataset 构建完成")
    print("==============================")

    X, Y = train_dataset[0]

    print(
        "X shape:",
        X.shape
    )

    print(
        "Y shape:",
        Y.shape
    )

    print("\nX 第一小时:")
    print(X[0])

    print("\nY 前5小时:")
    print(Y[:5])