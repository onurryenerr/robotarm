
/* Include files */
#include "transformerPi.h"
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define  width 1050
#define  height 720*2

void positionsReader(short int arrayofPositions[]) ;
void trMatrixReader(float trMatrix[][height]) ;
void outputWriter(short int output[], short int count) ;

int offset = height/2;
//offset = offset/2;
int i=0;
int j=0;

/* Function Definitions */
void main(void)
{
  static short int output[9999999];
  static short int output2 [9999999];
  static short int positions[9999999];
  static float trMatrix[width][height];  
  //int b_index, b_index1, b_index2;  
  memset(&positions[0], 0, 9999999U * sizeof(short int));
  positionsReader(positions);  
  memset(&trMatrix[0][0], 0, 2*width*height * sizeof(float));
  trMatrixReader(trMatrix);

  ///* end */
  //b_index = 0;
  //while  (dv[b_index] !=  9999) {
    //printf("%hu ", dv[b_index]);
    //b_index++;
    //fflush(stdout);
  //}
  
   //b_index1 = 0;
  //b_index2 = 0;
  //while  (dv2[b_index1][b_index2] !=  999999) {
    //printf("%lf ", dv2[b_index1][b_index2]);
    //b_index2++;
    //if (b_index2 == width){
      //b_index2 = 0;
      //b_index1++;
      //}
    //fflush(stdout);
  //}
  
  while (positions[j] != 2530){
      if (positions[j] >= 2510){
        output[i] = positions[j]/10;
        i++; j++;
        output[i] = positions[j]/10;
        i++; j++;
      }
      else{
        short int x = positions[j];
        short int y = positions[j+1];
        double alpha = trMatrix[x][y];
        double betta = trMatrix[x][offset+y];
        output[i] = round (928.0 + ((1992.0-928.0)/180.0) * alpha);
        i++;
        output[i] = round (850.0 + ((1914.0-850.0)/180.0) * betta);
        i++; j+=2;
      }
      
      if(i>5 && (output[i-1]==output[i-3]) && (output[i-2]==output[i-4]))
        i=i-2;
  }
  
  output[i] = 253;
  i++;
  output[i] = 253;
  
  //for(j=0;j<=i;j++)
    //output2[j] = output[j];
    
  outputWriter(output, i+1);
        
}

/* End of code generation (transformerPi.c) */


void positionsReader(short int arrayofPositions[]){    
     char number[10];
     int d;
     int digitIndex = 0;
     int posIndex = 0;
     double pos;
     
     FILE *fp = fopen("../images/fotoxy.csv", "r");
     if (!fp) {
         printf("Can't open file\n");
      //   return 0;
      }        
        
      while(1){
      while ( (d=getc(fp)) !=',' && d!=EOF){
        number[digitIndex] = d;
        digitIndex++;
      }
             
      if(d==EOF){
         break; 
        }
          
        
      number[digitIndex++] = '\0';
      digitIndex = 0;  
      arrayofPositions[posIndex++] = (short int)atoi(number);     
      }
      
      arrayofPositions[posIndex] = 9999;
      
      fclose (fp);
  }
  
  
void trMatrixReader(float trMatrix[][height]){ 
   // printf("teze girdik\n");   
     char number[10];
     int d;
     int digitIndex = 0;
     int posIndex1 = 0, posIndex2 = 0;
     double pos;
     
     FILE *fp = fopen("paper.csv", "r");
     if (!fp) {
         printf("Can't open file\n");
      //   return 0;
      }        
       
     //printf("a1\n");   
        
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
     //   printf("a8\n"); 
         break; 
        }
                
      number[digitIndex++] = '\0';
    
      digitIndex = 0;
     
      trMatrix[posIndex1][posIndex2++] = atof(number);
         
      }
            
      trMatrix[posIndex1][posIndex2] = 999999;
      fclose (fp);
  }


void outputWriter(short int output[], short int count){  
     FILE *fp = fopen("../images/fotoab.csv", "w");
     if (!fp) {
         printf("Can't open file\n");
      //   return 0;
      }        
        
     for (i=0; i<count; i++){
       fprintf (fp, "%hu%s", output[i], (i<count-1?",":"")); 
      }
      
      fclose (fp);
  }
