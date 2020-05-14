
/* Include files */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

//parameters to modify
#define  width 1050
#define  height 720
const int minLineLen = 12;
const int minBlackToBegin = 8;
const int minBlackToCont = 1;
const int penTip = 1; 
const int maxConnectionRange = 14;
//end of parameters to modify

const int windowSideSize = 2*penTip+1;
#define numOfNeighbours 8*windowSideSize
const int xMin = 3*penTip+2;
const int xMax = width-(3*penTip+1);
const int yMin = 3*penTip+2;
const int yMax = height-(3*penTip+1);

short int image[width][height] = {0};
short int output[9999999] = {0};
short int line[20000][2] = {0};
short int line1[20000][2] = {0};
short int line2[20000][2] = {0};
short int neighbours[56][2] = {0};
short int nextPoint[2] = {0};
unsigned int index1 = 1;

/* Function Definitions */
void imageReader(short int image[width][height]) ;
void outputWriter(short int output[], unsigned int  count) ;
int blackCounter (int x,int y);
void visitedMarker (int x,int y);
void unvisitedMarker (int x,int y);
void neighboursFinder (int x,int y);
void biggestNeighbourFinder (void);
void recoverLine (void);
void lineExtractor (short int x, short int y, short int line[20000][2]);
int distCalc (short int x1, short int y1, short int x2, short int y2);
void lineRecorder(short int line[20000][2]);

void concLines1(int i,int j);
void concLines(short int line1[20000][2],short int line2[20000][2]);
int lengthOfLine(short int line[20000][2]);
int countOfLines(short int lines[2000][20000][2]);
void copyLine(short int dst[20000][2], short int src[20000][2], int index1);
void copyLines(short int lines1[2000][20000][2], short int lines2[2000][20000][2]);
void addPosToLine(short int line[20000][2], short int x,short int y);
void flip(short int line[20000][2]);
void swap (short int* a,short int* b);

/* Function Definitions */
void main(void)
{ 
  int numLines = 0;
  int i = 0, j = 0;
  
  static short int lines[2000][20000][2];
  static short int lines_connected[2000][20000][2];   
//  memset(&image[0][0], 0, width*height * sizeof(short int));
// imageReader(image);  
//  memset(&output[0], 0, 9999999 * sizeof(short int));
//  outputWriter(output);
  
  memset(&lines[0][0][0], 0, 2000*20000*2 * sizeof(short int));
  memset(&lines_connected[0][0][0], 0, 2000*20000*2 * sizeof(short int));
  
  
  output[index1-1] = 2510; //lift the pen code
  index1=index1+1;
  output[index1-1] = 2510;
  index1=index1+1;
  
  //for i = xMin:windowSideSize:xMax
  for (i=xMin;i<=xMax;i+=windowSideSize){
      for (j=yMin;j<=yMax;j+=windowSideSize){
        if (blackCounter(i,j) >= minBlackToBegin){
          visitedMarker(i,j);
          neighboursFinder(i,j); //assign to neighbours[][]
          biggestNeighbourFinder(); //assign to nexPoint[2]
          lineExtractor (nextPoint[0],nextPoint[1],line1); //assign to line1
          biggestNeighbourFinder();
          lineExtractor (nextPoint[0],nextPoint[1],line2); //assign to line2
          concLines1(i,j);//line = [flip(line1,1); [i j]; line2];
          if (lengthOfLine(line)>=minLineLen){
            numLines = numLines + 1;            
            copyLine (lines[numLines-1],line,0);   //dest, source, dest index         
          }
          else
             recoverLine();
        }
      }
  }
  lines[numLines][0][0] = 9999;
 ///end of line finder
 
 ///line connector 
  int linesWaiting [2000];
  int numOfReductions = 1;
  int flipFlag = 0;
  int foundFlag;
  int x0,y0,x,y,k,distMin,distMinNum,length;
  float dist;
  while (numOfReductions > 0) {
    memset(&lines_connected[0][0][0], 0, 2000*20000*2 * sizeof(short int));
    numOfReductions = 0;
    flipFlag = 0;
    memset(&linesWaiting[0], 1, 2000 * sizeof(int));
    for(i=1;i<=countOfLines(lines);i++){ //after the lines in lines matrix, the new line should indicate the end
        if (linesWaiting[i-1] == 0){
        numOfReductions = numOfReductions + 1;
        continue;
        }
        linesWaiting[i-1] = 0;
        foundFlag = 0;
        length = lengthOfLine(lines[i-1]);
        x0 = lines[i-1][length-1][0]; //get end coordinates
        y0 = lines[i-1][length-1][1];
        for (k = i+1; k<=countOfLines(lines); k++) {
          distMin = 2147483647; //INT_MAX
          if (linesWaiting[k-1] == 1){
            if ((abs(lines[k-1][0][0]-x0)<=maxConnectionRange) && (abs(lines[k-1][0][1]-y0)<=maxConnectionRange)){
              dist = distCalc (lines[k-1][0][0],x0,lines[k-1][0][1],y0);
              if (dist<distMin){
                distMin = dist;
                distMinNum = k;
                flipFlag = 0; // no need to flip
                foundFlag = 1;
              }
            }
            else if((abs(lines[k-1][lengthOfLine(lines[k-1])-1][0]-x0)<=maxConnectionRange) && (abs(lines[k-1][lengthOfLine(lines[k-1])-1][1]-y0)<=maxConnectionRange)){
              dist = distCalc (lines[k-1][lengthOfLine(lines[k-1])-1][0],x0,lines[k-1][lengthOfLine(lines[k-1])-1][1],y0);
              if (dist<distMin){
                distMin = dist;
                distMinNum = k;
                flipFlag = 1; // flip
                foundFlag = 1;
              }
            }             
          }
        }
        if (foundFlag == 0)
          copyLine(lines_connected[i-numOfReductions-1],lines[i-1],0); //copyLine(dest,src,dest index)
        else{
          if(flipFlag == 1){
            flip(lines[distMinNum-1]);
            }
          concLines(lines[i-1],lines[distMinNum-1]); //concLines concatanates src to dst
          copyLine(lines_connected[i-numOfReductions-1],lines[i-1],0); 
          linesWaiting[distMinNum-1] = 0;
        }
    }
    //memcpy(&lines[0][0][0], &lines_connected[0][0][0], 2000*20000*2 * sizeof(short int))
    lines_connected[i-numOfReductions][0][0] = 9999;
    copyLines(lines, lines_connected);
  }
  ///end of connector 
  
  ///start of orderer
  int redNumOfLines = countOfLines(lines);
  int nextLineNumber;
  flipFlag = 0;
  memset(&linesWaiting[0], 1, 2000 * sizeof(int));
  int currentLineNumber = 1;
  while (currentLineNumber!=0) {
    if (flipFlag == 1)
      flip(lines_connected[currentLineNumber-1]);
    lineRecorder(lines_connected[currentLineNumber-1]);
    linesWaiting[currentLineNumber-1] = 0;
    distMin = 2147483647;
    nextLineNumber = 0;
    for (j=2;j<=redNumOfLines;j++){
      if (linesWaiting[i-1] == 1){
        dist = distCalc (lines_connected[currentLineNumber-1][lengthOfLine(lines_connected[currentLineNumber-1])-1][0], 
        lines_connected[currentLineNumber-1][lengthOfLine(lines_connected[currentLineNumber-1])-1][1],
        lines_connected[j-1][0][0], lines_connected[j-1][0][1]);
        if (dist<distMin){
          distMin = dist;
          nextLineNumber = j;
          flipFlag = 0;
        }
        dist = distCalc (lines_connected[currentLineNumber-1][lengthOfLine(lines_connected[currentLineNumber-1])-1][0], 
        lines_connected[currentLineNumber-1][lengthOfLine(lines_connected[currentLineNumber-1])-1][1],
        lines_connected[j-1][lengthOfLine(lines_connected[j-1])-1][0], lines_connected[j-1][lengthOfLine(lines_connected[j-1])-1][1]);
        if (dist<distMin){
          distMin = dist;
          nextLineNumber = j;
          flipFlag = 1;
        }
      }
    }
    currentLineNumber = nextLineNumber;
  }
  ///end of orderer
  
  output[index1-1] = 2530; //end of the file
  index1=index1+1;
  output[index1-1] = 2530;
  
  outputWriter(output,index1);
}  

  int blackCounter (int x,int y){
    int numBlack = 0;
    int i,j;
    for (i=-penTip;i<=penTip;i++){
        for (j=-penTip;j<=penTip;j++){
            if (image[x+i-1][y+j-1] == 1){
                numBlack = numBlack + 1;
            }
        }
    }
    return numBlack;
  }
  
  void visitedMarker (int x,int y){
    int i, j;
    for (i=-penTip;i<=penTip;i++){
        for (j=-penTip;j<=penTip;j++){
            image[x+i-1][y+j-1] = 0;        
        }
    }
  }
  
  void unvisitedMarker (int x,int y){
    int i, j;
    for (i=-penTip;i<=penTip;i++){
        for (j=-penTip;j<=penTip;j++){
            image[x+i-1][y+j-1] = 1;        
        }
    }
  }
  
  void neighboursFinder (int x,int y){
    int i, j, index1=1;
    memset(neighbours, 0, numOfNeighbours*sizeof(short int));
    for (i=-windowSideSize;i<=windowSideSize;i++){
      for (j=-windowSideSize;j<=windowSideSize;j++){
        if (i>-windowSideSize && i<windowSideSize &&
         j>-windowSideSize && j<windowSideSize){
         continue;
        }
        neighbours[index1-1][0] = x+i;
        neighbours[index1-1][1] = y+j;
        index1 = index1+1;       
      }
    }
  }  
  
  void biggestNeighbourFinder (void){
    int maxNumBlack = minBlackToCont;
    memset(nextPoint, 0, 2*sizeof(short int));
    int i,max;
    for(i=1;i<=numOfNeighbours;i++){
      if (neighbours [i-1][0]<xMin || neighbours [i-1][0]>xMax || neighbours [i-1][1]<yMin
                || neighbours [i-1][1]>yMax)
        continue;
      if (max=blackCounter(neighbours [i-1][0], neighbours [i-1][1]) > maxNumBlack){
            maxNumBlack = max;
            nextPoint[0] = neighbours[i-1][0];// %return biggest neighbour coordinate
            nextPoint[1] = neighbours[i-1][1];
      }
    }
  }
  
  void recoverLine (void){
    int i;
    for (i=1;i<=lengthOfLine(line);i++){
      unvisitedMarker(line[i-1][0],line[i-1][1]);    
    }
  }

void lineExtractor (short int x, short int y, short int line[20000][2]){
  line[0][0] = 9999;
  while (x || y){
      addPosToLine(line, x, y);
      visitedMarker(x,y);
      neighboursFinder(x,y);
      biggestNeighbourFinder();
      x=nextPoint[0];
      y=nextPoint[1];
  }
}

int distCalc (short int x1, short int y1, short int x2, short int y2){
    return ((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

void lineRecorder(short int line[20000][2]){
    int i;
    output [index1-1] = line[0][0];
    index1++;
    output [index1-1] = line[0][1];
    index1++;
    output[index1-1] = 2520;
    index1++;
    output[index1-1] = 2520;
    index1++;
    for (i=2;i<=lengthOfLine(line);i++){
      printf(".");
      output [index1-1] = line[i-1][0];
      index1++;
      output [index1-1] = line[i-1][1];
      index1++;
    }
    output[index1-1] = 2510;
    index1++;
    output[index1-1] = 2510;
    index1++;
}

///END OF MATLAB CODES. START OF EXTRA C CODES
void concLines1(int i,int j){
    flip(line1);
    copyLine(line,line1,0);
    addPosToLine(line, i, j);
    copyLine(line, line2, lengthOfLine(line1)+1);
}

void concLines(short int line1[20000][2],short int line2[20000][2]){
    int length1 = lengthOfLine(line1);
    int length2 = lengthOfLine(line2);
    copyLine(line1,line2,length1);
    line1[length1+length2][0] = 9999;
}

int lengthOfLine(short int line[20000][2]){
    int length = 0;
    while (line[length][0]!=9999)
      length++;
    return length;
}

int countOfLines(short int lines[2000][20000][2]){
    int count = 0;
    while (lines[count][0][0]!=9999)
      count++;
    return count;
}

void copyLine(short int dst[20000][2], short int src[20000][2], int index){
    int length = lengthOfLine(src);
    memcpy(dst+index, src, sizeof(short int)*2*length);
    dst[index + length][0] = 9999;
}

void copyLines(short int lines1[2000][20000][2], short int lines2[2000][20000][2]){
    int count = countOfLines(lines2);
    memcpy(lines1,lines2,sizeof(short int)*count*2*20000);
    lines1[count][0][0] = 9999;
}

void addPosToLine(short int line[20000][2], short int x,short int y){
    int length = lengthOfLine(line);
    line[length][0] = x;
    line[length][1] = y;
    line[length+1][0] = 9999;
}

void flip(short int line[20000][2]){
    int length = lengthOfLine(line);
    int i = 0;
    while (i<length/2){
      swap(&line[i][0],&line[length-i][0]);
      swap(&line[i][1],&line[length-i][1]);
      i++;
    }
}

void swap (short int* a,short int* b){
     short int t;
     t = *a;
    *a = *b;
    *b = t;
}

void imageReader(short int image[width][height]){    
     char number[10];
     int d;
     int digitIndex = 0;
     int posIndex1 = 0, posIndex2 = 0;
     double pos;
     
     FILE *fp = fopen("image.csv", "r");
     if (!fp) {
         printf("Can't open file\n");
      //   return 0;
      }        
        
      while(1){  
        while ( (d=getc(fp)) !=',' && d!=EOF){       
          if (d=='\n'){
            posIndex1++;
            posIndex2=0;
            continue;
          }
        number[digitIndex] = d;
        digitIndex++;
        }    
             
        if(d==EOF){
          break; 
        }
                  
      number[digitIndex++] = '\0';    
      digitIndex = 0;     
      image[posIndex1][posIndex2++] = (atoi(number)<255?1:0);   
      }
            
      fclose (fp);
  }
  

void outputWriter(short int output[], unsigned int count){
     unsigned int i = 0;
     FILE *fp = fopen("fotoxy.csv", "w");
     if (!fp) {
         printf("Can't open file\n");
      //   return 0;
      }        
        
     for (i=0; i<count; i++){
     //  printf("%hu",output[i]);
       fprintf (fp, "%hu%s", output[i], (i<count-1?",":"")); 
      }
      
      fclose (fp);
  }
