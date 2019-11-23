/////////////////////////////////////////////////////////////////////////
////                           myGlcd.c                              ////
////                                                                 ////
//// This file contains drivers for using a TopWay LM12864LFW with   ////
//// a KS0108 display controller. The LM12864LFW is 128 by 64 pixels.////
//// The driver treats the upper left pixel as (0,0).                ////
////                                                                 ////
//// See ex_glcd.c for suggested usage.                              ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// LCD Pin connections:                                            ////
//// (These can be changed as needed in the following defines).      ////
////  * 1: VSS is connected to GND                                   ////
////  * 2: VDD is connected to +5V                                   ////
////  * 3: V0  - LCD operating voltage (Constrast adjustment)        ////
////  * 4: D/I - Data or Instruction is connected to A0              ////
////  * 5: R/W - Read or Write is connected to A1                    ////
////  * 6: Enable is connected to A2                                 ////
////  *7-14: Data Bus 0 to 7 is connected to port d                  ////
////  *15: Chip Select 1 is connected to A3                          ////
////  *16: Chip Select 2 is connected to A4                          ////
////  *17: Reset is connected to A5                                  ////
////  *18: Negative voltage is also connected to the 50k Ohm POT     ////
////  *19: Positive voltage for LED backlight is connected to +5V    ////
////  *20: Negavtive voltage for LED backlight is connected to GND   ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////  glcd_init(mode)                                                ////
////     * Must be called before any other function.                 ////
////       - mode can be ON or OFF to turn the LCD on or off         ////
////                                                                 ////
////  glcd_pixel(x,y,color)                                          ////
////     * Sets the pixel to the given color.                        ////
////       - color can be ON or OFF                                  ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////many functions had the chip selection left after the end of them ////
////...however it's always set if needed in the beginning in many    ////
////functions the other ones that didn't have the function set are   ////
///left intentionally to use the settings of the previous ones       ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////to make the chip selection's understanding easier consider the   ////
////following naming convention                                      ////
////                                                                 ////
////functions that set the CS are "affetors"                         ////
////                                                                 ////
////functions that set the CS once they are called "shielded"        ////
////                                                                 ////
////functions that don't set the CS at all are "invisible" and most  ////
////of the time they want affectors before being used                ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////this code is written for the usage in both the PICC and MikroC   ////
////                                                                 ////
////PICC setup                                                       ////
////add this file to drivers folder then                             ////
////#define PICC                                                     ////
////#include "GLCD on picc.h"                                        ////
////#include <myGlcd.c>    //in the first 3 lines of the .c file     ////
////                                                                 ////
////MikroC setup                                                     ////
////add this file to the project folder then                         ////
////#define MikroC                                                   ////
////#include "myGlcd.c"    //in the first 2 lines of the .c file     ////
////write this line at the very first part of the main function      ////
////ADCON1 = 0x0F; CMCON  = 0x07; TRISA = 0x00; TRISD = 0x00;        ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////you'll probably have a problem with the read function in MikroC  ////
////if this problem wasn't solved in the MikroC version that you use ////
////then you've no option other than going to the assembly level and ////
////work out what i couldn't fix ... :S {anyway this is a problem    ////
////with compiling the code in MikroC itself ... not my problem =)}  ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////for tech support http://walidshouman.blogspot.com/				 ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////



#ifndef LM12864LFW
#define LM12864LFW

#ifndef GLCD_WIDTH
#define GLCD_WIDTH   128
#endif

#define GLCD_LEFT    0
#define GLCD_RIGHT   1

#ifndef ON
#define ON           1
#endif

#ifndef OFF
#define OFF          0
#endif

//////////helping macros to make the code portable
#ifdef MikroC

   //(here u can change the whole data port)
   #define GLCD_Data   PORTD         
   #define GLCD_Dir    TRISD 
   
   sbit GLCD_RS  at RA0_bit;
   sbit GLCD_RW  at RA1_bit;
   sbit GLCD_EN  at RA2_bit;
   sbit GLCD_CS1 at RA3_bit;
   sbit GLCD_CS2 at LATA4_bit;
   sbit GLCD_RST at RA5_bit;
   
   
   sbit GLCD_RS_Direction  at TRISA0_bit;
   sbit GLCD_RW_Direction  at TRISA1_bit;
   sbit GLCD_EN_Direction  at TRISA2_bit;
   sbit GLCD_CS1_Direction at TRISA3_bit;
   sbit GLCD_CS2_Direction at TRISA4_bit;
   sbit GLCD_RST_Direction at TRISA5_bit;
   
   //special Chip Selection macros
   #define CS1()        GLCD_CS1=1;
   #define CS1_Off()    GLCD_CS1=0;
   #define CS2()        GLCD_CS2=1;
   #define CS2_Off()    GLCD_CS2=0;
   
   
   //main inputs and outputs
   #define RS()      GLCD_RS=1;
   #define RS_Off()  GLCD_RS=0;   
   #define RW()      GLCD_RW=1;  
   #define RW_Off()  GLCD_RW=0;   
   #define RST()     GLCD_RST=1; 
   #define EN()      GLCD_EN=1;  
   #define EN_Off()  GLCD_EN=0;   
   
   //data 
   #define set_data_out(x)    GLCD_Data=x;     
   #define get_data_in()      GLCD_Data=0;       
   #define set_data_port(x)   GLCD_Dir=x;   
   
   //those functions aren't predefined in the MicroC
   #define bit_set(d,b) (d|=(1<<b))       //not revised
   #define bit_clear(d,b) (d&=~(1<<b))    //not revised
   typedef int BYTE; 
   typedef int int8;
   typedef int int1;

#endif


#ifdef PICC

   #define GLCD_CS1     PIN_A3
   #define GLCD_CS2     PIN_A4
   #define GLCD_RS      PIN_A0   // Data or Instruction input
   #define GLCD_RW      PIN_A1   // Read/Write
   #define GLCD_EN       PIN_A2   // Enable
   #define GLCD_RST     PIN_A5   // Reset
   
   //special Chip Selection macros
   #define CS1()        output_high(GLCD_CS1);
   #define CS1_Off()    output_low(GLCD_CS1);
   #define CS2()        output_high(GLCD_CS2);
   #define CS2_Off()    output_low(GLCD_CS2);
   
   
   //main inputs and outputs
   #define RS()      output_high(GLCD_RS);
   #define RS_Off()  output_low(GLCD_RS);   
   #define RW()      output_high(GLCD_RW);  
   #define RW_Off()  output_low(GLCD_RW);   
   #define RST()     output_high(GLCD_RST); 
   #define EN()      output_high(GLCD_EN);  
   #define EN_Off()  output_low(GLCD_EN);   
   
   //data (here u can change the whole data port)
   #define set_data_out(x)    output_d(x);     
   #define get_data_in()      input_d();       
   #define set_data_port(x)   set_tris_d(x);   

#endif



/////////////////////////////////////////////////////////////////////////
// Function Prototypes
/////////////////////////////////////////////////////////////////////////
void Enable_Pulse();                            //enable on then off with a 1us delay        //invisible
void Set_Start_Line(int line);                                                               //shielded
//int1 GOTO_COL(int x);                         //goto a certain column Y-Page   0 to 127    //shielded
//void GOTO_ROW(int y;)                         //goto a certain row X-Page      0 to 7      //invisible
void GOTO_XY(int x,int y);                      //goto anywhere in the screen                //shielded
//void GLCD_Clrln(int8 ln);                     //clears a whole line                        //shielded
//void GLCD_CLR();                              //clears the whole screen                    //shielded
void glcd_init(int1 mode);                      //initializing                               //shielded
void GLCD_Write(unsigned short b);              //write something                            //invisible
void glcd_pixel(int8 x, int8 y, int1 color);    //draw/erase pixel                           //shielded
void glcd_writeByte(int1 side, BYTE varData);   //write something in a certain chip          //shielded
BYTE glcd_read();                               //execute the read instruction               //invisible
//BYTE readAndReturn();                         //read what's in there at once               //invisible
/////////////////////////////////////////////////////////////////////////


void Enable_Pulse()
{
   EN()                                   //EN high
   delay_us(1);
   EN_Off()                               //EN low
   delay_us(1);
}

void Set_Start_Line(int line)
{
    RS_Off()                              //RS low --> command
    RW_Off()                              //RW low --> write
    CS1()                                 //are those needed 19-7-2012
    CS2()                                 //should be selected to affect both chips ... the simulation doesn't consider activating them a must
    EN_Off()                              //needed practically ... can be omitted for the usage in the simulation
    set_data_out(0xC0 | line);
    Enable_Pulse();
}


int1 GOTO_COL(int x)
{
   int side=0;                            //a default value to be changed if Chip 2 is selected
   int Col_Data;
   
   CS1_Off()                              //deselect both chips to select only one in the following if condition
   CS2_Off()
   
   RS_Off()                               //instruction
   RW_Off()                               //RW low --> write
   
   if(x<64)                               //left section     //should be the reverse!
   {
      CS1()                               //select chip 1
      Col_Data = x;                       //put column address on data port
   }
   else                                   //right section
   {
      CS2()
      Col_Data = x-64;                    //put column address on data port
      side=1;
   }
   Col_Data = (Col_Data | 0x40 ) & 0x7F;  //Command format 01 000000
   set_data_out(Col_Data)
   Enable_Pulse();
   
   return side;                           //if needed
}

void GOTO_ROW(int y)   
{
   int Col_Data;
   Col_Data = (y | 0xB8 ) & 0xBF;         //put row address on data port set command //leeh anding ya3ny!?? 10111000
                                          //y should be from 0 to 7 otherwise it will be ruined
   RS_Off()                               //RS low --> command
   RW_Off()                               //RW low --> write
   set_data_out(Col_Data)
   
   Enable_Pulse();
   
}


void GOTO_XY(int x,int y)
{
    GOTO_COL(x);
    GOTO_ROW(y);
}

void GLCD_Write(unsigned short b)
{
   RS()                                   //RS high --> data
   RW_Off()                               //RW low --> write
   
   set_data_out(b)                        //put data on data port
   delay_us(1);                           //is this needed?
   Enable_Pulse();
   RS_Off()                               //is this needed?
}


void GLCD_Clrln(int8 ln)
 {
    int k;
   
    GOTO_XY(0,ln);                        //test commenting the GOTO (it'll jump manually to the next X-Page's first byte)
    for(k=0;k<64;k++)    GLCD_Write(0x00);
    
  
    GOTO_XY(64,ln);
    for(k=0;k<64;k++)    GLCD_Write(0x00);
    
    CS1_Off()                             //being cautious deleting them ruins the lower half of the screen
    CS2_Off()
 }
 
 void GLCD_CLR()
{
   int8  m;                               //didn't work with unsigned short!!!! the PICC compiler has a problem defining the unsigned shorts within a function
   for(m=0;m<8;m++)
   {
    GLCD_Clrln(m);
   }
   
}


void glcd_init(int1 mode)
{  
   RST()
   RS_Off()                               //RS low --> command
   RW_Off()                               //RW low --> write
   
   if(mode)                               //on by default
      set_data_out(0x3f)                  //display on command 001111111
   else 
      set_data_out(0x3e)

   Enable_Pulse();

   Set_Start_Line(0);                     //important note: sending any instruction before the display On instruction is forbidden practically ... but works on the simulation
   GLCD_CLR();
    
}


void glcd_pixel(int8 col, int8 line, int1 color)
{

   BYTE varData;

   //////////goto the where to the point is
   GOTO_XY(col,line/8);                   //line/8 to get the page .. ie 50's page is floor(50/8)=6

   /////////read the previous value
   RS()                                   //set for data       //they aren't embedded in the read function to save 2 cycles ^^
   RW()                                   //Set for reading
   glcd_read();                           //the dummy read
   varData=glcd_read();

   ////////calculate the needd data to be written
   if(color == ON)
      bit_set(varData, line%8);           //Turn the pixel on
   else                                   //or
      bit_clear(varData, line%8);         //turn the pixel off

   ////////write the data
   GOTO_XY(col, line/8);               
   GLCD_Write(varData);                   //goto_xy provides the needed chip selection
   
   
}



// Purpose:    Write a byte of data to the specified chip
// Inputs:     1) chipSelect - which chip to write the data to
//             2) varData - the byte of data to write
void glcd_writeByte(int1 side, BYTE varData)
{
   if(side)                               //Choose which side to write to 0->left  1-> right
      CS2()
   else
      CS1()

   RW_Off()                               //Set for writing
   set_data_out(varData)                  //Put the data on the port
   Enable_Pulse();
}



/*int8 readAndReturn()
{
   int8 varData;
   
   //dummy read
   EN()                                   //Pulse the enable pin
   varData = get_data_in();               //Get the data from the display's output register
   EN_Off()
   
   
   //real read
   EN()                                   //manually Pulsate the enable pin
   varData = get_data_in()                //Get the data from the display's output register
   EN_Off()


   return varData;                        //Return the read data
}*/


// Purpose:    Reads a byte of data from the specified chip
// Ouputs:     A byte of data read from the chip

BYTE glcd_read()
{  
   BYTE varData;                          // Stores the data read from the LCD

   EN()                                   //manually Pulsate the enable pin
   varData = get_data_in()                //Get the data from the display's output register
   EN_Off()                               //try commenting this

   return varData;                        //Return the read data
}
