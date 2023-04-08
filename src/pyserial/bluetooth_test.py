import serial;

serialPort = serial.Serial(port='COM9', baudrate=9600);
size = 7

while 1: 
    data = serialPort.readline(size)

    if data:
        print(data)