import io
import picamera
import cv2
import numpy
import time
import array
#import request
import urllib
import base64
import json
from xml.etree.ElementTree import Element, SubElement, Comment, tostring
import xml.etree.ElementTree
import hmac,hashlib
import requests
from numpy import asarray

#########################################################################
#Face Detection
#########################################################################

#Create a memory stream so photos doesn't need to be saved in a file
stream = io.BytesIO()

#Get the picture (low resolution, so it should be quite fast)
#Here you can also specify other parameters (e.g.:rotate the image)
with picamera.PiCamera() as camera:
    camera.resolution = (3280, 2464)
    camera.capture(stream, format='jpeg', quality=100)
    camera.close()
    print ('CAPTURED')

#Convert the picture into a numpy array
buff = numpy.frombuffer(stream.getvalue(), dtype=numpy.uint8)

#Now creates an OpenCV image
image = cv2.imdecode(buff, 1)
image = cv2.rotate(image, cv2.ROTATE_90_CLOCKWISE)

#Load a cascade file for detecting faces
haar_face_cascade = cv2.CascadeClassifier('/home/pi/Desktop/haarcascade.xml') #haar_cascade
lbp_face_cascade = cv2.CascadeClassifier('/home/pi/Desktop/lbpcascade.xml') #lbp_cascade

#Convert to grayscale
image_gray = cv2.cvtColor(image,cv2.COLOR_BGR2GRAY)

#Look for faces in the image using the loaded cascade file
faces_haarcascade = haar_face_cascade.detectMultiScale(image_gray, 1.1, 5)
faces_lbpcascade = lbp_face_cascade.detectMultiScale(image_gray, 1.1, 5)

mutual_faces = 0
biggest_haar_coordinates = [-1]*6
biggest_lbp_coordinates = [-1]*6
biggest_mutual_coordinates = [-1]*6
crop_coordinates = [-1]*6
haar_max = 0
lbp_max = 0
mutual_max = 0


print ('Haar Cascade has found ' + str(len(faces_haarcascade)) + ' face(s)')
print ('LBP Cascade has found ' + str(len(faces_lbpcascade)) + ' face(s)')

#Draw a rectangle around every found face
for (x,y,w,h) in faces_haarcascade:
	if w*h > haar_max:
		haar_max = w*h
		biggest_haar_coordinates = [x,y,x+w,y+h,w,h]
	#print(x,y,w,h)
	#cv2.rectangle(image,(x,y),(x+w,y+h),(255,0,0),2)
    #cv2.rectangle(image, (x,y-10), (x+w,y+h+10), (0,255,0), 2) # for wider rectangles
    
for (x,y,w,h) in faces_lbpcascade:
	if w*h > lbp_max:
		lbp_max = w*h
		biggest_lbp_coordinates = [x,y,x+w,y+h,w,h]
	#print(x,y,w,h)
	#cv2.rectangle(image,(x,y),(x+w,y+h),(0,255,0),2)
	#cv2.rectangle(image, (x,y-10), (x+w,y+h+10), (0,255,0), 2) # for wider rectangles	
    
for (x1,y1,w1,h1) in faces_haarcascade:
	for (x2, y2, w2, h2) in faces_lbpcascade:
		if x1 < x2 and y1 < y2 and (x1+w1) > (x2+w2) and (y1+h1) > (y2+h2):
			if w1*h1 > mutual_max:
				mutual_max = w1*h1
				biggest_mutual_coordinates = [x1,y1,x1+w1,y1+h1,w1,h1]
			#x3 = round(((x1+x2)/2).item())
			#y3 = round(((y1+y2)/2).item())
			#w3 = round(((w1+w2)/2).item())
			#h3 = round(((h1+h2)/2).item())
			#print(x3,y3,w3,h3)
			#cv2.rectangle(image,(x3,y3),(x3+w3,y3+h3),(0,0,255),2)
			mutual_faces += 1
			
print (str(mutual_faces) + ' mutual face(s) have been found')

if mutual_faces == 0:
	quit()

#Save the result image
seconds = time.time()
local_time = time.ctime(seconds)
cv2.imwrite('../images/orig' + '.jpg',image)

xComRatio = (0.0/96)
yComRatio = (22.0/96)

if biggest_mutual_coordinates[0] != -1:
	xAdd = int (round (xComRatio * (biggest_mutual_coordinates[2] - biggest_mutual_coordinates[0])))
	yAdd = int (round (yComRatio * (biggest_mutual_coordinates[3] - biggest_mutual_coordinates[1])))
	cropped_image = image[(biggest_mutual_coordinates[1]-yAdd):(biggest_mutual_coordinates[3]+yAdd),(biggest_mutual_coordinates[0]-xAdd):(biggest_mutual_coordinates[2]+xAdd)]
	crop_coordinates = biggest_mutual_coordinates.copy()
elif biggest_haar_coordinates[0] != -1:
	if biggest_lbp_coordinates[0] != -1 and biggest_lbp_coordinates[5]*biggest_lbp_coordinates[6] > biggest_haar_coordinates[5]*biggest_haar_coordinates[6]:
		xAdd = int (round (xComRatio * (biggest_lbp_coordinates[2] - biggest_lbp_coordinates[0])))
		yAdd = int (round (yComRatio * (biggest_lbp_coordinates[3] - biggest_lbp_coordinates[1])))
		cropped_image = image[(biggest_lbp_coordinates[1]-200):(biggest_lbp_coordinates[3]+200),(biggest_lbp_coordinates[0]-150):(biggest_lbp_coordinates[2]+150)]
		crop_coordinates = biggest_lbp_coordinates.copy()
	else:
		xAdd = int (round (xComRatio * (biggest_haar_coordinates[2] - biggest_haar_coordinates[0])))
		yAdd = int (round (yComRatio * (biggest_haar_coordinates[3] - biggest_haar_coordinates[1])))	
		cropped_image = image[(biggest_haar_coordinates[1]-200):(biggest_haar_coordinates[3]+200),(biggest_haar_coordinates[0]-150):(biggest_haar_coordinates[2]+150)]
		crop_coordinates = biggest_haar_coordinates.copy()
else:
	xAdd = int (round (xComRatio * (biggest_lbp_coordinates[2] - biggest_lbp_coordinates[0])))
	yAdd = int (round (yComRatio * (biggest_lbp_coordinates[3] - biggest_lbp_coordinates[1])))	
	cropped_image = image[(biggest_lbp_coordinates[1]-200):(biggest_lbp_coordinates[3]+200),(biggest_lbp_coordinates[0]-150):(biggest_lbp_coordinates[2]+150)]
	crop_coordinates = biggest_lbp_coordinates.copy()

#####################################################################################

cv2.imwrite('../images/crop' + '.jpg', cropped_image)

#image_filtered = cv2.bilateralFilter(image,5,30,30)
#image_gray_filtered = cv2.bilateralFilter(image_gray,7,50,50)
#cropped_image_filtered = cv2.bilateralFilter(cropped_image,60,10,10)

grayscale = cv2.cvtColor(cropped_image,cv2.COLOR_BGR2GRAY)


#cropped_image_gray = cv2.cvtColor(cropped_image, cv2.COLOR_BGR2GRAY)
#cropped_image_gray_filtered = cv2.bilateralFilter(cropped_image_gray,7,50,50)


scale_percent = 17 # percent of original size
width = int(grayscale.shape[1] * scale_percent / 100)
height = int(grayscale.shape[0] * scale_percent / 100)
dim = (width, height)
scaled_down = cv2.resize(grayscale, dim, interpolation = cv2.INTER_AREA)

filtered = cv2.GaussianBlur(scaled_down,(5,5),0)
cv2.imwrite('../images/filter' + '.jpg', filtered)

edge = cv2.Canny(filtered,2250,2250,apertureSize=7,L2gradient=True)
ret,inverted = cv2.threshold(edge,220,255,cv2.THRESH_BINARY_INV)
#cropped_edges_50_100 = cv2.medianBlur(cropped_edges_50_100,5)
#cropped_edges_50_100 = ~cropped_edges_50_100
#cropped_edges_50_100_inverted = cv2.adaptiveThreshold(cropped_edges_50_100,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,cv2.THRESH_BINARY,11,2)


width = 720
height = 1050
dim = (width, height)
scaled_up = cv2.resize(inverted, dim, interpolation = cv2.INTER_AREA)
cv2.imwrite('../images/edge' + '.jpg', scaled_up)


data = asarray(scaled_up)
numpy.savetxt("../images/edge.csv", data, delimiter=",", fmt='%d' )
'''
# another implementation
def auto_canny(image,sigma):
	v = numpy.median(image)
	lower = int(max(0,(1.0-sigma)*v))
	upper = int(min(255, (1.0 + sigma)*v))
	edged = cv2.Canny(image,lower,upper)
	return edged
	
blurred = cv2.GaussianBlur(cropped_image_gray, (3,3),0)
cv2.imwrite('blurred.jpg',blurred)

for i in [float(j) / 100 for j in range(0,100,1)]:
	auto = auto_canny(blurred,i)
	ret,auto_inverted = cv2.threshold(auto,127,255,cv2.THRESH_BINARY_INV)
	cv2.imwrite('auto_' + str(i)+'.jpg',auto_inverted)
'''

