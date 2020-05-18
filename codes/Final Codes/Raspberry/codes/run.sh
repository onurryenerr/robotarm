#!/bin/bash

cd /home/pi/Desktop/codes

file="../images"
if [ -d "$file" ]
then
	rm -r ../images
fi
mkdir ../images

sleep 5
echo "Photo to edge started"
python3 photo2edge.py

file="../images/edge.csv"
if [ -f "$file" ]
then
	echo "$Success"
else
	echo "No face detected. Restarting"
	sleep 5
	exec "./run.sh"
fi

#gcc edge2xy.c -lm -o edge2xy.out
echo "Edge to XY lines started"
./edge2xy.out

python XYToImage.py

echo "Transformer from XY to AB started"
./transformer.out

file="../images/fotoxy.jpg"
if [ -f "$file" ]
then
	echo "$Success"
	python3 previewer.py&
fi

echo "Transformer from XY to AB started"
./transformer.out

echo "Serial communication to Arduino started"
#python3 serialComm.py

sleep 10
exec "./run.sh"
