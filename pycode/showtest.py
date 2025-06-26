import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from collections import deque

# 配置参数
CHANNELS = 8               # 通道数
BUFFER_SIZE = 200          # 每个通道显示的数据点数
SAMPLE_RATE = 100          # 采样率（Hz）
UPDATE_INTERVAL = 50       # 刷新间隔（ms）

class RealtimePlot:
    def __init__(self):
        # 初始化图形和子图布局
        self.fig, self.axes = plt.subplots(2, 4, figsize=(16, 8))
        self.axes = self.axes.flatten()  # 转换为1D数组方便索引
        
        # 初始化数据缓冲区（使用deque实现环形缓冲区）
        self.data = [deque(maxlen=BUFFER_SIZE) for _ in range(CHANNELS)]
        self.t = deque(maxlen=BUFFER_SIZE)
        
        # 初始化曲线对象
        self.lines = []
        for i, ax in enumerate(self.axes):
            line, = ax.plot([], [], lw=1.5)
            ax.set_title(f'Channel {i+1}')
            ax.set_ylim(-5, 5)  # 初始Y轴范围
            ax.grid(True)
            self.lines.append(line)
        
        # 调整布局
        self.fig.tight_layout()
        
        # 计算时间轴
        self.base_time = np.arange(-BUFFER_SIZE/SAMPLE_RATE, 0, 1/SAMPLE_RATE)
        
    def update_plot(self, frame):
        """动画更新函数"""
        # 生成模拟数据（替换为真实数据源）
        new_samples = np.random.randn(1, CHANNELS) * 2 + np.sin(0.1 * frame)
        
        # 更新数据缓冲区
        self.t.extend(self.base_time + frame/SAMPLE_RATE)
        for ch in range(CHANNELS):
            self.data[ch].append(new_samples[0, ch])
        
        # 更新曲线数据
        for ch, line in enumerate(self.lines):
            line.set_data(self.t, self.data[ch])
            self.axes[ch].relim()        # 自动调整Y轴范围
            self.axes[ch].autoscale_view()
        
        return self.lines
    
    def run(self):
        """启动动画"""
        ani = FuncAnimation(
            self.fig, 
            self.update_plot, 
            interval=UPDATE_INTERVAL, 
            blit=True,   # 使用blit优化性能
            cache_frame_data=False
        )
        plt.show()

if __name__ == '__main__':
    plot = RealtimePlot()
    plot.run()