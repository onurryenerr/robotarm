import io
import picamera
import cv2
import numpy
import time
import array
import urllib.request
import urllib
import base64
import json
from xml.etree.ElementTree import Element, SubElement, Comment, tostring
import xml.etree.ElementTree
import hmac,hashlib
import requests

#########################################################################
#Face Detection
#########################################################################

#Create a memory stream so photos doesn't need to be saved in a file
stream = io.BytesIO()

#Get the picture (low resolution, so it should be quite fast)
#Here you can also specify other parameters (e.g.:rotate the image)
with picamera.PiCamera() as camera:
    camera.resolution = (3280, 2464)
    camera.capture(stream, format='jpeg')

#Convert the picture into a numpy array
buff = numpy.fromstring(stream.getvalue(), dtype=numpy.uint8)

#Now creates an OpenCV image
image = cv2.imdecode(buff, 1)

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

#Save the result image
seconds = time.time()
local_time = time.ctime(seconds)
#cv2.imwrite('result_' + str(local_time) + '.jpg',image)

if biggest_mutual_coordinates[0] != -1:
	cropped_image = image[(biggest_mutual_coordinates[1]-200):(biggest_mutual_coordinates[3]+200),(biggest_mutual_coordinates[0]-150):(biggest_mutual_coordinates[2]+150)]
	crop_coordinates = biggest_mutual_coordinates.copy()
elif biggest_haar_coordinates[0] != -1:
	if biggest_lbp_coordinates[0] != -1 and biggest_lbp_coordinates[5]*biggest_lbp_coordinates[6] > biggest_haar_coordinates[5]*biggest_haar_coordinates[6]:
		cropped_image = image[(biggest_lbp_coordinates[1]-200):(biggest_lbp_coordinates[3]+200),(biggest_lbp_coordinates[0]-150):(biggest_lbp_coordinates[2]+150)]
		crop_coordinates = biggest_lbp_coordinates.copy()
	else:	
		cropped_image = image[(biggest_haar_coordinates[1]-200):(biggest_haar_coordinates[3]+200),(biggest_haar_coordinates[0]-150):(biggest_haar_coordinates[2]+150)]
		crop_coordinates = biggest_haar_coordinates.copy()
else:
	cropped_image = image[(biggest_lbp_coordinates[1]-200):(biggest_lbp_coordinates[3]+200),(biggest_lbp_coordinates[0]-150):(biggest_lbp_coordinates[2]+150)]
	crop_coordinates = biggest_lbp_coordinates.copy()

#####################################################################################

image_filtered = cv2.bilateralFilter(image,7,50,50)
image_gray_filtered = cv2.bilateralFilter(image_gray,7,50,50)
cropped_image_filtered = cv2.bilateralFilter(cropped_image,7,50,50)
cropped_image_gray = cv2.cvtColor(cropped_image, cv2.COLOR_BGR2GRAY)
cropped_image_gray_filtered = cv2.bilateralFilter(cropped_image_gray,7,50,50)


#####################################################################################
#Upload to imgur
#######################################################################################

cv2.imwrite('image_' + str(local_time) + '.jpg', image)
cv2.imwrite('cropped_image_' + str(local_time) + '.jpg', cropped_image)

f = open('image_' + str(local_time) + '.jpg', 'rb')
imgur_image_data = f.read()
f = open('cropped_image_' + str(local_time) + '.jpg', 'rb')
imgur_cropped_image_data = f.read()

b64_image = base64.standard_b64encode(imgur_image_data)
b64_cropped_image = base64.standard_b64encode(imgur_cropped_image_data)

client_id = "c3a549405fec2d3"
headers = {'Authorization': 'Client-ID ' + client_id}

d_image = {'image': b64_image, 'title': 'test'}
d_cropped_image = {'image': b64_cropped_image, 'title': 'test'}

data_image = urllib.parse.urlencode(d_image).encode("utf-8")
data_cropped_image = urllib.parse.urlencode(d_cropped_image).encode("utf-8")

request = urllib.request.Request(url="https://api.imgur.com/3/upload.json", headers=headers)

with urllib.request.urlopen(request,data=data_image) as f:
	response = f.read()
	#print(response)
	parse = json.loads(response)
	image_Url = parse['data']['link']
	
with urllib.request.urlopen(request,data=data_cropped_image) as f:
	response = f.read()
	#print(response)
	parse = json.loads(response)
	cropped_image_Url = parse['data']['link']

########################################################################################


#######################################################################################
#Pho.to
######################################################################################
key = b'52af485a19cc3e3a88b07104d1e22076'
app_id = '8d0c7b37460760e55a4b020013820883'
api_endpoint = "http://opeapi.ws.pho.to/addtask"

image_process_call = Element('image_process_call')
image_url = SubElement(image_process_call,'image_url')

methods_list = SubElement(image_process_call,'methods_list')
method = SubElement(methods_list,'method')
name = SubElement(method,'name')
name.text = 'edge_detection'
params = SubElement(method,'params')
params.text = 'use_blur=false;strength=1'

image_url.text = image_Url
image_data = tostring(image_process_call)

image_url.text = cropped_image_Url
cropped_image_data = tostring(image_process_call)

sign_image_data = hmac.new(key, image_data, hashlib.sha1).hexdigest()
sign_cropped_image_data = hmac.new(key, cropped_image_data, hashlib.sha1).hexdigest()


image_data = {'app_id': app_id,
		'sign_data': sign_image_data,
		'data': image_data}

cropped_image_data = {'app_id': app_id,
		'sign_data': sign_cropped_image_data,
		'data': cropped_image_data}

image_request = requests.post(url=api_endpoint,data=image_data)
image_response = image_request.text

cropped_image_request = requests.post(url=api_endpoint,data=cropped_image_data)
cropped_image_response = cropped_image_request.text

#print(photo_response)
resp = xml.etree.ElementTree.fromstring(image_response)
image_request_id = resp[0].text

resp = xml.etree.ElementTree.fromstring(cropped_image_response)
cropped_image_request_id = resp[0].text

#print(resp[0].text)

getRequestUrl = 'http://opeapi.ws.pho.to/getresult'

image_PARAMS = {'request_id':image_request_id}
cropped_image_PARAMS = {'request_id':cropped_image_request_id}

time.sleep(30)

get_image_Request = requests.get(url=getRequestUrl, params = image_PARAMS)
get_image_Response = get_image_Request.text

get_cropped_image_Request = requests.get(url=getRequestUrl, params = cropped_image_PARAMS)
get_cropped_image_Response = get_cropped_image_Request.text

resp = xml.etree.ElementTree.fromstring(get_image_Response)
pho_to_image_url = resp[2].text

resp = xml.etree.ElementTree.fromstring(get_cropped_image_Response)
pho_to_cropped_image_url = resp[2].text

with open('pho_to_image_result_' + str(local_time) + '.jpg','wb') as handle:
	response = requests.get(pho_to_image_url, stream = True)
	
	if not response.ok:
		print (response)
		
	for block in response.iter_content(1024):
		if not block:
			break
			
		handle.write(block)
		
with open('pho_to_cropped_image_result_' + str(local_time) + '.jpg','wb') as handle:
	response = requests.get(pho_to_cropped_image_url, stream = True)
	
	if not response.ok:
		print (response)
		
	for block in response.iter_content(1024):
		if not block:
			break
			
		handle.write(block)

pho_to_image = cv2.imread('pho_to_image_result_' + str(local_time) + '.jpg')
pho_to_image_cropped = pho_to_image[(crop_coordinates[1]-200):(crop_coordinates[3]+200),(crop_coordinates[0]-150):(crop_coordinates[2]+150)]
cv2.imwrite('pho_to_image_cropped_result_' + str(local_time) + '.jpg',pho_to_image_cropped)

#######################################################################################


'''
cropped_edges_20_30 = cv2.Canny(cropped_image,20,30)
ret,cropped_edges_20_30_inverted = cv2.threshold(cropped_edges_20_30,127,255,cv2.THRESH_BINARY_INV)

cropped_edges_30_70 = cv2.Canny(cropped_image,30,70)
ret,cropped_edges_30_70_inverted = cv2.threshold(cropped_edges_30_70,127,255,cv2.THRESH_BINARY_INV)

cropped_edges_40_80 = cv2.Canny(cropped_image,40,80)
ret,cropped_edges_40_80_inverted = cv2.threshold(cropped_edges_40_80,127,255,cv2.THRESH_BINARY_INV)

cropped_edges_50_100 = cv2.Canny(cropped_image,50,100)
ret,cropped_edges_50_100_inverted = cv2.threshold(cropped_edges_50_100,127,255,cv2.THRESH_BINARY_INV)

cropped_edges_60_120 = cv2.Canny(cropped_image,60,120)
ret,cropped_edges_60_120_inverted = cv2.threshold(cropped_edges_60_120,127,255,cv2.THRESH_BINARY_INV)

cropped_edges_85_170 = cv2.Canny(cropped_image,85,170)
ret,cropped_edges_85_170_inverted = cv2.threshold(cropped_edges_85_170,127,255,cv2.THRESH_BINARY_INV)

cropped_edges_100_200 = cv2.Canny(cropped_image,100,200)
ret,cropped_edges_100_200_inverted = cv2.threshold(cropped_edges_100_200,127,255,cv2.THRESH_BINARY_INV)



cropped_filtered_edges_20_30 = cv2.Canny(cropped_image_filtered,20,30)
ret,cropped_filtered_edges_20_30_inverted = cv2.threshold(cropped_filtered_edges_20_30,127,255,cv2.THRESH_BINARY_INV)

cropped_filtered_edges_30_70 = cv2.Canny(cropped_image_filtered,30,70)
ret,cropped_filtered_edges_30_70_inverted = cv2.threshold(cropped_filtered_edges_30_70,127,255,cv2.THRESH_BINARY_INV)

cropped_filtered_edges_40_80 = cv2.Canny(cropped_image_filtered,40,80)
ret,cropped_filtered_edges_40_80_inverted = cv2.threshold(cropped_filtered_edges_40_80,127,255,cv2.THRESH_BINARY_INV)

cropped_filtered_edges_50_100 = cv2.Canny(cropped_image_filtered,50,100)
ret,cropped_filtered_edges_50_100_inverted = cv2.threshold(cropped_filtered_edges_50_100,127,255,cv2.THRESH_BINARY_INV)

cropped_filtered_edges_60_120 = cv2.Canny(cropped_image_filtered,60,120)
ret,cropped_filtered_edges_60_120_inverted = cv2.threshold(cropped_filtered_edges_60_120,127,255,cv2.THRESH_BINARY_INV)

cropped_filtered_edges_85_170 = cv2.Canny(cropped_image_filtered,85,170)
ret,cropped_filtered_edges_85_170_inverted = cv2.threshold(cropped_filtered_edges_85_170,127,255,cv2.THRESH_BINARY_INV)

cropped_filtered_edges_100_200 = cv2.Canny(cropped_image_filtered,100,200)
ret,cropped_filtered_edges_100_200_inverted = cv2.threshold(cropped_filtered_edges_100_200,127,255,cv2.THRESH_BINARY_INV)



cropped_gray_edges_20_30 = cv2.Canny(cropped_image_gray,20,30)
ret,cropped_gray_edges_20_30_inverted = cv2.threshold(cropped_gray_edges_20_30,127,255,cv2.THRESH_BINARY_INV)

cropped_gray_edges_30_70 = cv2.Canny(cropped_image_gray,30,70)
ret,cropped_gray_edges_30_70_inverted = cv2.threshold(cropped_gray_edges_30_70,127,255,cv2.THRESH_BINARY_INV)

cropped_gray_edges_40_80 = cv2.Canny(cropped_image_gray,40,80)
ret,cropped_gray_edges_40_80_inverted = cv2.threshold(cropped_gray_edges_40_80,127,255,cv2.THRESH_BINARY_INV)

cropped_gray_edges_50_100 = cv2.Canny(cropped_image_gray,50,100)
ret,cropped_gray_edges_50_100_inverted = cv2.threshold(cropped_gray_edges_50_100,127,255,cv2.THRESH_BINARY_INV)

cropped_gray_edges_60_120 = cv2.Canny(cropped_image_gray,60,120)
ret,cropped_gray_edges_60_120_inverted = cv2.threshold(cropped_gray_edges_60_120,127,255,cv2.THRESH_BINARY_INV)

cropped_gray_edges_85_170 = cv2.Canny(cropped_image_gray,85,170)
ret,cropped_gray_edges_85_170_inverted = cv2.threshold(cropped_gray_edges_85_170,127,255,cv2.THRESH_BINARY_INV)

cropped_gray_edges_100_200 = cv2.Canny(cropped_image_gray,100,200)
ret,cropped_gray_edges_100_200_inverted = cv2.threshold(cropped_gray_edges_100_200,127,255,cv2.THRESH_BINARY_INV)



cropped_gray_filtered_edges_20_30 = cv2.Canny(cropped_image_gray_filtered,20,30)
ret,cropped_gray_filtered_edges_20_30_inverted = cv2.threshold(cropped_gray_filtered_edges_20_30,127,255,cv2.THRESH_BINARY_INV)

cropped_gray_filtered_edges_30_70 = cv2.Canny(cropped_image_gray_filtered,30,70)
ret,cropped_gray_filtered_edges_30_70_inverted = cv2.threshold(cropped_gray_filtered_edges_30_70,127,255,cv2.THRESH_BINARY_INV)

cropped_gray_filtered_edges_40_80 = cv2.Canny(cropped_image_gray_filtered,40,80)
ret,cropped_gray_filtered_edges_40_80_inverted = cv2.threshold(cropped_gray_filtered_edges_40_80,127,255,cv2.THRESH_BINARY_INV)

cropped_gray_filtered_edges_50_100 = cv2.Canny(cropped_image_gray_filtered,50,100)
ret,cropped_gray_filtered_edges_50_100_inverted = cv2.threshold(cropped_gray_filtered_edges_50_100,127,255,cv2.THRESH_BINARY_INV)

cropped_gray_filtered_edges_60_120 = cv2.Canny(cropped_image_gray_filtered,60,120)
ret,cropped_gray_filtered_edges_60_120_inverted = cv2.threshold(cropped_gray_filtered_edges_60_120,127,255,cv2.THRESH_BINARY_INV)

cropped_gray_filtered_edges_85_170 = cv2.Canny(cropped_image_gray_filtered,85,170)
ret,cropped_gray_filtered_edges_85_170_inverted = cv2.threshold(cropped_gray_filtered_edges_85_170,127,255,cv2.THRESH_BINARY_INV)

cropped_gray_filtered_edges_100_200 = cv2.Canny(cropped_image_gray_filtered,100,200)
ret,cropped_gray_filtered_edges_100_200_inverted = cv2.threshold(cropped_gray_filtered_edges_100_200,127,255,cv2.THRESH_BINARY_INV)





edges_20_30 = cv2.Canny(image,20,30)
ret,edges_20_30_inverted = cv2.threshold(edges_20_30,127,255,cv2.THRESH_BINARY_INV)

edges_30_70 = cv2.Canny(image,30,70)
ret,edges_30_70_inverted = cv2.threshold(edges_30_70,127,255,cv2.THRESH_BINARY_INV)

edges_40_80 = cv2.Canny(image,40,80)
ret,edges_40_80_inverted = cv2.threshold(edges_40_80,127,255,cv2.THRESH_BINARY_INV)

edges_50_100 = cv2.Canny(image,50,100)
ret,edges_50_100_inverted = cv2.threshold(edges_50_100,127,255,cv2.THRESH_BINARY_INV)

edges_60_120 = cv2.Canny(image,60,120)
ret,edges_60_120_inverted = cv2.threshold(edges_60_120,127,255,cv2.THRESH_BINARY_INV)

edges_85_170 = cv2.Canny(image,85,170)
ret,edges_85_170_inverted = cv2.threshold(edges_85_170,127,255,cv2.THRESH_BINARY_INV)

edges_100_200 = cv2.Canny(image,100,200)
ret,edges_100_200_inverted = cv2.threshold(edges_100_200,127,255,cv2.THRESH_BINARY_INV)



filtered_edges_20_30 = cv2.Canny(image_filtered,20,30)
ret,filtered_edges_20_30_inverted = cv2.threshold(filtered_edges_20_30,127,255,cv2.THRESH_BINARY_INV)

filtered_edges_30_70 = cv2.Canny(image_filtered,30,70)
ret,filtered_edges_30_70_inverted = cv2.threshold(filtered_edges_30_70,127,255,cv2.THRESH_BINARY_INV)

filtered_edges_40_80 = cv2.Canny(image_filtered,40,80)
ret,filtered_edges_40_80_inverted = cv2.threshold(filtered_edges_40_80,127,255,cv2.THRESH_BINARY_INV)

filtered_edges_50_100 = cv2.Canny(image_filtered,50,100)
ret,filtered_edges_50_100_inverted = cv2.threshold(filtered_edges_50_100,127,255,cv2.THRESH_BINARY_INV)

filtered_edges_60_120 = cv2.Canny(image_filtered,60,120)
ret,filtered_edges_60_120_inverted = cv2.threshold(filtered_edges_60_120,127,255,cv2.THRESH_BINARY_INV)

filtered_edges_85_170 = cv2.Canny(image_filtered,85,170)
ret,filtered_edges_85_170_inverted = cv2.threshold(filtered_edges_85_170,127,255,cv2.THRESH_BINARY_INV)

filtered_edges_100_200 = cv2.Canny(image_filtered,100,200)
ret,filtered_edges_100_200_inverted = cv2.threshold(filtered_edges_100_200,127,255,cv2.THRESH_BINARY_INV)



gray_edges_20_30 = cv2.Canny(image_gray,20,30)
ret,gray_edges_20_30_inverted = cv2.threshold(gray_edges_20_30,127,255,cv2.THRESH_BINARY_INV)

gray_edges_30_70 = cv2.Canny(image_gray,30,70)
ret,gray_edges_30_70_inverted = cv2.threshold(gray_edges_30_70,127,255,cv2.THRESH_BINARY_INV)

gray_edges_40_80 = cv2.Canny(image_gray,40,80)
ret,gray_edges_40_80_inverted = cv2.threshold(gray_edges_40_80,127,255,cv2.THRESH_BINARY_INV)

gray_edges_50_100 = cv2.Canny(image_gray,50,100)
ret,gray_edges_50_100_inverted = cv2.threshold(gray_edges_50_100,127,255,cv2.THRESH_BINARY_INV)

gray_edges_60_120 = cv2.Canny(image_gray,60,120)
ret,gray_edges_60_120_inverted = cv2.threshold(gray_edges_60_120,127,255,cv2.THRESH_BINARY_INV)

gray_edges_85_170 = cv2.Canny(image_gray,85,170)
ret,gray_edges_85_170_inverted = cv2.threshold(gray_edges_85_170,127,255,cv2.THRESH_BINARY_INV)

gray_edges_100_200 = cv2.Canny(image_gray,100,200)
ret,gray_edges_100_200_inverted = cv2.threshold(gray_edges_100_200,127,255,cv2.THRESH_BINARY_INV)



gray_filtered_edges_20_30 = cv2.Canny(image_gray_filtered,20,30)
ret,gray_filtered_edges_20_30_inverted = cv2.threshold(gray_filtered_edges_20_30,127,255,cv2.THRESH_BINARY_INV)

gray_filtered_edges_30_70 = cv2.Canny(image_gray_filtered,30,70)
ret,gray_filtered_edges_30_70_inverted = cv2.threshold(gray_filtered_edges_30_70,127,255,cv2.THRESH_BINARY_INV)

gray_filtered_edges_40_80 = cv2.Canny(image_gray_filtered,40,80)
ret,gray_filtered_edges_40_80_inverted = cv2.threshold(gray_filtered_edges_40_80,127,255,cv2.THRESH_BINARY_INV)

gray_filtered_edges_50_100 = cv2.Canny(image_gray_filtered,50,100)
ret,gray_filtered_edges_50_100_inverted = cv2.threshold(gray_filtered_edges_50_100,127,255,cv2.THRESH_BINARY_INV)

gray_filtered_edges_60_120 = cv2.Canny(image_gray_filtered,60,120)
ret,gray_filtered_edges_60_120_inverted = cv2.threshold(gray_filtered_edges_60_120,127,255,cv2.THRESH_BINARY_INV)

gray_filtered_edges_85_170 = cv2.Canny(image_gray_filtered,85,170)
ret,gray_filtered_edges_85_170_inverted = cv2.threshold(gray_filtered_edges_85_170,127,255,cv2.THRESH_BINARY_INV)

gray_filtered_edges_100_200 = cv2.Canny(image_gray_filtered,100,200)
ret,gray_filtered_edges_100_200_inverted = cv2.threshold(gray_filtered_edges_100_200,127,255,cv2.THRESH_BINARY_INV)





cv2.imwrite('cropped_filtered' + str(local_time) + '.jpg',cropped_image_filtered)
cv2.imwrite('cropped_gray' + str(local_time) + '.jpg',cropped_image_gray)
cv2.imwrite('cropped_gray_filtered' + str(local_time) + '.jpg',cropped_image_gray_filtered)


cv2.imwrite('cropped_edges_20_30' + str(local_time) + '.jpg', cropped_edges_20_30_inverted)
cv2.imwrite('cropped_edges_30_70' + str(local_time) + '.jpg', cropped_edges_30_70_inverted)
cv2.imwrite('cropped_edges_40_80' + str(local_time) + '.jpg', cropped_edges_40_80_inverted)
cv2.imwrite('cropped_edges_50_100' + str(local_time) + '.jpg', cropped_edges_50_100_inverted)
cv2.imwrite('cropped_edges_60_120' + str(local_time) + '.jpg', cropped_edges_60_120_inverted)
cv2.imwrite('cropped_edges_85_170' + str(local_time) + '.jpg', cropped_edges_85_170_inverted)
cv2.imwrite('cropped_edges_100_200' + str(local_time) + '.jpg', cropped_edges_100_200_inverted)

cv2.imwrite('cropped_filtered_edges_20_30' + str(local_time) + '.jpg', cropped_filtered_edges_20_30_inverted)
cv2.imwrite('cropped_filtered_edges_30_70' + str(local_time) + '.jpg', cropped_filtered_edges_30_70_inverted)
cv2.imwrite('cropped_filtered_edges_40_80' + str(local_time) + '.jpg', cropped_filtered_edges_40_80_inverted)
cv2.imwrite('cropped_filtered_edges_50_100' + str(local_time) + '.jpg', cropped_filtered_edges_50_100_inverted)
cv2.imwrite('cropped_filtered_edges_60_120' + str(local_time) + '.jpg', cropped_filtered_edges_60_120_inverted)
cv2.imwrite('cropped_filtered_edges_85_170' + str(local_time) + '.jpg', cropped_filtered_edges_85_170_inverted)
cv2.imwrite('cropped_filtered_edges_100_200' + str(local_time) + '.jpg', cropped_filtered_edges_100_200_inverted)

cv2.imwrite('cropped_gray_edges_20_30' + str(local_time) + '.jpg', cropped_gray_edges_20_30_inverted)
cv2.imwrite('cropped_gray_edges_30_70' + str(local_time) + '.jpg', cropped_gray_edges_30_70_inverted)
cv2.imwrite('cropped_gray_edges_40_80' + str(local_time) + '.jpg', cropped_gray_edges_40_80_inverted)
cv2.imwrite('cropped_gray_edges_50_100' + str(local_time) + '.jpg', cropped_gray_edges_50_100_inverted)
cv2.imwrite('cropped_gray_edges_60_120' + str(local_time) + '.jpg', cropped_gray_edges_60_120_inverted)
cv2.imwrite('cropped_gray_edges_85_170' + str(local_time) + '.jpg', cropped_gray_edges_85_170_inverted)
cv2.imwrite('cropped_gray_edges_100_200' + str(local_time) + '.jpg', cropped_gray_edges_100_200_inverted)

cv2.imwrite('cropped_gray_filtered_edges_20_30' + str(local_time) + '.jpg', cropped_gray_filtered_edges_20_30_inverted)
cv2.imwrite('cropped_gray_filtered_edges_30_70' + str(local_time) + '.jpg', cropped_gray_filtered_edges_30_70_inverted)
cv2.imwrite('cropped_gray_filtered_edges_40_80' + str(local_time) + '.jpg', cropped_gray_filtered_edges_40_80_inverted)
cv2.imwrite('cropped_gray_filtered_edges_50_100' + str(local_time) + '.jpg', cropped_gray_filtered_edges_50_100_inverted)
cv2.imwrite('cropped_gray_filtered_edges_60_120' + str(local_time) + '.jpg', cropped_gray_filtered_edges_60_120_inverted)
cv2.imwrite('cropped_gray_filtered_edges_85_170' + str(local_time) + '.jpg', cropped_gray_filtered_edges_85_170_inverted)
cv2.imwrite('cropped_gray_filtered_edges_100_200' + str(local_time) + '.jpg', cropped_gray_filtered_edges_100_200_inverted)





cv2.imwrite('filtered' + str(local_time) + '.jpg',image_filtered)
cv2.imwrite('gray' + str(local_time) + '.jpg',image_gray)
cv2.imwrite('gray_filtered' + str(local_time) + '.jpg',image_gray_filtered)


cv2.imwrite('edges_20_30' + str(local_time) + '.jpg', edges_20_30_inverted)
cv2.imwrite('edges_30_70' + str(local_time) + '.jpg', edges_30_70_inverted)
cv2.imwrite('edges_40_80' + str(local_time) + '.jpg', edges_40_80_inverted)
cv2.imwrite('edges_50_100' + str(local_time) + '.jpg', edges_50_100_inverted)
cv2.imwrite('edges_60_120' + str(local_time) + '.jpg', edges_60_120_inverted)
cv2.imwrite('edges_85_170' + str(local_time) + '.jpg', edges_85_170_inverted)
cv2.imwrite('edges_100_200' + str(local_time) + '.jpg', edges_100_200_inverted)

cv2.imwrite('filtered_edges_20_30' + str(local_time) + '.jpg', filtered_edges_20_30_inverted)
cv2.imwrite('filtered_edges_30_70' + str(local_time) + '.jpg', filtered_edges_30_70_inverted)
cv2.imwrite('filtered_edges_40_80' + str(local_time) + '.jpg', filtered_edges_40_80_inverted)
cv2.imwrite('filtered_edges_50_100' + str(local_time) + '.jpg', filtered_edges_50_100_inverted)
cv2.imwrite('filtered_edges_60_120' + str(local_time) + '.jpg', filtered_edges_60_120_inverted)
cv2.imwrite('filtered_edges_85_170' + str(local_time) + '.jpg', filtered_edges_85_170_inverted)
cv2.imwrite('filtered_edges_100_200' + str(local_time) + '.jpg', filtered_edges_100_200_inverted)

cv2.imwrite('gray_edges_20_30' + str(local_time) + '.jpg', gray_edges_20_30_inverted)
cv2.imwrite('gray_edges_30_70' + str(local_time) + '.jpg', gray_edges_30_70_inverted)
cv2.imwrite('gray_edges_40_80' + str(local_time) + '.jpg', gray_edges_40_80_inverted)
cv2.imwrite('gray_edges_50_100' + str(local_time) + '.jpg', gray_edges_50_100_inverted)
cv2.imwrite('gray_edges_60_120' + str(local_time) + '.jpg', gray_edges_60_120_inverted)
cv2.imwrite('gray_edges_85_170' + str(local_time) + '.jpg', gray_edges_85_170_inverted)
cv2.imwrite('gray_edges_100_200' + str(local_time) + '.jpg', gray_edges_100_200_inverted)

cv2.imwrite('gray_filtered_edges_20_30' + str(local_time) + '.jpg', gray_filtered_edges_20_30_inverted)
cv2.imwrite('gray_filtered_edges_30_70' + str(local_time) + '.jpg', gray_filtered_edges_30_70_inverted)
cv2.imwrite('gray_filtered_edges_40_80' + str(local_time) + '.jpg', gray_filtered_edges_40_80_inverted)
cv2.imwrite('gray_filtered_edges_50_100' + str(local_time) + '.jpg', gray_filtered_edges_50_100_inverted)
cv2.imwrite('gray_filtered_edges_60_120' + str(local_time) + '.jpg', gray_filtered_edges_60_120_inverted)
cv2.imwrite('gray_filtered_edges_85_170' + str(local_time) + '.jpg', gray_filtered_edges_85_170_inverted)
cv2.imwrite('gray_filtered_edges_100_200' + str(local_time) + '.jpg', gray_filtered_edges_100_200_inverted)
'''


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

