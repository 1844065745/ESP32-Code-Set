import serial
import numpy as np

THRESHOLD = 0.05       # 斜率阈值
WINDOW_SIZE = 5         # 数据窗口大小，假设采样频率为10Hz，0.5秒对应5个数据点
DELAY = 50              # 动作识别后的延迟计数，假设采样频率为10Hz，延迟5秒对应50个数据点
DATA_LENGTH = 15        # 串口数据固定长度（每个电感值5位，共3个电感值）

# 计算给定数据窗口内线性拟合的斜率
def calculate_slope(y):
    x = np.arange(len(y))
    slope, _ = np.polyfit(x, y, 1)
    return slope

# 根据斜率判断状态：1表示增大，-1表示减小，0表示不变
def get_state(slope):
    if slope > THRESHOLD:
        return 1
    elif slope < -THRESHOLD:
        return -1
    else:
        return 0

class ActionRecognizer:
    def __init__(self):
        self.L0_window = []
        self.L1_window = []
        self.L2_window = []
        self.delay_counter = 0

    def identify_action(self, L0_curr, L1_curr, L2_curr):
        self.L0_window.append(L0_curr)
        self.L1_window.append(L1_curr)
        self.L2_window.append(L2_curr)

        if len(self.L0_window) > WINDOW_SIZE:
            self.L0_window.pop(0)
            self.L1_window.pop(0)
            self.L2_window.pop(0)

        if len(self.L0_window) < WINDOW_SIZE:
            return 0

        if self.delay_counter > 0:
            self.delay_counter -= 1
            return 0

        slope_L0 = calculate_slope(self.L0_window)
        slope_L1 = calculate_slope(self.L1_window)
        slope_L2 = calculate_slope(self.L2_window)

        state_L0 = get_state(slope_L0)
        state_L1 = get_state(slope_L1)
        state_L2 = get_state(slope_L2)

        action_code = 0

        print(f"状态：{state_L0}, {state_L1}, {state_L2}")

        if state_L0 == 1 and state_L1 == -1 and state_L2 == 0:
            action_code = 11  # 前进动作
        elif state_L0 == -1 and state_L1 == 1 and state_L2 == 0:
            action_code = 22  # 后退动作
        elif state_L0 == 0 and state_L1 == 1 and state_L2 == -1:
            action_code = 44  # 右移动作
        elif state_L0 == 0 and state_L1 == -1 and state_L2 == 1:
            action_code = 33  # 左移动作

        if action_code != 0:
            print(f"动作号码：{action_code}")
            self.delay_counter = DELAY

        return action_code

# 主程序
if __name__ == "__main__":
    ser = serial.Serial('COM9', 9600, timeout=1)
    recognizer = ActionRecognizer()  # 在循环外创建识别器对象

    print("开始读取串口COM10数据")

    while True:
        if ser.in_waiting > 0:  # 检查串口缓冲区是否有数据
            data = ser.readline().decode('utf-8').strip()  # 读取一行数据并解码为字符串
            #print("接收到的原始数据:", data)

            # 按逗号分隔数据
            sensor_data = data.split(',')[0:3]
            #print("接收到的原始数据2:", sensor_data)

            # 检查数据是否完整
            if len(sensor_data) == 3:
                try:
                    sensor1 = float(sensor_data[0])  # 将第一个传感器数据转换为浮点数
                    sensor2 = float(sensor_data[1])  # 将第二个传感器数据转换为浮点数
                    sensor3 = float(sensor_data[2])  # 将第三个传感器数据转换为浮点数

                    # print("传感器1数据:", sensor1)
                    # print("传感器2数据:", sensor2)
                    # print("传感器3数据:", sensor3)

                    # 调用识别器处理传感器数据+6
                    recognizer.identify_action(sensor1, sensor2, sensor3)

                except ValueError:
                    print("数据格式错误，无法转换为浮点数")
            else:
                print("数据格式错误，数据不完整")
