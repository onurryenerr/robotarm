% width = 700; % image width  pixel 
% height = 484; % image height pixel
width = 700; % image width  pixel 
height = 484; % image height pixel

minLineLen = 2; % min length of line to record
minLineWidth = 3; %min width of thickest part of the of line

global penTip
penTip = 1;
penTipDouble = 2*penTip+1;
global image
image = transpose (cdata);
%image = repmat (cdata, 1); %rename image object to cdata. Should be logical
image(1:penTipDouble+1,:) = zeros; %clean borders
image((end-(penTipDouble)):end,:) = zeros;
image(:,1:penTipDouble+1) = zeros;
image(:,(end-(penTipDouble)):end) = zeros;

% global imageTemp
% imageTemp = repmat (image, 1);
%dlmwrite('azer.csv',cdata)

global k
k = 1;
global output %% one dimensional array holding output
%output = zeros(1,size*size);

output(k) = 2510; %lift the pen code
k=k+1;
output(k) = 2510;
k=k+1;

for i = (penTip+1):(width-penTip) %ignore borders, traverse
    for j = (penTip+1):(height-penTip)
      % if image (blackCounter(i,j) == penTip^2) % beginning of the line
      %  if blackCounter(i,j) >= ((penTip+2)^2)/2 % beginning of the line
      if blackCounter(i,j) >= minLineWidth % beginning of the line
           
%            visitedMarker(i,j); % mark all points as visited
%            neighbours = neighboursFinder(i,j); %get all neighbours coordinates in 8x2 matrix
%            nextPoint = biggestNeighbourFinder (neighbours);
                       
           visitedMarker(i,j);  %dont let extracting line towards current point           
           neighbours = neighboursFinder(i,j); %get all neighbours coordinates in 8x2 matrix
           nextPoint = biggestNeighbourFinder (neighbours); %find biggest neighbour to the current point
           line1 = lineExtractor (nextPoint(1),nextPoint(2)); %extract line starting from biggest neighbor
           nextPoint = biggestNeighbourFinder (neighbours); %find another neighbour to the current point
           line2 = lineExtractor (nextPoint(1),nextPoint(2));
             line = [flip(line1,1); [i j]; line2];
             %flip elements in each column add current point and line2
         if length(line(:,1))>=minLineLen   
             output (k) = line(1,1); %go to first point
             k=k+1;
             output (k) = line(1,2);
             k=k+1;           
             output (k) = 2520; %after going to first point, lower pen
             k=k+1;
             output (k) = 2520;
             k=k+1;
             for index=2:length(line(:,1))
                output (k) = line(index,1); %save point
                k=k+1;
                output (k) = line(index,2);
                k=k+1;
             end
              %add end of line
             output(k)=2510; %lift pen
             k=k+1;
             output(k)=2510;
             k=k+1;
         end
       end
    end         
 end

output(k)=2530; %end of the file
k=k+1;
output(k)=2530;

dlmwrite('fotoxy.csv',output) % output transformation matrix, 

function numBlack = blackCounter (x,y)
global penTip
global image
numBlack = 0;
    for i=-penTip:penTip
        for j=-penTip:penTip
            if image(x+i,y+j) == 1
            numBlack = numBlack + 1;
            end
        end
    end
end

function visitedMarker (x,y)
global penTip
global image
    for i=-penTip:penTip
        for j=-penTip:penTip
              image(x+i,y+j) = 0;   
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

function line = lineExtractor (x,y)
line = double.empty(0,2);
    while (x || y) %continue until line breaks
               %if nextPoint coordinate is 0,0, it means no neighbour found
              %update current coordinate
             line = [line; [x y]]; %add current point to the line
             visitedMarker(x,y); % mark all points as visited
             neighbours = neighboursFinder(x,y); %get all neighbours coordinates in 8x2 matrix
             nextPoint = biggestNeighbourFinder (neighbours);             
             x=nextPoint(1);
             y=nextPoint(2);
    end
end
