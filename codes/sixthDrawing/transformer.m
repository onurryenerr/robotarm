clear all;
positions = readmatrix('fotoxy.csv');
trMatrix = readmatrix('paper.csv');

i=1;
j=1;

while (positions(j) ~= 2530)
    if (positions(j) >= 2510)
    output (i) = positions(j)/10;
    i=i+1;
    j=j+1;
    output (i) = positions(j)/10;
    i=i+1;
    j=j+1;
    
    else
    output (i) = 604+((2350-604)/180)*trMatrix(positions(j),positions(j+1));
    i=i+1;
    output (i) = 604+((2350-604)/180)*trMatrix(positions(j),484+positions(j+1));
    i=i+1;  
    j=j+2;
    end
    
    if (i>5 && (output(i-1)==output(i-3)) && (output(i-2)==output(i-4)) )
        i=i-2; %% eliminate repetitive points
    end
    
    if (i>7 && (output(i-3)==252) && (output(i-1)==output(i-5)) && (output(i-2)==output(i-6)) )
        i=i-2; %% eliminate repetitive points after lower position command
    end
    
    if (i>5 && (output(i-1)==251) && (output(i-3)==252))
         i=i-6; %% eliminate single point
    end
    
end

output = round (output);

output (i) = 253; %end of file
i=i+1;
output (i) = 253;

output2 = output(:,1:i); %get rid of elements after end of file code

dlmwrite('fotoab.csv',output2)