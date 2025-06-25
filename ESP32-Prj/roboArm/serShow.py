import serial
import serial.tools.list_ports
 
# 获取所有串口设备实例。
# 如果没找到串口设备，则输出：“无串口设备。”
# 如果找到串口设备，则依次输出每个设备对应的串口号和描述信息。
ports_list = list(serial.tools.list_ports.comports())
if len(ports_list) <= 0:
    print("无串口设备。")
else:
    print("可用的串口设备如下：")
    for comport in ports_list:
        print(list(comport)[0], list(comport)[1])


port_name ='COM8' 

def is_serial_port_exist(port_name):
    ports = serial.tools.list_ports.comports()
    for port, desc, hwid in sorted(ports):
        if port_name in port:
            return True
    return False


if is_serial_port_exist(port_name):
    print(f"The serial port {port_name} exists.")
else:
    print(f"The serial port {port_name} does not exist.")

