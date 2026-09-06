import torch
import torch.nn as nn

class LSTMModel(nn.Module):

    def __init__(
        self,
        input_size=12,
        hidden_size=128,
        num_layers=2,
        prediction_length=24,
        output_size=2,
        dropout=0.2
    ):
        super().__init__()

        self.hidden_size = hidden_size
        self.num_layers = num_layers
        self.prediction_length = prediction_length
        self.output_size = output_size

        # ==============================
        # LSTM
        # ==============================

        self.lstm = nn.LSTM(
            input_size=input_size,
            hidden_size=hidden_size,
            num_layers=num_layers,
            batch_first=True,
            dropout=dropout if num_layers > 1 else 0
        )

        # ==============================
        # 全连接层
        # ==============================

        self.fc = nn.Linear(
            hidden_size,
            prediction_length * output_size
        )

    def forward(self, x):

        # x:
        # [batch_size, 24, 12]
        # LSTM 输出
        lstm_out, (hidden, cell) = self.lstm(x)
        # hidden:
        # [num_layers, batch_size, hidden_size]
        # 取最后一层 LSTM 的 hidden state
        last_hidden = hidden[-1]

        # last_hidden:
        # [batch_size, hidden_size]
        # 全连接层
        out = self.fc(last_hidden)

        # out:
        # [batch_size, 24 * 2]

        # 变成未来24小时 × 2个预测目标
        out = out.view(
            x.size(0),
            self.prediction_length,
            self.output_size
        )

        # [batch_size, 24, 2]

        return out

# ============================================================
# 测试模型
# ============================================================

if __name__ == "__main__":

    # 模拟一个 batch
    x = torch.randn(
        32,
        24,
        12
    )

    model = LSTMModel()

    y = model(x)

    print("输入 shape:", x.shape)
    print("输出 shape:", y.shape)

    print("\n模型结构：")
    print(model)

    # 参数量
    total_params = sum(
        p.numel()
        for p in model.parameters()
    )

    print("\n模型参数量:", total_params)