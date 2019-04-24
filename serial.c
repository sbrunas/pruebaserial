
//extern int   serialOpen      (const char *device, const int baud) ;
//extern void  serialClose     (const int fd) ;
//extern void  serialFlush     (const int fd) ;
//extern void  serialPutchar   (const int fd, const unsigned char c) ;
//extern void  serialPuts      (const int fd, const char *s) ;
//extern void  serialPrintf    (const int fd, const char *message, ...) ;
//extern int   serialDataAvail (const int fd) ;
//extern int   serialGetchar   (const int fd) ;

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringSerial.h>
#include <wiringPi.h>

/* Unsigned integer types  */
#define uint8_t unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned long
#define byte unsigned char
#define SOP '<'
#define EOP '>'
/*typedef enum {
	FALSE = 0, 
	TRUE = !FALSE
} bool;*/
bool started = FALSE;
bool ended = FALSE;
char inData[80];
byte indexx;
int val[2];
int i = 0;
uint32_t size = 0;
int fd, i ;
int32_t *ch0; // puntero para el buffer
int32_t *ch1;
static void ReadSerialData() ;
static void AtoiData() ;
static void boleanReset () ;
static void SaveData (int32_t col0, int32_t col1);
static void Datatobuffer(int dato1, int dato2);


//---------------------------------------------------------------------------------------------------------
//	name: ReadSerialData
//	function:  Read data comming from serial port
//	parameter: none
//	The return value:  NULL*/
//---------------------------------------------------------------------------------------------------------
static void ReadSerialData() {	
	char inChar = serialGetchar(fd) ; 
    if(inChar == SOP)
    {
       indexx = 0;
       inData[indexx] = '\0';
       started = TRUE;
       ended = FALSE;
    }
    else if(inChar == EOP)
    {
       ended = TRUE;
       //break;
    }
    else
    {
      if(indexx < 79)
      {
        inData[indexx] = inChar;
        indexx++;
        inData[indexx] = '\0';
      }
    }
}
//---------------------------------------------------------------------------------------------------------
//	name: AtoiData
//	function:  Convert char to int
//	parameter: none
//	The return value:  NULL*/
//---------------------------------------------------------------------------------------------------------
static void AtoiData(){
    char *Data=NULL;
  	if(started && ended){
   		Data =strtok(inData,",");
    	while(Data != NULL)
    	{  
      		//Data=strtok(NULL,",");
      		for (i=0;i<2;i++){
      			val[i] = atoi(Data);
      			Data = strtok(NULL,",");
      		}

    	}//while
 
    boleanReset ();
	}

}
//---------------------------------------------------------------------------------------------------------
//	name: boleanReset
//	function:  Reset flags
//	parameter: none
//	The return value:  NULL*/
//---------------------------------------------------------------------------------------------------------
static void boleanReset (){
//Reset
    started = FALSE;
    ended= FALSE;
    indexx = 0;
    inData[indexx]='\0';
}
//---------------------------------------------------------------------------------------------------------
//	name: SaveData
//	function:  Save data to txt
//	parameter: none
//	The return value:  NULL*/
//---------------------------------------------------------------------------------------------------------
static void SaveData (int32_t col0, int32_t col1){
	FILE *datos0 ;		
	datos0 = fopen("datos.txt", "a+") ; //fijate que utiliso a+
	fprintf(datos0,"dato 1 = %ld	dato 2 = %ld", col0, col1) ;
	fflush(stdout) ;
	fclose(datos0) ;
}
 //------------------------------------------------------------------------------------------------------------
static void Datatobuffer(int dato1, int dato2){
	ch0[size] = dato1 ;
	ch1[size] = dato2 ;
	size ++ ; 

}
int main(){

	
	int datacount = 100 ;//tamaño del buffer
	//TXT file open--------------------------------------------------------------------------------------------
	FILE *datos0 = NULL;	
	datos0 = fopen("datos.txt", "w"); // Creo que esa "w" la tienes en mayuscula

	
	//ch0 memory block-----------------------------------------------------------------------------------------
  	ch0 = calloc(datacount, sizeof(int32_t)) ; 
  	if(!ch0){
  		perror("Error allocating memory for channel 0");
    	abort();
  	}
  	//------------------------------------------------------------------------------------------------------------
	//ch1 memory block-----------------------------------------------------------------------------------------
  	ch1 = calloc(datacount, sizeof(int32_t)) ; 
  	if(!ch1){
  		perror("Error allocating memory for channel 0");
    	abort();
  	}
  	//------------------------------------------------------------------------------------------------------------
	
	if ((fd = serialOpen("/dev/ttyACM0", 9600)) < 0){
		
		return 1 ;
	}
	//------------------------------------------------------------------------------------------------------------
	while(1){
		//------GET DATA RECIEVE--------     
 		ReadSerialData();  
 		AtoiData(); 
 		Datatobuffer(val[0], val[1]);
 		if (size == datacount){
 			break ;
 		}
	}
	for (i=1; i < size; i++){
		SaveData(ch0[i], ch1[i]) ;
		}
	free(ch0);
	free(ch1);
	fclose(datos0);
	return 0 ;
}

	



