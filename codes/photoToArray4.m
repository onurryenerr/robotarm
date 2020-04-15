clearvars -except cdata
% width = 700; % image width  pixel 
% height = 484; % image height pixel
width = 1050; % image width  pixel 
height = 720; % image height pixel

global minLineLen
global minBlackToBegin
global minBlackToCont
global connectionSize
global penTip

%% FINE ADJUSTING PARAMETERS
minLineLen = 10; % min length of line to record
minBlackToBegin = 1; %min # of blacks to begin line
minBlackToCont = 1; %min # of blacks to continue line
penTip = 1; %%defines sliding window's size. peTip=1 corresponds to
%3x3 sliding windows, penTip = 2=>5x5, 3=>7x7 and so on.
connectionSize = 3; %number of windows to nearby points to connect other line
%% END OF FINE ADJUSTING PARAMETERS

global windowSideSize
windowSideSize = 2*penTip+1;
global maxConnectionRange
maxConnectionRange = windowSideSize * connectionSize;
global xMin
global xMax
global yMin
global yMax
xMin = 3*penTip+2;
xMax = width-(3*penTip+1);
yMin = 3*penTip+2;
yMax = height-(3*penTip+1);
global image
image = transpose (cdata);
image = flip (image,1);

%image = repmat (cdata, 1); %rename image object to cdata. Should be logical
% image(1:windowSideSize+1,:) = zeros; %clean borders
% image((end-(windowSideSize)):end,:) = zeros;
% image(:,1:windowSideSize+1) = zeros;
% image(:,(end-(windowSideSize)):end) = zeros;
% global imageTemp
% imageTemp = repmat (image, 1);
%dlmwrite('azer.csv',cdata)

global index
index = 1;
global output %% one dimensional array holding output
%output = zeros(1,size*size);

output(index) = 2510; %lift the pen code
index=index+1;
output(index) = 2510;
index=index+1;

%%%%%%%%%%%%%%%%%%%%%%%%  LINE FINDER  %%%%%%%%%%%%%%%%%%%%
lines = {}; %cell array variable to store lines.
numLines = 0; %number of lines

for i = xMin:windowSideSize:xMax %ignore borders, traverse
    for j = yMin:windowSideSize:yMax
      % if image (blackCounter(i,j) == penTip^2) % beginning of the line
      %  if blackCounter(i,j) >= ((penTip+2)^2)/2 % beginning of the line
      if blackCounter(i,j) >= minBlackToBegin % beginning of the line
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
            numLines = numLines + 1;
            lines(numLines, :, :) = {line};
         end
       end
    end         
end
%%%%%%%%%%%%%%  END OF LINE FINDER  %%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%  LINE CONNECTOR  %%%%%%%%%%%%%%%%%
numOfReductions = 1;
linesConnected = {};
while numOfReductions > 0
linesConnected = {}; %cell array to hold lines after connection.
numOfReductions = 0;
flipFlag = 0;
linesWaiting = ones (1,numLines);
for i=1:size(lines,1) %%current line to find close lines in the end
    if linesWaiting (i) == 0 %%if the line is added dont proceed it anymore
        numOfReductions = numOfReductions + 1;
        continue
    end
    linesWaiting(i) = 0; %delete the line from waiting list
    foundFlag = 0; %no lines found to be connected yet
    x0 = lines{i}(end,1); %get end coordinates
    y0 = lines{i}(end,2);
    closePoints = closePointsFinder (x0, y0); %get close points coordinates
    for j=1:8 %close point number to find another line beginning or end
        x1 = closePoints(j,1); %current close point coordinates to find another line beginning or end
        y1 = closePoints(j,2);
        for k = i+1:size(lines,1)  %search all other available lines ends and beginnings
            if linesWaiting (k) == 1
                if (abs(lines{k}(1,1)-x1)<=maxConnectionRange) && (abs(lines{k}(1,2)-y1)<=maxConnectionRange)
                    flipFlag = 0; % no need to flip
                    foundFlag = 1;
                    break
                elseif (abs(lines{k}(end,1)-x1)<=maxConnectionRange) && (abs(lines{k}(end,2)-y1)<=maxConnectionRange)
                    flipFlag = 1; % no need to flip
                    foundFlag = 1;
                    break
                end
            end
        end
        if foundFlag == 1
            break
        end
    end
    if foundFlag == 0
    linesConnected(i-numOfReductions, :, :) = {lines{i}}; %no line found to add to the end of current line
    else
        if flipFlag == 1
            lines{k} = flip(lines{k});
        end
        linesConnected(i-numOfReductions, :, :) = {[lines{i};lines{k}]};
        linesWaiting(k) = 0;
    end
end
lines = linesConnected (:);
end

%%repeat line connector until no line concetenation occur

%%%%%%%%%%%%%%  END OF LINE CONNECTOR  %%%%%%%%%%%%%%%%%%%%%%%


%%%%%%%%%%%%%%  LINE ORDERER AND RECORDER  %%%%%%%%%%%%%%%%%%%%%%%
redNumOfLines = size(lines,1);
flipFlag = 0;
linesWaiting = ones (1,redNumOfLines);
currentLineNumber = 1; %start with the first line. find closest line ... 
%line beginning or line end to the end of current line
    while currentLineNumber~=0
        if flipFlag == 1 % flip the line if needed
            linesConnected{currentLineNumber} = flip(linesConnected{currentLineNumber});
        end
        lineRecorder (linesConnected{currentLineNumber}) %record current line
        linesWaiting(currentLineNumber) = 0; %delete the line from waiting list
        distMin = 2*(width+height);
        nextLineNumber = 0;
        for j=2:redNumOfLines
            if linesWaiting(j) == 1 %if line is not drawn yet
             %calc. distance between end of current line and beginning of next line
             dist = distCalc (linesConnected{currentLineNumber}(end,1), linesConnected{currentLineNumber}(end,2), ...
             linesConnected{j}(1,1), linesConnected{j}(1,2));
                if dist<distMin
                   distMin = dist;
                   nextLineNumber = j;
                   flipFlag = 0; %found point is beginning of line. no need to flip
                end
                
             dist = distCalc (linesConnected{currentLineNumber}(end,1), linesConnected{currentLineNumber}(end,2), ...
             linesConnected{j}(end,1), linesConnected{j}(end,2));
                if dist<distMin
                   distMin = dist;
                   nextLineNumber = j;
                   flipFlag = 1; %found point is the end of line. need to flip
                end                
            end
        end
           currentLineNumber = nextLineNumber;
    end

%%%%%%%%%%%%%% END OF LINE ORDERER AND RECORDER %%%%%%%%%%%%%%%%%%%%%%%
    
output(index)=2530; %end of the file
index=index+1;
output(index)=2530;

dlmwrite('fotoxy.csv',output) % output transformation matrix, 
%display result
numOfLines = 0;
outputImage = ones(width,height);
for coun = 1:2:(length(output))
    if output(coun)<2510
        outputImage(output(coun) , output(coun+1)) = 0;
    end
    if output(coun)==2510
        numOfLines = numOfLines + 1;
    end
    if output(coun)==2530
        break
    end
end
%outputImage=flip(outputImage,1);
%outputImage=flip(outputImage,2);
%Display parameters;
penTip
minLineLen
minBlackToBegin
minBlackToCont

%Display output
imshow(outputImage)
numLines
redNumOfLines

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
global windowSideSize
neighbours = zeros (8,2);
index = 1;
    for i=-1:1
        for j=-1:1
            if ((i~=0) || (j~=0)) %ignore point itself
             neighbours(index,1) = x+i*windowSideSize;
             neighbours(index,2) = y+j*windowSideSize;
             index = index+1;
            end
        end
    end
end

function closePoints = closePointsFinder (x,y)
global maxConnectionRange
closePoints = zeros (8,2);
index = 1;
    for i=-1:1
        for j=-1:1
            if ((i~=0) || (j~=0)) %ignore point itself
             closePoints(index,1) = x+i*maxConnectionRange;
             closePoints(index,2) = y+j*maxConnectionRange;
             index = index+1;
            end
        end
    end
end

function nextPoint = biggestNeighbourFinder (neighbours)
global minBlackToCont
global xMin
global xMax
global yMin
global yMax
maxNumBlack = minBlackToCont;
nextPoint = zeros (1,2);
    for i=1:8
        if neighbours (i,1)<xMin || neighbours (i,1)>xMax || neighbours (i,2)<yMin ...
                || neighbours (i,2)>yMax
        continue
        end
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

function dist = distCalc (x1,y1,x2,y2)
    dist = sqrt((x1-x2).^2+(y1-y2).^2);
end

function lineRecorder (line)
global output
global index

    output (index) = line(1,1); %go to first point
      
             index=index+1;
             output (index) = line(1,2);
             index=index+1;           
             output (index) = 2520; %after going to first point, lower pen
             index=index+1;
             output (index) = 2520;
             index=index+1;
             for i=2:length(line(:,1))
                output (index) = line(i,1); %save point
                index=index+1;
                output (index) = line(i,2);
                index=index+1;
             end
              %add end of line
             output(index)=2510; %lift pen
             index=index+1;
             output(index)=2510;
             index=index+1;
end