width = 700; % image width  pixel 
height = 484; % image height pixel

global tempdata
global tempdataTemp
global lenLine
lenLine = 0;
global penTip
penTip = 3;

%tempdata = repmat (cdata, 1); %Should be logical
tempdata = transpose (cdata);

tempdata(1:penTip+1,:) = zeros; %clean borders
tempdata((end-(penTip)):end,:) = zeros;
tempdata(:,1:penTip+1) = zeros;
tempdata(:,(end-(penTip)):end) = zeros;

global fullCheckFlag
fullCheckFlag = 1;
global fullCheckFlagTemp
fullCheckFlagTemp = 1;

global di
di = 0;
global dj
dj = 0;
global ti
global tj
global tiTemp
global tjTemp
global k
k = 1;
global found
found = 0;
global output %% one dimensional array holding output
%output = zeros(1,size*size);

output(k) = 2510; %lift the pen code
k=k+1;
output(k) = 2510;
k=k+1;
for i = 2:(width-1) %ignore borders
    for j = 2:(height-1)
        if tempdata (i,j) == 1 % beginning of the line
           
           output (k) = i; %go to first point
           k=k+1;
           output (k) = j;
           k=k+1;
           
           output (k) = 2520; %after going to first point, lower pen
           k=k+1;
           output (k) = 2520;
           k=k+1;
           
           tempdata (i,j) = 0;
           ti=i;
           tj=j;
           fullCheckFlag = 1; 
           
           while (fullCheckFlag<2)
           
           if ((di == -1 && dj == -1))
               order = [1 4 2 6 3 7 5 8];
               orderer (order)
           end 
           
           if (di == -1 && dj == -0)
               order = [2 1 3 4 5 6 8 7];
               orderer (order)                
           end
           
           if (di == -1 && dj == 1)
                order = [3 2 5 1 8 4 7 6];
                orderer (order) 
           end
           
           if (di == 0 && dj == -1)
               order = [4 6 1 7 2 8 3 5];
               orderer (order)    
           end
           
           if (di == 0 && dj == 1)
               order = [5 3 8 2 7 1 6 4];
               orderer (order)       
           end
           
           if (di == 1 && dj == -1)
               order = [6 7 4 8 1 5 2 3];
               orderer (order)
           end
           
           if (di == 1 && dj == 0)
               order = [7 8 6 5 4 3 1 2];
               orderer (order)       
           end
           
           if (di == 1 && dj == 1)  % up right or newline               
               order = [8 5 7 3 6 2 4 1];
               orderer (order)                
           end
           
           if (di == 0 && dj == 0)  % newline
               maxDirFinder ()
           end
               
           fullCheckFlag = fullCheckFlag + 1;
           
           if fullCheckFlag == 2  %no neighbour point is found, end of line
               di = 0; 
               dj = 0;
               output(k)=2510; %lift pen
               k=k+1;
               output(k)=2510;
               k=k+1;         
           end
           
           end           
        end
    end
end

output(k)=2530; %end of the file
k=k+1;
output(k)=2530;

dlmwrite('fotoxy.csv',output) % output transformation matrix,
lower = 0;

for i=1:k 
    if (i>5 && (output(i-1)==2510) && (output(i-3)==2520))
         i=i-6; %% eliminate single point
    end
end

for i=1:k
    if output(i) == 2520
        lower = lower+1; %number of lower
    end
end
lower

clear output
%clearvars -except cdata


function order1 ()
    global ti
    global tj
    global tempdata
    global found
    if tempdata (ti-1,tj-1) == 1
       recorder(ti-1,tj-1)
       found = 1;
    end
end

function order2 ()
    global ti
    global tj
    global tempdata
    global found
    if tempdata (ti-1,tj) == 1
       recorder(ti-1,tj)
       found = 1;
    end
end 

function order3 ()
    global ti
    global tj
    global tempdata
    global found
    if tempdata (ti-1,tj+1) == 1
       recorder(ti-1,tj+1)
       found = 1;
    end
end

function order4 ()
    global ti
    global tj
    global tempdata
    global found
    if tempdata (ti,tj-1) == 1
       recorder(ti,tj-1)
       found = 1;
    end
end

function order5 ()
    global ti
    global tj
    global tempdata
    global found
    if tempdata (ti,tj+1) == 1
       recorder(ti,tj+1)
       found = 1;
    end
end

function order6 ()
    global ti
    global tj
    global tempdata
    global found
    if tempdata (ti+1,tj-1) == 1
       recorder(ti+1,tj-1)
       found = 1;
    end
end

function order7 ()
    global ti
    global tj
    global tempdata
    global found
    if tempdata (ti+1,tj) == 1
       recorder(ti+1,tj)
       found = 1;
    end
end

function order8 ()
    global ti
    global tj
    global tempdata
    global found
    if tempdata (ti+1,tj+1) == 1
       recorder(ti+1,tj+1)
       found = 1;
    end
end

function orderer (order)
   global found
   i=1;
   found = 0;
   while i<=8
       if order(i) == 1
           order1()
         elseif order(i) == 2
           order2()
         elseif order(i) == 3
           order3()
         elseif order(i) == 4
           order4()
         elseif order(i) == 5
           order5()
         elseif order(i) == 6
           order6()
         elseif order(i) == 7
           order7()
         elseif order(i) == 8
           order8()  
       end
       if (found == 1)
           i=9; %exit loop
       else
           i=i+1;
       end
   end    
end

function recorder(x,y)
    global fullCheckFlag
    global ti
    global tj
    global di
    global dj
    global k
    global output
    global tempdata
    global penTip
    
    fullCheckFlag=0;
    di=x-ti; %save direction
    dj=y-tj;    
   
    tempdata (x,y) = 0;
    
    for i = 1:penTip
    tempdata (ti + i*dj, tj - i*di) = 0;
    tempdata (ti - i*dj, tj + i*di) = 0;
    end
           
    ti=x;
    tj=y;
    output (k) = x;
    k=k+1;
    output (k) = y;
    k=k+1;
end

function order1Temp ()
    global tiTemp
    global tjTemp
    global tempdataTemp
    global found
    if tempdataTemp (tiTemp-1,tjTemp-1) == 1
       recorderTemp(tiTemp-1,tjTemp-1)
       found = 1;
    end
end

function order2Temp ()
    global tiTemp
    global tjTemp
    global tempdataTemp
    global found
    if tempdataTemp (tiTemp-1,tjTemp) == 1
       recorderTemp(tiTemp-1,tjTemp)
       found = 1;
    end
end 

function order3Temp ()
    global tiTemp
    global tjTemp
    global tempdataTemp
    global found
    if tempdataTemp (tiTemp-1,tjTemp+1) == 1
       recorderTemp(tiTemp-1,tjTemp+1)
       found = 1;
    end
end

function order4Temp ()
    global tiTemp
    global tjTemp
    global tempdataTemp
    global found
    if tempdataTemp (tiTemp,tjTemp-1) == 1
       recorderTemp(tiTemp,tjTemp-1)
       found = 1;
    end
end

function order5Temp ()
    global tiTemp
    global tjTemp
    global tempdataTemp
    global found
    if tempdataTemp (tiTemp,tjTemp+1) == 1
       recorderTemp(tiTemp,tjTemp+1)
       found = 1;
    end
end

function order6Temp ()
    global tiTemp
    global tjTemp
    global tempdataTemp
    global found
    if tempdataTemp (tiTemp+1,tjTemp-1) == 1
       recorderTemp(tiTemp+1,tjTemp-1)
       found = 1;
    end
end

function order7Temp ()
    global tiTemp
    global tjTemp
    global tempdataTemp
    global found
    if tempdataTemp (tiTemp+1,tjTemp) == 1
       recorderTemp(tiTemp+1,tjTemp)
       found = 1;
    end
end

function order8Temp ()
    global tiTemp
    global tjTemp
    global tempdataTemp
    global found
    if tempdataTemp (tiTemp+1,tjTemp+1) == 1
       recorderTemp(tiTemp+1,tjTemp+1)
       found = 1;
    end
end


function ordererTemp2 (order)
   global found
   global lenLine
   i=1;
   found = 0;
   while i<=8
       if order(i) == 1
           order1Temp()
       elseif order(i) == 2
           order2Temp()
       elseif order(i) == 3
           order3Temp()
       elseif order(i) == 4
           order4Temp()
       elseif order(i) == 5
           order5Temp()
       elseif order(i) == 6
           order6Temp()
       elseif order(i) == 7
           order7Temp()
       elseif order(i) == 8
           order8Temp()  
       end
        if (found == 1)
            i=9; %exit loop
            lenLine = lenLine + 1;
        else
            i=i+1;
        end
   end    
end



function maxDirFinder ()
global lenLine
global tempdataTemp
global tempdata
global di
global dj
global fullCheckFlag
global tiTemp
global tjTemp
global ti
global tj
maxLength = 0;
maxLenDir = 0;
lengths = zeros(1,8);
   for dir = 1:8
       lenLine = 0;
       tempdataTemp = repmat (tempdata, 1);
       tiTemp = ti;
       tjTemp = tj;
       pathFinder(dir)
       lengths(dir) = lenLine;
       if (lengths(dir))>maxLength
           maxLength = lengths(dir);
           maxLenDir = dir;
       end
   end
   
 if maxLength == 0 % no following point is found
    di = 0;
    dj = 0;
    fullCheckFlag = 1;
 else
    fullCheckFlag = 0; 
    %some points found. Continue from the direction given below. 
 end
 
 if maxLenDir == 1
     di = -1; dj = -1;
 elseif maxLenDir == 2
     di = -1; dj = 0;
 elseif maxLenDir == 3
     di = -1; dj = 1;
 elseif maxLenDir == 4
     di = 0; dj = -1;
 elseif maxLenDir == 5
     di = 0; dj = 1;
 elseif maxLenDir == 6
     di = 1; dj = -1;
 elseif maxLenDir == 7
     di = 1; dj = 0;
 elseif maxLenDir == 8
     di = 1; dj = 1;
 end
   
end




function recorderTemp(x,y)
    global fullCheckFlagTemp    
    global tiTemp
    global tjTemp
    global di
    global dj
    global tempdataTemp
    global penTip
    
    fullCheckFlagTemp=0;
    di=x-tiTemp; %save direction
    dj=y-tjTemp;    
   
    tempdataTemp (x,y) = 0;    
    for i = 1:penTip
    tempdataTemp (tiTemp + i*dj, tjTemp - i*di) = 0;
    tempdataTemp (tiTemp - i*dj, tjTemp + i*di) = 0;
    end
            
    tiTemp=x;
    tjTemp=y;
end


function pathFinder (dir)                      
global di
global dj
fullCheckFlagTemp = 1;            
           while (fullCheckFlagTemp<2)
           
           if ((di == -1 && dj == -1) || (dir==1))
               dir = 0;
               order = [1 4 2 6 3 7 5 8];
               ordererTemp2 (order)
           end 
           
           if ((di == -1 && dj == -0) || (dir==2) )
               dir = 0;
               order = [2 1 3 4 5 6 8 7];
               ordererTemp2 (order)                
           end
           
           if ((di == -1 && dj == 1) || (dir==3) )
               dir = 0;
                order = [3 2 5 1 8 4 7 6];
                ordererTemp2 (order) 
           end
           
           if ((di == 0 && dj == -1) || (dir==4) )
               dir = 0;
               order = [4 6 1 7 2 8 3 5];
               ordererTemp2 (order)    
           end
           
           if ((di == 0 && dj == 1) || (dir==5) )
               dir = 0;
               order = [5 3 8 2 7 1 6 4];
               ordererTemp2 (order)       
           end
           
           if ((di == 1 && dj == -1) || (dir==6) )
               dir = 0;
               order = [6 7 4 8 1 5 2 3];
               ordererTemp2 (order)
           end
           
           if ((di == 1 && dj == 0) || (dir==7))
               dir = 0;
               order = [7 8 6 5 4 3 1 2];
               ordererTemp2 (order)       
           end
           
           if ((di == 1 && dj == 1) || (dir==8))
               dir = 0;             
               order = [8 5 7 3 6 2 4 1];
               ordererTemp2 (order)                
           end
           
           fullCheckFlagTemp = fullCheckFlagTemp + 1;
           
           if fullCheckFlagTemp == 2  %no neighbour point is found, end of line
               di = 0; 
               dj = 0;     
           end
           
           end           
        
end