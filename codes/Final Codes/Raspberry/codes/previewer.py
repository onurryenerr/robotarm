# from PIL import Image
# with Image.open('/home/pi/Desktop/edge.jpg') as img:
	# img.show()

# import webbrowser
# webbrowser.open('/home/pi/Desktop/edge.jpg')
# webbrowser.open('/home/pi/Desktop/photo.jpg')

import cv2
import numpy as np

orig=cv2.imread('../images/orig.jpg')
orig = orig[1:3279 , 108:2356]
crop=cv2.imread('../images/crop.jpg')
edge=cv2.imread('../images/edge.jpg')
fotoxy=cv2.imread('../images/fotoxy.jpg')

width = int(720/2.8)
height = int(1050/2.8)
dim = (width, height)
crop = cv2.resize(crop, dim, interpolation = cv2.INTER_AREA)
edge = cv2.resize(edge, dim, interpolation = cv2.INTER_AREA)
orig = cv2.resize(orig, dim, interpolation = cv2.INTER_AREA)
fotoxy = cv2.resize(fotoxy, dim, interpolation = cv2.INTER_AREA)

#concatanate image Horizontally
img_concate_Hori=np.concatenate((orig,crop,edge,fotoxy),axis=1)
#concatanate image Vertically
#img_concate_Verti=np.concatenate((edge,fotoxy),axis=1)
cv2.imshow('1. Original                                   2. Face detecion                                  3. Edge detection                                  4. Line conversion',img_concate_Hori)
#cv2.imshow('concatenated_Verti',img_concate_Verti)
cv2.waitKey(30000)
cv2.destroyAllWindows()


