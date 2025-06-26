import socket
import struct
import pandas as pd
from datetime import datetime
import keyboard  # 导入keyboard库


data_inductor = {}
data_IMU = {}

conn_que = [] #客户端套接字队列




#查询本机地址函数
def get_host_ip():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(('8.8.8.8', 80))
        ip = s.getsockname()[0]
    finally:
        s.close()
        print("My IP is: " + ip)
    return ip


def main():
    # 获取程序开始时间
    start_time = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    excel_file_name = f"{start_time}_data.xlsx"
    print(f"Excel file name: {excel_file_name}")
    # 创建一个空的DataFrame用于保存数据
    columns = ['L1', 'L2', 'L3', 'L4', 'L5', 'L6', 'L7', 'L8']  # 替换为实际列名
    data_frame = pd.DataFrame(columns=columns)
    # 创建一个空的DataFrame用于保存数据
    columns = ['L1', 'L2', 'L3', 'L4', 'L5', 'L6', 'L7', 'L8']  # 替换为实际列名
    new_row = {'L1': 'L1', 'L2': 'L2', 'L3': 'L3', 'L4': 'L4',
               'L5': 'L5', 'L6': 'L6', 'L7': 'L7', 'L8': 'L8'}
    data_frame = data_frame._append(new_row, ignore_index=True)

    # 网络连接初始化
    ip  = get_host_ip()
    port = 10000
    server = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    try:
        server.bind((ip, port))
        server.listen(1)
        connection1, address1 = server.accept()
        print("Connection from ", address1)
    except socket.error as e:
        print("Failed connecting...")


    flag_ifctnu = 0

    while True:
        rx = connection1.recv(61)
        # print(rx,len(rx))
        if rx[:1] == b'\xaa':
            try:
                for i in range(8):
                    data_inductor[i] = round(struct.unpack('f', rx[4*i+1 : 4*i+5])[0] , 3)
                    if data_inductor[i]>100 or data_inductor[i]<=1:
                        flag_ifctnu=1
                        break
                    else:
                        flag_ifctnu=0
                if flag_ifctnu == 1:
                    print("Inductor data error")
                    continue
                # 将数据添加到DataFrame
                new_row = {'L1': data_inductor[0], 'L2': data_inductor[1], 'L3': data_inductor[2], 'L4': data_inductor[3],
                           'L5': data_inductor[4], 'L6': data_inductor[5], 'L7': data_inductor[6], 'L8': data_inductor[7]}
                data_frame = data_frame._append(new_row, ignore_index=True)
                print('Inductor:', data_inductor) 
            except:
                print("Error unpacking inductor data")
                pass
        
        # 检测是否按下 "Esc" 键，如果是则保存并退出程序
        if keyboard.is_pressed('esc'):
            data_frame.to_excel(excel_file_name, index=False, header=False, engine='openpyxl')
            print(f"Data appended to Excel: {excel_file_name}")
            break


if __name__ =="__main__":
    main()

