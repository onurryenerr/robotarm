# from PIL import Image
# with Image.open('/home/pi/Desktop/edge.jpg') as img:
	# img.show()

# import webbrowser
# webbrowser.open('/home/pi/Desktop/edge.jpg')
# webbrowser.open('/home/pi/Desktop/photo.jpg')

import cv2
import numpy as np

orig=cv2.imread('../images/orig.jpg')
crop=cv2.imread('../images/crop.jpg')
hist=cv2.imread('../images/hist.jpg')
clahe=cv2.imread('../images/clahe.jpg')
blur=cv2.imread('../images/blur.jpg')
edge=cv2.imread('../images/edge.jpg')
fotoxy=cv2.imread('../images/fotoxy.jpg')
logo=cv2.imread('logo.png')

dScale = 3.1
width = int(720/dScale)
height = int(1050/dScale)
orig = cv2.resize(orig, (width*2,height), interpolation = cv2.INTER_AREA)
logo = cv2.resize(logo, (100,2*height), interpolation = cv2.INTER_AREA)
dim = (width, height)
crop = cv2.resize(crop, dim, interpolation = cv2.INTER_AREA)
edge = cv2.resize(edge, dim, interpolation = cv2.INTER_AREA)
fotoxy = cv2.resize(fotoxy, dim, interpolation = cv2.INTER_AREA)
hist = cv2.resize(hist, dim, interpolation = cv2.INTER_AREA)
clahe = cv2.resize(clahe, dim, interpolation = cv2.INTER_AREA)
blur = cv2.resize(blur, dim, interpolation = cv2.INTER_AREA)


#concatanate image Horizontally
up=np.concatenate((orig,crop,blur),axis=1)
#concatanate image Horizontally
down=np.concatenate((hist,clahe,edge,fotoxy),axis=1)
#concatanate image Vertically
img=np.concatenate((up,down),axis=0)
#add logo
img=np.concatenate((img,logo),axis=1)
cv2.imshow('1.Original  2.Face detecion  3.Bilateral filter  4.Histogram normalization  5.CLAHE normalization  6.Edge detection  7.Line extraction',img)
cv2.waitKey(0)
cv2.destroyAllWindows()


