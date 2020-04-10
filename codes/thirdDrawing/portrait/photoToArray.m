width = 350; % image width  pixel 
height = 242; % image height pixel

global tempdata
cdata = transpose (cdata);
tempdata = repmat (cdata, 1); %rename image object to cdata. Should be logical
tempdata(1,:) = zeros; %clean borders
tempdata(end,:) = zeros;
tempdata(:,1) = zeros;
tempdata(:,end) = zeros;

global fullCheckFlag
fullCheckFlag = 1;
global di
di = 0;
global dj
dj = 0;
global ti
ti = 2;
global tj
tj = 2;
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
           
           if ((di == 1 && dj == 1 ) || (di == 0 && dj == 0))  % up right or newline               
               order = [8 5 7 3 6 2 4 1];
               orderer (order)
                
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
    
    fullCheckFlag=0;
    di=x-ti; %save direction
    dj=y-tj;    
    ti=x;
    tj=y;
    output (k) = x;
    k=k+1;
    output (k) = y;
    k=k+1;
    tempdata (x,y) = 0;
end