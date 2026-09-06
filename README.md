┌──────────────────────────────────────┐
│            数据粒度：1小时            │
├──────────────────────────────────────┤
│                                      │
│ 输入：过去24小时                      │
│                                      │
│ ① occupancy                          │
│ ② duration                           │
│ ③ volume                             │
│ ④ hour                               │
│ ⑤ day_of_week                        │
│ ⑥ weekend                            │
│ ⑦ holiday                            │
│ ⑧ temperature                        │
│ ⑨ humidity                           │
│ ⑩ rainfall                           │
│ ⑪ electricity_price                  │
│ ⑫ service_price                      │
│ ⑬ total_piles                        │
│ ⑭ charging_capacity                  │
│                                      │
├──────────────────────────────────────┤
│                                      │
│              一个统一模型             │
│        所有充电站共同训练              │
│                                      │
├──────────────────────────────────────┤
│                                      │
│ 输出：未来24小时                      │
│                                      │
│ 每小时：                              │
│     occupancy                        │
│     volume                           │
│                                      │
├──────────────────────────────────────┤
│                                      │
│ 后处理：                              │
│ occupancy → 空闲桩数量                │
│ occupancy → 高峰充电时段              │
│ volume → 充电需求/负荷                │
│                                      │
└──────────────────────────────────────┘
各充电站、各电桩的历史充电时段（充电时长）、充电量、用户出行时段、天气（气温、降雨）、节假日（是/否）等。
空间信息？（邻接，不做空间了，没有实际充电桩位置信息，别人的参考不了

历史：occupation（占用数/站点总数），

空间单位：充电站
时间粒度：1小时
历史窗口：过去24小时
预测窗口：未来24小时
模型：所有站点共用一个模型
直接预测：未来每小时 occupancy + volume

预测未来 1 小时、6 小时、24 小时各站点的充电负荷、空闲桩数量、高峰充电时段。

模型：集成学习，LSTM(GRU)，

每站，每小时：
occupancy（占用的充电桩/该站充电桩总数）
duration（
volume（过去一小时用电量）

e_price
s_price

temperature
humidity
rainfall

hour（几点）
day_of_week（周几）
is_holiday（是节假日吗）
=====================
预处理输出：
=====================
输出文件：
./data/processed/processed_data.csv

数据规模：
行数：5916528
列数：17

字段：
['time', 'station_id', 'TAZID', 'longitude', 'latitude', 'charge_count', 'occupancy', 'duration', 'volume', 'payment', 'temperature', 'humidity', 'rainfall', 'hour', 'day_of_week', 'is_weekend', 'is_holiday']

前 10 行：
                 time  station_id TAZID   longitude   latitude  charge_count  occupancy  duration    volume    payment  temperature  humidity  rainfall  hour  day_of_week  is_weekend  is_holiday
0 2022-09-01 00:00:00        1001   559  113.784724  22.714121            20       0.20  0.583333  4.083333   6.819167         27.3        87         0     0            3           0           0
1 2022-09-01 01:00:00        1001   559  113.784724  22.714121            20       0.35  0.416667  2.916667   4.870833         27.3        87         0     1            3           0           0
2 2022-09-01 02:00:00        1001   559  113.784724  22.714121            20       0.35  0.750000  5.250000   8.767500         27.3        87         0     2            3           0           0
3 2022-09-01 03:00:00        1001   559  113.784724  22.714121            20       0.40  0.750000  5.250000   8.767500         27.3        87         0     3            3           0           0
4 2022-09-01 04:00:00        1001   559  113.784724  22.714121            20       0.30  0.583333  4.083333   6.819167         27.3        87         0     4            3           0           0
5 2022-09-01 05:00:00        1001   559  113.784724  22.714121            20       0.30  0.416667  2.916667   4.870833         27.2        89         0     5            3           0           0
6 2022-09-01 06:00:00        1001   559  113.784724  22.714121            20       1.00  0.750000  5.250000   8.767500         27.2        89         0     6            3           0           0
7 2022-09-01 07:00:00        1001   559  113.784724  22.714121            20       0.45  0.416667  2.916667   4.870833         27.2        89         0     7            3           0           0
8 2022-09-01 08:00:00        1001   559  113.784724  22.714121            20       1.00  1.000000  7.000000  11.690000         28.6        80         0     8            3           0           0
9 2022-09-01 09:00:00        1001   559  113.784724  22.714121            20       0.50  0.916667  6.416667  10.715833         28.6        80         0     9            3           0           0

缺失值：
time            0
station_id      0
TAZID           0
longitude       0
latitude        0
charge_count    0
occupancy       0
duration        0
volume          0
payment         0
temperature     0
humidity        0
rainfall        0
hour            0
day_of_week     0
is_weekend      0
is_holiday      0
dtype: int64

充电站数量：
1362

时间范围：
2022-09-01 00:00:00
到
2023-02-28 23:00:00

payment 统计：
count    5.916528e+06
mean     4.652908e+02
std      1.270756e+03
min      0.000000e+00
25%      6.400000e+01
50%      1.542291e+02
75%      4.048193e+02
max      1.974435e+04
Name: payment, dtype: float64
===============
构建数据集
===============
Train
2022-09-01 ~ 2023-01-15
Val
2023-01-16 ~ 2023-02-01
Test
2023-02-02 ~ 2023-02-28
75%，10%，15%
窗口是24h
=============
model
=============
LSTM
                    历史24小时
                         │
                         ↓
                  ┌────────────┐
                  │    LSTM    │
                  │ 2 layers   │
                  │ hidden=128 │
                  └──────┬─────┘
                         │
                    最后隐藏状态
                       [128]
                         │
                         ↓
                  Fully Connected
                       128
                        ↓
                        48
                        ↓
                     reshape
                        ↓
                     [24,2]
                   ↙         ↘
             occupancy      volume
输入 shape: torch.Size([32, 24, 10])
输出 shape: torch.Size([32, 24, 2])
#(batch size=32,24h,input feature 10,output feature 2)

模型结构：
LSTMModel(
  (lstm): LSTM(10, 128, num_layers=2, batch_first=True, dropout=0.2)
  (fc): Linear(in_features=128, out_features=48, bias=True)
)

模型参数量: 209968