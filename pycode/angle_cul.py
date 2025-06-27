import socket
import pandas as pd
from datetime import datetime
import keyboard
import threading
import time

data_inductor = {}
data_IMU = {}
conn_que = []

stop_broadcast = False  # 控制UDP广播的标志

# 获取本机 IP
def get_host_ip():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(('8.8.8.8', 80))
        ip = s.getsockname()[0]
    finally:
        s.close()
        print("My IP is: " + ip)
    return ip

# UDP 广播函数
def udp_broadcast(ip, port):
    udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    udp.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    message = f"{ip}:{port}".encode()

    while not stop_broadcast:
        udp.sendto(message, ('<broadcast>', 8888))  # ESP32 监听 8888 端口
        print("[UDP] Broadcasting IP:", message.decode())
        time.sleep(0.2)  # 每 0.5 秒广播一次

    udp.close()
    print("[UDP] Broadcast stopped.")

def main():
    global stop_broadcast

    # 初始化 Excel 文件
    start_time = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    excel_file_name = f"{start_time}_data.xlsx"
    print(f"Excel file name: {excel_file_name}")
    columns = ['L1', 'L2', 'L3', 'L4']
    data_frame = pd.DataFrame(columns=columns)
    data_frame = data_frame._append({'L1': 'L1', 'L2': 'L2', 'L3': 'L3', 'L4': 'L4'}, ignore_index=True)

    ip = get_host_ip()
    port = 10000

    # 启动 UDP 广播线程
    threading.Thread(target=udp_broadcast, args=(ip, port), daemon=True).start()

    # 启动 TCP 服务器
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((ip, port))
    server.listen(1)
    print(f"[TCP] Listening at {ip}:{port}...")

    try:
        conn, addr = server.accept()
        print(f"[TCP] Connected from {addr}")
        stop_broadcast = True  # 停止 UDP 广播

        conn_file = conn.makefile('r')  # 支持 readline()

        # 初始化时间戳变量
        last_recv_time = time.perf_counter()

        while True:
            line = conn_file.readline()
            if not line:
                print("[TCP] Client disconnected.")
                break

            current_time = time.perf_counter()
            delta_time = round(1000*(current_time-last_recv_time), 3)
            last_recv_time = current_time

            line = line.strip()
            # print("Received:", line, "Delta time:", delta_time)

            if line.startswith("Ldata,"):
                parts = line[6:].split(",")
                parts = parts[1:5]  # 只保留 L1, L2, L3, L4 的数据
                # print("Received Ldata:", parts, "Delta time:", delta_time)
                if len(parts) == 4:
                    try:
                        values = [round(float(p), 3) for p in parts]
                        if any(v > 200 or v <= 1 for v in values):
                            print("Invalid inductor data")
                            continue
                        new_row = dict(zip(columns, values))
                        data_frame = data_frame._append(new_row, ignore_index=True)
                        print("Parsed inductor:", new_row, " | Delta time:", delta_time, "ms")
                    except ValueError:
                        print("Invalid float format")
                        continue
                else:
                    print("Unexpected data format")

            if keyboard.is_pressed('esc'):
                data_frame.to_excel(excel_file_name, index=False, header=False, engine='openpyxl')
                print(f"Saved data to: {excel_file_name}")
                break

    except Exception as e:
        print("[ERROR]", e)
    finally:
        conn.close()
        server.close()
        print("[TCP] Server closed")

if __name__ == "__main__":
    main()
