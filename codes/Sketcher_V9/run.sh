#!/bin/bash

cd /home/pi/Desktop/codes

file="../images"
if [ -d "$file" ]
then
	rm -r ../images
fi
mkdir ../images

pkill -f serialStarter.py
sleep 1
python3 serialStarter.py&

echo "Photo to edge started"
python3 photo2edge.py

file="../images/edge.csv"
if [ -f "$file" ]
then
	echo "$Success"
else
	echo "No face detected. Restarting"
	exec "./run.sh"
fi

#gcc edge2xy.c -lm -o edge2xy.out
echo "Edge to XY lines started"
./edge2xy.out

python XYToImage.py

echo "Transformer from XY to AB started"
gcc transformerPi.c -lm -o transformer.out
./transformer.out

file="../images/fotoxy.jpg"
if [ -f "$file" ]
then
	echo "$Success"
	pkill -f previewer.py
	python3 previewer.py&
fi

echo "Serial communication to Arduino started"
python3 serialComm.py
exec "./run.sh"
