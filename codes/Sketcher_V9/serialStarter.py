import serial, time
arduino = serial.Serial('/dev/ttyACM0', baudrate = 115200, timeout =None)
time.sleep(1)
while True:
	time.sleep(10)
