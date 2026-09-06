import torch
import numpy as np
from torch.utils.data import DataLoader

from dataset import build_datasets
from model import LSTMModel

# =========================
# 配置
# =========================
BATCH_SIZE = 64
MODEL_PATH = "checkpoints/best_model.pth"

# =========================
# 设备
# =========================
device = torch.device(
    "cuda" if torch.cuda.is_available() else "cpu"
)

print("使用设备:", device)


# =========================
# 加载数据
# =========================
(
    train_dataset,
    val_dataset,
    test_dataset,
    x_scaler,
    y_scaler
) = build_datasets()


test_loader = DataLoader(
    test_dataset,
    batch_size=BATCH_SIZE,
    shuffle=False
)


# =========================
# 加载模型
# =========================
model = LSTMModel(
    input_size=12,
    prediction_length=24,
    output_size=2
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

print("模型加载成功")
print("最佳 Epoch:", checkpoint["epoch"])
print("最佳 Val Loss:", checkpoint["val_loss"])


# =========================
# 测试
# =========================
all_predictions = []
all_targets = []


with torch.no_grad():

    for X, Y in test_loader:

        X = X.to(device)

        prediction = model(X)

        all_predictions.append(
            prediction.cpu().numpy()
        )

        all_targets.append(
            Y.numpy()
        )


predictions = np.concatenate(
    all_predictions,
    axis=0
)

targets = np.concatenate(
    all_targets,
    axis=0
)


print("\n标准化空间预测结果：")
print("Prediction:", predictions.shape)
print("Target:", targets.shape)
print("\n========== Raw Prediction ==========")
print(predictions[0, :5])

print("\n========== Raw Target ==========")
print(targets[0, :5])

print("\n========== Y Scaler ==========")
print("mean:", y_scaler.mean_)
print("scale:", y_scaler.scale_)

# =========================
# 反标准化
# =========================
predictions_2d = predictions.reshape(-1, 2)
targets_2d = targets.reshape(-1, 2)

predictions_real = y_scaler.inverse_transform(
    predictions_2d
)

targets_real = y_scaler.inverse_transform(
    targets_2d
)


predictions_real = predictions_real.reshape(
    predictions.shape
)

targets_real = targets_real.reshape(
    targets.shape
)


# =========================
# 分别计算指标
# =========================

# occupancy
pred_occ = predictions_real[:, :, 0]
true_occ = targets_real[:, :, 0]

# volume
pred_volume = predictions_real[:, :, 1]
true_volume = targets_real[:, :, 1]


# MAE
occ_mae = np.mean(
    np.abs(pred_occ - true_occ)
)

volume_mae = np.mean(
    np.abs(pred_volume - true_volume)
)


# RMSE
occ_rmse = np.sqrt(
    np.mean(
        (pred_occ - true_occ) ** 2
    )
)

volume_rmse = np.sqrt(
    np.mean(
        (pred_volume - true_volume) ** 2
    )
)


print("\n========== Test Results ==========")

print(
    f"Occupancy MAE : {occ_mae:.6f}"
)

print(
    f"Occupancy RMSE: {occ_rmse:.6f}"
)

print(
    f"Volume MAE    : {volume_mae:.6f}"
)

print(
    f"Volume RMSE   : {volume_rmse:.6f}"
)


# =========================
# 查看一个样本
# =========================

sample = 0

print("\n========== Sample Prediction ==========")

print("未来24小时：")

for hour in range(24):

    print(
        f"+{hour + 1:02d}h | "
        f"Occupancy: "
        f"{pred_occ[sample, hour]:.4f} | "
        f"Volume: "
        f"{pred_volume[sample, hour]:.2f}"
    )