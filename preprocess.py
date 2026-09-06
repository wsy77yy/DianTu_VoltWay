import os
import pandas as pd
import numpy as np

# ============================================================
# 1. 路径配置
# ============================================================

DATA_DIR = "./data/raw"
OUTPUT_DIR = "./data/processed"

OCCUPANCY_FILE = os.path.join(DATA_DIR, "occupancy.csv")
DURATION_FILE = os.path.join(DATA_DIR, "duration.csv")
VOLUME_FILE = os.path.join(DATA_DIR, "volume.csv")

E_PRICE_FILE = os.path.join(DATA_DIR, "e_price.csv")
S_PRICE_FILE = os.path.join(DATA_DIR, "s_price.csv")

WEATHER_FILE = os.path.join(DATA_DIR, "weather_central.csv")
INF_FILE = os.path.join(DATA_DIR, "inf.csv")

OUTPUT_FILE = os.path.join(
    OUTPUT_DIR,
    "processed_data.csv"
)


# ============================================================
# 2. 节假日
# ============================================================
HOLIDAYS = {
    # 2022
    "2022-01-01",
    "2022-01-02",
    "2022-01-03",

    "2022-01-31",
    "2022-02-01",
    "2022-02-02",
    "2022-02-03",
    "2022-02-04",
    "2022-02-05",
    "2022-02-06",

    "2022-04-03",
    "2022-04-04",
    "2022-04-05",

    "2022-04-30",
    "2022-05-01",
    "2022-05-02",
    "2022-05-03",
    "2022-05-04",

    "2022-06-03",
    "2022-06-04",
    "2022-06-05",

    "2022-09-10",
    "2022-09-11",
    "2022-09-12",

    "2022-10-01",
    "2022-10-02",
    "2022-10-03",
    "2022-10-04",
    "2022-10-05",
    "2022-10-06",
    "2022-10-07",

    # 2023
    "2023-01-01",

    "2023-01-21",
    "2023-01-22",
    "2023-01-23",
    "2023-01-24",
    "2023-01-25",
    "2023-01-26",
    "2023-01-27",

    "2023-04-05",

    "2023-04-29",
    "2023-04-30",
    "2023-05-01",
    "2023-05-02",
    "2023-05-03",

    "2023-06-22",
    "2023-06-23",
    "2023-06-24",

    "2023-09-29",
    "2023-09-30",
    "2023-10-01",
    "2023-10-02",
    "2023-10-03",
    "2023-10-04",
    "2023-10-05",
    "2023-10-06",
}
# ============================================================
# 3. 工具函数
# ============================================================

def check_file(path):
    if not os.path.exists(path):
        return False
    return True


def load_wide_csv(path, value_name):
    """
    读取类似：
    time,102,104,105
    2022-09-01 00:00:00,17,1,15
    的 UrbanEV 宽表。
    转换为：
    time                 TAZID    value
    2022-09-01 00:00     102      17
    2022-09-01 00:00     104       1
    2022-09-01 00:00     105      15
    """

    if not check_file(path):
        raise FileNotFoundError(
            f"找不到文件：{path}"
        )

    df = pd.read_csv(path)

    # 第一列作为时间
    time_col = df.columns[0]

    df[time_col] = pd.to_datetime(
        df[time_col],
        errors="coerce"
    )

    df.rename(
        columns={
            time_col: "time"
        },
        inplace=True
    )

    # 宽表 -> 长表
    df = df.melt(
        id_vars=["time"],
        var_name="TAZID",
        value_name=value_name
    )

    # TAZID 统一为字符串
    df["TAZID"] = (
        df["TAZID"]
        .astype(str)
        .str.strip()
    )

    # 转换成数字
    df[value_name] = pd.to_numeric(
        df[value_name],
        errors="coerce"
    )

    return df


# ============================================================
# 4. 读取 occupancy / duration / volume
# ============================================================

def load_charging_data():

    print("=" * 60)
    print("读取充电数据")
    print("=" * 60)

    occupancy = load_wide_csv(
        OCCUPANCY_FILE,
        "occupancy_raw"
    )

    duration = load_wide_csv(
        DURATION_FILE,
        "duration"
    )

    volume = load_wide_csv(
        VOLUME_FILE,
        "volume"
    )

    print("occupancy:", occupancy.shape)
    print("duration :", duration.shape)
    print("volume   :", volume.shape)

    # --------------------------------------------------------
    # 合并
    # --------------------------------------------------------

    data = occupancy.merge(
        duration,
        on=["time", "TAZID"],
        how="outer"
    )

    data = data.merge(
        volume,
        on=["time", "TAZID"],
        how="outer"
    )

    return data


# ============================================================
# 5. 根据 e_price + s_price + volume 计算 payment
# ============================================================

def calculate_payment(data):

    print("=" * 60)
    print("计算 payment")
    print("=" * 60)

    print(
        "payment = volume × (e_price + s_price)"
    )

    # --------------------------------------------------------
    # 读取电价
    # --------------------------------------------------------

    e_price = load_wide_csv(
        E_PRICE_FILE,
        "e_price"
    )

    # --------------------------------------------------------
    # 读取服务费
    # --------------------------------------------------------

    s_price = load_wide_csv(
        S_PRICE_FILE,
        "s_price"
    )

    print("e_price:", e_price.shape)
    print("s_price:", s_price.shape)

    # --------------------------------------------------------
    # 合并价格
    # --------------------------------------------------------

    price = e_price.merge(
        s_price,
        on=["time", "TAZID"],
        how="outer"
    )

    # --------------------------------------------------------
    # 合并到主数据
    # --------------------------------------------------------

    data = data.merge(
        price,
        on=["time", "TAZID"],
        how="left"
    )

    # --------------------------------------------------------
    # 检查价格缺失
    # --------------------------------------------------------

    print(
        "e_price 缺失：",
        data["e_price"].isna().sum()
    )

    print(
        "s_price 缺失：",
        data["s_price"].isna().sum()
    )

    # --------------------------------------------------------
    # 计算 payment
    # --------------------------------------------------------

    data["payment"] = (
        data["volume"]
        * (
            data["e_price"]
            + data["s_price"]
        )
    )

    # --------------------------------------------------------
    # 删除中间变量
    # --------------------------------------------------------

    data.drop(
        columns=[
            "e_price",
            "s_price"
        ],
        inplace=True
    )

    print()
    print("payment 计算完成。")

    print(
        "payment 最小值：",
        data["payment"].min()
    )

    print(
        "payment 最大值：",
        data["payment"].max()
    )

    print(
        "payment 平均值：",
        data["payment"].mean()
    )

    return data


# ============================================================
# 6. 读取充电站信息
# ============================================================

def load_station_info():

    print("=" * 60)
    print("读取充电站信息")
    print("=" * 60)

    if not check_file(INF_FILE):
        raise FileNotFoundError(
            f"找不到文件：{INF_FILE}"
        )

    inf = pd.read_csv(
        INF_FILE
    )

    required_columns = [
        "station_id",
        "longitude",
        "latitude",
        "charge_count",
        "TAZID"
    ]

    for col in required_columns:

        if col not in inf.columns:

            raise ValueError(
                f"inf.csv 中缺少字段：{col}"
            )

    inf["TAZID"] = (
        inf["TAZID"]
        .astype(str)
        .str.strip()
    )

    return inf


# ============================================================
# 7. 合并 station 信息
# ============================================================

def merge_station_info(data, inf):

    print("=" * 60)
    print("合并充电站信息")
    print("=" * 60)

    station_info = inf[
        [
            "TAZID",
            "station_id",
            "longitude",
            "latitude",
            "charge_count"
        ]
    ].copy()

    data = data.merge(
        station_info,
        on="TAZID",
        how="left"
    )

    # 检查 TAZID 是否成功匹配
    missing = data["station_id"].isna().sum()

    print(
        "无法匹配 station_id 的数据：",
        missing
    )

    if missing > 0:

        missing_taz = (
            data.loc[
                data["station_id"].isna(),
                "TAZID"
            ]
            .drop_duplicates()
            .tolist()
        )

        print(
            "无法匹配的 TAZID：",
            missing_taz[:20]
        )

    return data


# ============================================================
# 8. 处理 occupancy
# ============================================================

def process_occupancy(data):

    print("=" * 60)
    print("处理 occupancy")
    print("=" * 60)

    # 原始 occupancy：
    # 某个站在这个小时的占用数量
    #
    # charge_count：
    # 这个站拥有多少充电桩
    #
    # occupancy_rate：
    # 占用数量 / 充电桩数量

    data["occupancy"] = (
        data["occupancy_raw"]
        / data["charge_count"]
    )

    # 限制到 0~1
    data["occupancy"] = data[
        "occupancy"
    ].clip(
        lower=0,
        upper=1
    )

    data.drop(
        columns=["occupancy_raw"],
        inplace=True
    )

    return data


# ============================================================
# 9. 读取天气
# ============================================================

def load_weather():

    print("=" * 60)
    print("读取天气数据")
    print("=" * 60)

    if not check_file(WEATHER_FILE):

        raise FileNotFoundError(
            f"找不到文件：{WEATHER_FILE}"
        )

    weather = pd.read_csv(
        WEATHER_FILE
    )

    # 第一列是时间
    if "time" not in weather.columns:

        weather.rename(
            columns={
                weather.columns[0]: "time"
            },
            inplace=True
        )

    weather["time"] = pd.to_datetime(
        weather["time"],
        errors="coerce"
    )

    # UrbanEV：
    #
    # T     -> temperature
    # U     -> humidity
    # nRAIN -> rainfall

    rename_dict = {}

    if "T" in weather.columns:
        rename_dict["T"] = "temperature"

    if "U" in weather.columns:
        rename_dict["U"] = "humidity"

    if "nRAIN" in weather.columns:
        rename_dict["nRAIN"] = "rainfall"

    weather.rename(
        columns=rename_dict,
        inplace=True
    )

    keep_columns = ["time"]

    for col in [
        "temperature",
        "humidity",
        "rainfall"
    ]:

        if col in weather.columns:
            keep_columns.append(col)

    weather = weather[
        keep_columns
    ]

    # 同一个时间只保留一条
    weather = weather.drop_duplicates(
        subset=["time"]
    )

    return weather


# ============================================================
# 10. 合并天气
# ============================================================

def merge_weather(data, weather):

    print("=" * 60)
    print("合并天气")
    print("=" * 60)

    data = data.merge(
        weather,
        on="time",
        how="left"
    )

    return data


# ============================================================
# 11. 添加时间特征
# ============================================================

def add_time_features(data):

    print("=" * 60)
    print("添加时间特征")
    print("=" * 60)

    data["hour"] = data["time"].dt.hour
    data["day_of_week"] = data["time"].dt.dayofweek

    # 小时周期编码
    data["hour_sin"] = np.sin(
        2 * np.pi * data["hour"] / 24
    )
    data["hour_cos"] = np.cos(
        2 * np.pi * data["hour"] / 24
    )
    # 星期周期编码
    data["day_sin"] = np.sin(
        2 * np.pi * data["day_of_week"] / 7
    )
    data["day_cos"] = np.cos(
        2 * np.pi * data["day_of_week"] / 7
    )
    
    data["is_weekend"] = (
        data["day_of_week"] >= 5
    ).astype(int)

    date_string = (
        data["time"]
        .dt.strftime("%Y-%m-%d")
    )

    data["is_holiday"] = (
        date_string.isin(
            HOLIDAYS
        )
    ).astype(int)

    return data


# ============================================================
# 12. 缺失值处理
# ============================================================

def fill_missing_values(data):

    print("=" * 60)
    print("处理缺失值")
    print("=" * 60)

    numeric_columns = [
        "occupancy",
        "duration",
        "volume",
        "payment",
        "temperature",
        "humidity",
        "rainfall"
    ]

    for col in numeric_columns:

        if col not in data.columns:
            continue

        missing = data[col].isna().sum()

        if missing == 0:
            continue

        print(
            f"{col}: {missing} 个缺失值"
        )

        # 按 TAZID 按时间进行插值
        data = data.sort_values(
            ["TAZID", "time"]
        )

        data[col] = (
            data
            .groupby("TAZID")[col]
            .transform(
                lambda x:
                x.interpolate(
                    method="linear",
                    limit_direction="both"
                )
            )
        )

        # 如果整个站点都是缺失
        # 最后使用 0
        data[col] = (
            data[col]
            .fillna(0)
        )

    return data


# ============================================================
# 13. 整理字段
# ============================================================

def organize_columns(data):

    columns = [
        # 时间
        "time",

        # 空间
        "station_id",
        "TAZID",
        "longitude",
        "latitude",
        "charge_count",

        # 充电行为
        "occupancy",
        "duration",
        "volume",
        "payment",

        # 天气
        "temperature",
        "humidity",
        "rainfall",

        # 时间特征
        "hour_sin",
        "hour_cos",
        "day_sin",
        "day_cos",
        "is_weekend",
        "is_holiday",
    ]

    columns = [
        col
        for col in columns
        if col in data.columns
    ]

    data = data[
        columns
    ]

    return data


# ============================================================
# 14. 主程序
# ============================================================

def main():

    print()
    print("=" * 70)
    print("EV Charging Prediction - Preprocessing")
    print("=" * 70)
    print()

    # 创建输出目录
    os.makedirs(
        OUTPUT_DIR,
        exist_ok=True
    )

    # --------------------------------------------------------
    # Step 1
    # 读取 occupancy / duration / volume
    # --------------------------------------------------------

    data = load_charging_data()

    # --------------------------------------------------------
    # Step 2
    # 读取 station 信息
    # --------------------------------------------------------

    inf = load_station_info()

    # --------------------------------------------------------
    # Step 3
    # TAZID -> station_id
    # --------------------------------------------------------

    data = merge_station_info(
        data,
        inf
    )

    # --------------------------------------------------------
    # Step 4
    # occupancy 转成占用率
    # --------------------------------------------------------

    data = process_occupancy(
        data
    )

    # --------------------------------------------------------
    # Step 5
    # 计算 payment
    # --------------------------------------------------------

    data = calculate_payment(
        data
    )

    # --------------------------------------------------------
    # Step 6
    # 天气
    # --------------------------------------------------------

    weather = load_weather()

    data = merge_weather(
        data,
        weather
    )

    # --------------------------------------------------------
    # Step 7
    # 时间特征
    # --------------------------------------------------------

    data = add_time_features(
        data
    )

    # --------------------------------------------------------
    # Step 8
    # 缺失值
    # --------------------------------------------------------

    data = fill_missing_values(
        data
    )

    # --------------------------------------------------------
    # Step 9
    # 整理字段
    # --------------------------------------------------------

    data = organize_columns(
        data
    )

    # --------------------------------------------------------
    # Step 10
    # 排序
    # --------------------------------------------------------

    data = data.sort_values(
        ["station_id", "time"]
    ).reset_index(
        drop=True
    )

    # --------------------------------------------------------
    # Step 11
    # 保存
    # --------------------------------------------------------

    data.to_csv(
        OUTPUT_FILE,
        index=False
    )

    # ========================================================
    # 输出检查
    # ========================================================

    print()
    print("=" * 70)
    print("预处理完成！")
    print("=" * 70)

    print()
    print("输出文件：")
    print(OUTPUT_FILE)

    print()
    print("数据规模：")
    print(
        f"行数：{len(data)}"
    )
    print(
        f"列数：{len(data.columns)}"
    )

    print()
    print("字段：")
    print(
        data.columns.tolist()
    )

    print()
    print("前 10 行：")
    print(
        data.head(10).to_string()
    )

    print()
    print("缺失值：")
    print(
        data.isna().sum()
    )

    print()
    print("充电站数量：")
    print(
        data["station_id"].nunique()
    )

    print()
    print("时间范围：")
    print(
        data["time"].min()
    )
    print("到")
    print(
        data["time"].max()
    )

    print()
    print("payment 统计：")
    print(
        data["payment"].describe()
    )

    print()
    print("=" * 70)
    print("Done.")
    print("=" * 70)


if __name__ == "__main__":
    main()