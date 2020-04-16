import serial, time, csv
with open('Anar.csv', 'r') as file:
    reader = csv.reader(file)
    for row in reader:
        print(row[0])
        print ("Basladim")

    

arduino = serial.Serial('COM3', baudrate = 115200, timeout =None)
time.sleep(1)

bufferSize = 20 
ramSize = 200
i=0

#make correction to the end of file
#length = len(row)-1
#while row[length] != 253:
#   length -= 1
#   row[length] = 253
print (len(row)-1)
for x in range(0, 2*bufferSize, 1):
    row.append("253")
print (len(row)-1)


while True:
    if arduino.inWaiting() > 0:
        data = arduino.read().decode("utf-8")
        if data == 'r':
            print ("Ri aldim")
            for x in range(0, bufferSize, 2):
                result = int(row[i])
                result1 = ((result & 0xFF00) >> 8)
                result2 = (result & 0xFF)
                i = i+1
                arduino.write(bytes([result1, result2]))
                #data = arduino.readline()
            if row[i-1]=="253":
                time.sleep(1)
                exit()

        if data == 'p':
            print ("Pi verdiler")
            if arduino.inWaiting() > 0:
                data = arduino.readline()
                print (data)
                #time.sleep(1)
