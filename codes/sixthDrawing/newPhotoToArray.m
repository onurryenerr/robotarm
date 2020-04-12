% width = 700; % image width  pixel 
% height = 484; % image height pixel
width = 700; % image width  pixel 
height = 484; % image height pixel

global penTip
penTip = 1;
penTipDouble = 2*penTip;
global tempData
tempData = transpose (cdata);
%tempData = repmat (cdata, 1); %rename image object to cdata. Should be logical
tempData(1:penTipDouble+1,:) = zeros; %clean borders
tempData((end-(penTipDouble)):end,:) = zeros;
tempData(:,1:penTipDouble+1) = zeros;
tempData(:,(end-(penTipDouble)):end) = zeros;

%dlmwrite('azer.csv',cdata)

global k
k = 1;
global output %% one dimensional array holding output
%output = zeros(1,size*size);

output(k) = 2510; %lift the pen code
k=k+1;
output(k) = 2510;
k=k+1;
for i = (penTip+1):(width-penTip) %ignore borders, traverse as matrix
    for j = (penTip+1):(height-penTip)
      % if tempData (blackCounter(i,j) == penTip^2) % beginning of the line
        if blackCounter(i,j) >= 3 % beginning of the line            
           output (k) = i; %go to first point
           k=k+1;
           output (k) = j;
           k=k+1;           
           output (k) = 2520; %after going to first point, lower pen
           k=k+1;
           output (k) = 2520;
           k=k+1;
           
           visitedMarker(i,j); % mark all points as visited
           neighbours = neighboursFinder(i,j); %get all neighbours coordinates in 8x2 matrix
           nextPoint = biggestNeighbourFinder (neighbours);
           %find neighbour coordinate with max. number of blacks
        
           while (nextPoint(1,1) || nextPoint(1,2)) %continue until line breaks
               %if nextPoint coordinate is 0,0, it means no neighbour found
             ti=nextPoint(1); %update current coordinate
             tj=nextPoint(2);
             output (k) = ti; %save point
             k=k+1;
             output (k) = tj;
             k=k+1;
             visitedMarker(ti,tj); % mark all points as visited
             neighbours = neighboursFinder(ti,tj); %get all neighbours coordinates in 8x2 matrix
             nextPoint = biggestNeighbourFinder (neighbours);
           end
           %no neighbour point is found, end of line
             output(k)=2510; %lift pen
             k=k+1;
             output(k)=2510;
             k=k+1;
       end
    end           
 end

output(k)=2530; %end of the file
k=k+1;
output(k)=2530;

dlmwrite('fotoxy.csv',output) % output transformation matrix, 

function numBlack = blackCounter (x,y)
global penTip
global tempData
numBlack = 0;
    for i=-penTip:penTip
        for j=-penTip:penTip
            if tempData(x+i,y+j) == 1
            numBlack = numBlack + 1;
            end
        end
    end
end

function visitedMarker (x,y)
global penTip
global tempData
    for i=-penTip:penTip
        for j=-penTip:penTip
              tempData(x+i,y+j) = 0;   
        end
    end
end

function neighbours = neighboursFinder (x,y)
global penTip
neighbours = zeros (8,2);
index = 1;
    for i=-1:1
        for j=-1:1
            if ((i~=0) || (j~=0)) %ignore point itself
             neighbours(index,1) = x+i*penTip;
             neighbours(index,2) = y+j*penTip;
             index = index+1;
            end
        end
    end
end

function nextPoint = biggestNeighbourFinder (neighbours)
maxNumBlack = 0;
nextPoint = zeros (1,2);
    for i=1:8
        
        if blackCounter(neighbours (i,1), neighbours (i,2)) > maxNumBlack
            maxNumBlack = blackCounter(neighbours (i,1), neighbours (i,2));
            nextPoint = neighbours(i,:); %return biggest neighbour coordinate
        end
    end
end