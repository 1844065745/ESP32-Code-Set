import time
import serial
import serial.tools.list_ports


port_name = 'COM8' 
baudrate  = 115200

def is_serial_port_exist(port_name):
    ports = serial.tools.list_ports.comports()
    for port, desc, hwid in sorted(ports):
        if port_name in port:
            return True
    return False

if __name__ == "__main__":
    while True:
        while True:
            if is_serial_port_exist(port_name):
                try:
                    ser = serial.Serial(port_name, baudrate)    # 打开COM，将波特率配置为115200，其余参数使用默认值
                    if ser.isOpen():                       # 判断串口是否成功打开
                        print("打开串口成功！")
                        print(ser.name)    # 输出串口号
                        break
                    else:
                        print("打开串口失败。")
                        print("Retry in 1 seconds.")
                except:
                    print("串口异常。")
                    print("Retry in 1 seconds.")
            else:
                print(f"The serial port {port_name} does not exist.")
                print("Retry in 1 seconds.")
            time.sleep(1)

        while True:
            try:
                com_input = ser.read(1) # 读取串口输入信息并输出。
                if com_input:   # 如果读取结果非空，则输出
                    if com_input == b'\x31':
                        print("控制参数：", com_input)
                    elif com_input == b'\x32':
                        print("控制参数：", com_input)
                    elif com_input == b'\x33':
                        print("控制参数：", com_input)
                    elif com_input == b'\x34':
                        print("控制参数：", com_input)
                    else:
                        print("非控制参数：", com_input)
            except:
                break






    
