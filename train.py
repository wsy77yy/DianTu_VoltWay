import torch
import torch.nn as nn
from torch.utils.data import DataLoader

from dataset import build_datasets
from model import LSTMModel
import joblib
# ============================================================
# 配置
# ============================================================

BATCH_SIZE = 64
LEARNING_RATE = 0.001
EPOCHS = 50
MODEL_SAVE_PATH = "checkpoints/best_model.pth"

# ============================================================
# 设备
# ============================================================

device = torch.device(
    "cuda" if torch.cuda.is_available() else "cpu"
)

print("使用设备:", device)


# ============================================================
# 创建保存目录
# ============================================================

import os

os.makedirs(
    "checkpoints",
    exist_ok=True
)


# ============================================================
# 加载数据
# ============================================================

print("\n正在加载数据集...")

train_dataset, val_dataset, test_dataset, x_scaler, y_scaler = build_datasets()
joblib.dump(
    x_scaler,
    "checkpoints/x_scaler.pkl"
)

joblib.dump(
    y_scaler,
    "checkpoints/y_scaler.pkl"
)
# #测试能不能跑用的
# from torch.utils.data import Subset
# train_dataset = Subset(
#     train_dataset,
#     range(5000)
# )
# val_dataset = Subset(
#     val_dataset,
#     range(1000)
# 
# ============================================================
# DataLoader
# ============================================================

train_loader = DataLoader(
    train_dataset,
    batch_size=BATCH_SIZE,
    shuffle=True
)

val_loader = DataLoader(
    val_dataset,
    batch_size=BATCH_SIZE,
    shuffle=False
)


print("\nDataLoader 创建完成")

print("Train batches:", len(train_loader))
print("Validation batches:", len(val_loader))


# ============================================================
# 创建模型
# ============================================================

model = LSTMModel(
    input_size=12,
    hidden_size=128,
    num_layers=2,
    prediction_length=24,
    output_size=2,
    dropout=0.2
)

model = model.to(device)


print("\n模型：")
print(model)


# ============================================================
# Loss
# ============================================================

criterion = nn.MSELoss()


# ============================================================
# Optimizer
# ============================================================

optimizer = torch.optim.Adam(
    model.parameters(),
    lr=LEARNING_RATE
)


# ============================================================
# 训练
# ============================================================

best_val_loss = float("inf")


for epoch in range(EPOCHS):

    # --------------------------------------------------------
    # Training
    # --------------------------------------------------------

    model.train()

    train_loss = 0.0

    for batch_idx, (X, Y) in enumerate(train_loader):

        X = X.to(device)
        Y = Y.to(device)

        # 清空梯度
        optimizer.zero_grad()

        # 前向传播
        prediction = model(X)

        # 计算 Loss
        loss = criterion(
            prediction,
            Y
        )

        # 反向传播
        loss.backward()

        # 更新参数
        optimizer.step()

        train_loss += loss.item()

        # 每10000个batch打印一次
        if (batch_idx + 1) % 10000 == 0:

            print(
                f"Epoch [{epoch + 1}/{EPOCHS}] "
                f"Batch [{batch_idx + 1}/{len(train_loader)}] "
                f"Loss: {loss.item():.6f}"
            )

    train_loss /= len(train_loader)


    # --------------------------------------------------------
    # Validation
    # --------------------------------------------------------

    model.eval()

    val_loss = 0.0

    with torch.no_grad():

        for X, Y in val_loader:

            X = X.to(device)
            Y = Y.to(device)

            prediction = model(X)

            loss = criterion(
                prediction,
                Y
            )

            val_loss += loss.item()

    val_loss /= len(val_loader)


    # --------------------------------------------------------
    # 输出
    # --------------------------------------------------------

    print(
        f"\nEpoch [{epoch + 1}/{EPOCHS}] "
        f"Train Loss: {train_loss:.6f} "
        f"Val Loss: {val_loss:.6f}"
    )


    # --------------------------------------------------------
    # 保存最佳模型
    # --------------------------------------------------------

    if val_loss < best_val_loss:

        best_val_loss = val_loss

        torch.save(
            {
                "model_state_dict": model.state_dict(),
                "optimizer_state_dict": optimizer.state_dict(),
                "epoch": epoch + 1,
                "val_loss": val_loss
            },
            MODEL_SAVE_PATH
        )

        print(
            f"保存最佳模型 → {MODEL_SAVE_PATH}"
        )

print("\n训练完成！")
print("Best Validation Loss:", best_val_loss)