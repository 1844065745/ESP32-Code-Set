import socket
import pandas as pd
import threading
import time
import keyboard
from queue import Full, Queue

recv_queue = Queue(maxsize=500)
data_frame = []
columns = ['id'] + [f'L{i}' for i in range(1, 17)] + ['ping']

def get_host_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(('8.8.8.8', 80))
    ip = s.getsockname()[0]
    s.close()
    return ip

def handle_client_receive_only(conn_file):
    """接收线程，仅读 socket 行，非阻塞写入有限长度队列"""
    while True:
        try:
            line = conn_file.readline()
            if not line:
                print("[RECV] Client disconnected.")
                break
            now = time.perf_counter()
            try:
                recv_queue.put_nowait((line, now))  # 非阻塞
            except Full:
                print("[QUEUE FULL] Incoming data dropped.")
        except Exception as e:
            print("[RECV ERROR]", e)
            break

def process_data_loop():
    """主线程处理逻辑：从队列中取出数据，解析、校验、保存等"""
    packet_id_last = -1
    last_time = time.perf_counter()

    while True:
        try:
            line, recv_time = recv_queue.get(timeout=0.1)
        except:
            # timeout，为了让键盘检测及时执行
            line, recv_time = None, None

        if line:
            line = line.strip()
            delta_time = round(1000 * (recv_time - last_time), 3)
            last_time = recv_time

            if not line.startswith("Ldata,"):
                continue

            print("[RECV]", line, "| Delta time:", delta_time)

            parts = line[6:].split(",")
            if len(parts) != 17:
                print("[WARN] Invalid data length:", len(parts))
                continue

            try:
                packet_id = int(parts[0])
                if packet_id_last != -1 and packet_id != packet_id_last + 1:
                    print(f"[MISS] Missed {packet_id - packet_id_last} packet(s)")
                packet_id_last = packet_id

                values = [round(float(p), 3) for p in parts]
                if any(v > 100 or v <= 1 for v in values[1:]):
                    print("[WARN] Sensor out of range")
                    continue

                row = dict(zip(columns, values))
                row['ping'] = delta_time
                data_frame.append(row)

            except ValueError:
                print("[ERROR] Invalid float format")

        # 键盘处理逻辑
        if keyboard.is_pressed('esc'):
            df = pd.DataFrame(data_frame)
            fname = time.strftime("%Y-%m-%d_%H-%M-%S") + "_data.csv"
            df.to_csv(fname, index=False)
            print(f"[SAVE] Data saved to: {fname}")
            break
        if keyboard.is_pressed('q'):
            print("[EXIT] Quit without saving.")
            break

def main():
    ip = get_host_ip()
    port = 10000
    print(f"[SERVER] Listening on {ip}:{port}")

    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((ip, port))
    server.listen(1)

    conn, addr = server.accept()
    print(f"[TCP] Connected from {addr}")
    conn_file = conn.makefile('r')

    # 启动接收线程
    threading.Thread(target=handle_client_receive_only, args=(conn_file,), daemon=True).start()

    # 主线程处理队列数据
    process_data_loop()

    conn.close()
    server.close()
    print("[CLOSED] Server closed.")

if __name__ == "__main__":
    main()
