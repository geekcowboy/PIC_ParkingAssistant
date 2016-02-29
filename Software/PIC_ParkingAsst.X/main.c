/*
 * File:   main.c
 *
 * Created on February 3, 2016, 8:13 PM
 * 
 * The Parking Assistant replaces the old "tennis ball on a string"
 * to help you properly park your car in a garage.
 */


// Include the necessary libraries
#include <xc.h>
#include <p18f2520.h>

#define _XTAL_FREQ 8000000      //Set clock frequency

//Configure pin B0 for PING sensor
#define pingUS LATBbits.LATB0
#define pingDir TRISBbits.TRISB0
#define pingIn PORTBbits.RB0

//Configure pin B1 for green LED
#define greenLED LATBbits.LATB1
#define greenLEDDir TRISBbits.TRISB1

//Configure pin B1 for yellow LED
#define yellowLED LATBbits.LATB2
#define yellowLEDDir TRISBbits.TRISB2

//Configure pin B1 for red LED
#define redLED LATBbits.LATB3
#define redLEDDir TRISBbits.TRISB3

//Define transition states
#define state_RED 0
#define state_YELLOW 1
#define state_GREEN 2

//Define distances for state transitions
#define dx1 80      //Entering: Yellow Off; Red On
#define dx2 140     //Exiting:  Yellow On; Red Off
#define dx3 275     //Entering: Green Off; Yellow On
#define dx4 305     //Exiting:  Green On; Yellow Off

//Set microcontroller configuration bits
#pragma config OSC = INTIO67
#pragma config WDT = OFF  
#pragma config LVP = OFF   
#pragma config PBADEN = OFF


void delay_timer(void);
int checkDistance(void);
void setGreenLight(void);
void setYellowLight(void);
void setRedLight(void);

/* Function: main
*
* Take a distance reading.
* Based on current state and distance reading
* determine if a state transition is necessary
*/
void main() {
    
    unsigned int state_CURRENT = state_RED;
    unsigned int distance = 0;
    
    ADCON1 = 0x0F;
    T0CON = 0x00;
    OSCCON = 0x72;
    CMCON = 0x07;
    INTCON2 = 0x80;
    
    TRISB = 0;
    TRISA = 0;
    LATB = 0;
    LATA = 0;
    
    greenLEDDir = 0;                        //GREEN, set output
    yellowLEDDir = 0;                       //YELLOW, set output
    redLEDDir = 0;                          //RED, set output
    
    distance = checkDistance();
   
    if (distance < dx2) {
        setRedLight();
        state_CURRENT = state_RED;
    }
    else if (distance > dx4) {
        setGreenLight();
        state_CURRENT = state_GREEN;
    }
    else {
        setYellowLight();
        state_CURRENT = state_YELLOW;      
    } 
    
    while(1) {
        distance = checkDistance();
        
        switch(state_CURRENT) {
            case state_RED:
                if (distance > dx2) {
                    setYellowLight();
                    state_CURRENT = state_YELLOW;
                }
                break;
            case state_YELLOW:
                if (distance <= dx1) {
                    setRedLight();
                    state_CURRENT = state_RED;
                }
                else if (distance > dx4) {
                    setGreenLight();
                    state_CURRENT = state_GREEN;
                }
                break;
            case state_GREEN:
                if (distance <= dx3) {
                    setYellowLight();
                    state_CURRENT = state_YELLOW;
                }
                break;
            default:
                setRedLight();
                state_CURRENT = state_RED;
                break;
        }
    }
    return;
}


void delay_timer() {
    for (int i = 0; i <2000; i++);
}



int checkDistance(void) {
    unsigned long Tm;
    unsigned char Tl, Th;
    unsigned int distance[10];                  // distance in cm  
    unsigned int avgDistance = 0;
    
    for (int ctr = 0; ctr < 10; ctr++) {
    
        TMR0H = 0;                          //Sets the Initial Value of Timer
        TMR0L = 0;                          //Sets the Initial Value of Timer
        
        pingDir = 0;                        //Sets RB0 as an output, PING pin
  
        pingUS = 0;                         //TRIGGER LOW
        delay_timer();
        //__delay_us(2);                    //TRIGGER HIGH
        pingUS = 1;  
        delay_timer(); 
        //__delay_us(5);                     //10uS Delay
        pingUS = 0;                         //TRIGGER LOW
        
        pingDir = 1;                        //Sets RB0 as an input
        while(pingIn==0);            //Waiting for Echo
        T0CONbits.TMR0ON = 1;               //Timer Starts                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
        while(pingIn==1);            //Waiting for Echo goes LOW
        T0CONbits.TMR0ON = 0;               //Timer Stops
        
        Tl = TMR0L;
        Th = TMR0H;
        Tm = Th * 256 + Tl;
        Tm = Tm/2;
        Tm = 34*Tm;
        Tm = Tm/1000;
        distance[ctr] = (unsigned)Tm;
   
        delay_timer();
        delay_timer();
        //delay_timer();
        //delay_timer();
        //delay_timer();
    }
    
    for (int ctr2 = 0; ctr2 < 10; ctr2++) {
        avgDistance += distance[ctr2];
    }
    
    avgDistance = avgDistance/10;
    return avgDistance;
}



void setGreenLight(void) {
greenLED = 1;
yellowLED = 0;
redLED = 0;   
}


void setYellowLight(void) {
    greenLED = 0;
    yellowLED = 1;
    redLED = 0;    
}

void setRedLight(void) {
    greenLED = 0;
    yellowLED = 0;
    redLED = 1;
}