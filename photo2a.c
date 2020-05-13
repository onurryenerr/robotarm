
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
constant int minLineLen = 12;
constant int minBlackToBegin = 8;
constant int minBlackToCont = 1;
constant int penTip = 1; 
constant int maxConnectionRange = 14;
//end of parameters to modify

constant int windowSideSize = 2*penTip+1;
#define numOfNeighbours 8*windowSideSize
constant int xMin = 3*penTip+2;
constant int xMax = width-(3*penTip+1);
constant int yMin = 3*penTip+2;
constant int yMax = height-(3*penTip+1);

short int image[width][height] = {0};
short int output[9999999] = {0};
short int line[20000][2] = {0};
short int line1[20000][2] = {0};
short int line2[20000][2] = {0};
short int neighbours[numOfNeighbours][2] = {0};
short int nexPoint[2] = {0};
unsigned int index = 0;

void imageReader(short int image[width][height]) ;
void outputWriter(short int output[], short int count) ;
/* Function Definitions */
void main(void)
{ 
  int numLines = 0;
  int i = 0; j = 0;
  
  static short int lines[2000][20000][2];
  static short int lines_connected[2000][20000][2];   
//  memset(&image[0][0], 0, width*height * sizeof(short int));
// imageReader(image);  
//  memset(&output[0], 0, 9999999 * sizeof(short int));
//  outputWriter(output);
  
  memset(&lines[0][0][0], 0, 2000*20000*2 * sizeof(short int));
  memset(&lines_connected[0][0][0], 0, 2000*20000*2 * sizeof(short int));
  
  
  output[index] = 2510; //lift the pen code
  index=index+1;
  output[index] = 2510;
  index=index+1;
  
  //for i = xMin:windowSideSize:xMax
  for (i=xMin;i<=xMax;i+=windowSideSize){
      for (j=yMin;j<=yMax;j+=windowSideSize){
        if (blackCounter(i,j) >= minBlackToBegin){
          visitedMarker(i,j);
          neighboursFinder(i,j); //assign to neighbours[][]
          biggestNeighbourFinder; //assign to nexPoint[2]
          lineExtractor (nextPoint[0],nextPoint[1],line1); //assign to line1
          biggestNeighbourFinder;
          lineExtractor (nextPoint[0],nextPoint[1],line2); //assign to line2
          concLines1(i,j)//line = [flip(line1,1); [i j]; line2];
          if (lengthOfLine(line)>=minLineLen){            
            addLine (lines,line,numLines);
            numLines = numLines + 1;
          }
          else
             recoverLine (line);
        }
      }
  }
  
  
  
}

/* End of code generation (transformerPi.c) */


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
      image[posIndex1][posIndex2++] = (short int)atoi(number);   
      
      }
            
      fclose (fp);
  }
  

void outputWriter(short int output[], short int count){  
     FILE *fp = fopen("fotoxy.csv", "w");
     if (!fp) {
         printf("Can't open file\n");
      //   return 0;
      }        
        
     for (i=0; i<count; i++){
       fprintf (fp, "%hu%s", output[i], (i<count-1?",":"")); 
      }
      
      fclose (fp);
  }
