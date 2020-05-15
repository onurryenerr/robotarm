#!/bin/bash
file="../images"
if [ -d "$file" ]
then
	rm -r ../images
fi
mkdir ../images

echo "Photo to edge started"
python3 photo2edge.py

echo "Edge to XY lines started"
./edge2xy.out

python XYToImage.py

echo "Transformer from XY to AB started"
./transformer.out

echo "Serial communication to Arduino started"
python3 serialComm.py

file="../images/edge.jpg"
if [ -f "$file" ]
then
	echo "$Success"
else
	echo "$..."
fi
