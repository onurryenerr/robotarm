import csv
import time
import array
from PIL import Image, ImageOps
import numpy as np

w, h = 1050, 720
xyImage = np.zeros((w, h, 3), dtype=np.uint8)


with open('../images/fotoxy.csv', 'r') as file:
    data = csv.reader(file)
    row = next(data)
    
i=0
while row[i]!="2530":
	if (int(row[i]) < 2510):
		xyImage[(int(row[i])-1):(int(row[i])+1), (int(row[i+1])-1):(int(row[i+1])+1)] = [255, 255, 255]
	i = i+2

img = Image.fromarray(xyImage, 'RGB')
img = ImageOps.invert(img)
img.save('../images/fotoxy.jpg', quality=95)
