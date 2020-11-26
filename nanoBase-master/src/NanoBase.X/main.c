/*
/////////////////// DATOS DEL PROGRAMA ////////////////////

//  TTITULO: Water level controller
//  MICRO:PIC16F15244
//  ESTUDIANTES: Jorge Andres Herrera-Angie Paola Huertas
//  FECHA: 26 de noviembre de 2020

///////////// CONFIGURACI�N del MCU ////////////////// */

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <conio.h>


#pragma config FEXTOSC = OFF    // External Oscillator Mode Selection bits->Oscillator not enabled
#pragma config RSTOSC = HFINTOSC_1MHZ    // Power-up Default Value for COSC bits->HFINTOSC (1 MHz)
#pragma config CLKOUTEN = OFF    // Clock Out Enable bit->CLKOUT function is disabled; I/O function on RA4
#pragma config VDDAR = HI    // VDD Range Analog Calibration Selection bit->Internal analog systems are calibrated for operation between VDD = 2.3V - 5.5V

// CONFIG2
#pragma config MCLRE = EXTMCLR    // Master Clear Enable bit->If LVP = 0, MCLR pin is MCLR; If LVP = 1, RA3 pin function is MCLR
#pragma config PWRTS = PWRT_OFF    // Power-up Timer Selection bits->PWRT is disabled
#pragma config WDTE = OFF    // WDT Operating Mode bits->WDT disabled; SEN is ignored
#pragma config BOREN = ON    // Brown-out Reset Enable bits->Brown-out Reset Enabled, SBOREN bit is ignored
#pragma config BORV = LO    // Brown-out Reset Voltage Selection bit->Brown-out Reset Voltage (VBOR) set to 1.9V
#pragma config PPS1WAY = ON    // PPSLOCKED One-Way Set Enable bit->The PPSLOCKED bit can be cleared and set only once in software
#pragma config STVREN = ON    // Stack Overflow/Underflow Reset Enable bit->Stack Overflow or Underflow will cause a reset

// CONFIG4
#pragma config BBSIZE = BB512    // Boot Block Size Selection bits->512 words boot block size
#pragma config BBEN = OFF    // Boot Block Enable bit->Boot Block is disabled
#pragma config SAFEN = OFF    // SAF Enable bit->SAF is disabled
#pragma config WRTAPP = OFF    // Application Block Write Protection bit->Application Block is not write-protected
#pragma config WRTB = OFF    // Boot Block Write Protection bit->Boot Block is not write-protected
#pragma config WRTC = OFF    // Configuration Registers Write Protection bit->Configuration Registers are not write-protected
#pragma config WRTSAF = OFF    // Storage Area Flash (SAF) Write Protection bit->SAF is not write-protected
#pragma config LVP = ON    // Low Voltage Programming Enable bit->Low Voltage programming enabled. MCLR/Vpp pin function is MCLR. MCLRE Configuration bit is ignored.

// CONFIG5
#pragma config CP = OFF    // User Program Flash Memory Code Protection bit->User Program Flash Memory code protection is disabled

///////////// DEFINICIONES  //////////////////
#define _XTAL_FREQ 1000000
///////////// VARIABLES GLOBALES  //////////////////
int resultHigh,resultLow,estado;

///////////// DECLARACI�N DE FUNCIONES Y PROCEDIMIENTOS ///////////////////
void PIN_MANAGER_Initialize(void)
{
    // LATx registers
    LATA = 0x00;
    LATB = 0x00;
    LATC = 0x00;

    // TRISx registers
    TRISA = 0x10;
    TRISB = 0xF0;
    TRISC = 0xFF;

    // WPUx registers
    WPUB = 0x00;
    WPUA = 0x00;
    WPUC = 0x00;

    // ODx registers
    ODCONA = 0x00;
    ODCONB = 0x00;
    ODCONC = 0x00;

    // SLRCONx registers
    SLRCONA = 0x37;
    SLRCONB = 0xF0;
    SLRCONC = 0xFF;

    // INLVLx registers
    INLVLA = 0x3F;
    INLVLB = 0xF0;
    INLVLC = 0xFF;
   
    // ANSELx registers
    ANSELC = 0x00;
    ANSELB = 0x00;
    ANSELA = 0x00;


    // TRISx registers
    TRISA2 = 0;// Definiendo puerto A2 como salida digital
    TRISB4 = 0;// Definiendo como salida digital
    TRISC0 = 0;// Definiendo como salida digital
    TRISC1 = 0;// Definiendo como salida digital
    TRISC5 = 1;// Definiendo puerto C5 como entrada digital
    TRISC6 = 1;// Definiendo puerto C6 como entrada digital
    TRISC7 = 1;// Definiendo puerto C7 como entrada digital
    RA2PPS = 0x03;                                                              // RA2->PWM3:PWM3OUT;

}



void OSCILLATOR_Initialize(void)
{
    OSCEN = 0x00;                                                               // MFOEN disabled; LFOEN disabled; ADOEN disabled; HFOEN disabled;
    OSCFRQ = 0x00;                                                              // HFFRQ0 1_MHz
    OSCTUNE = 0x00;
}

void TMR2_Initialize(void)
{
    T2CLKCON = 0x01;                                                            // T2CS FOSC/4;  
    T2HLT = 0x00;                                                               // T2PSYNC Not Synchronized; T2MODE Software control; T2CKPOL Rising Edge; T2CKSYNC Not Synchronized;
    T2RST = 0x00;
    T2PR = 249;  
    T2TMR = 0x00;
    PIR1bits.TMR2IF = 0;                                                        // Clearing IF flag.
    T2CON = 0b10000000;                                                               // T2CKPS 1:1; T2OUTPS 1:1; TMR2ON on;
}

 void PWM3_Initialize(void)
 {    
    PWM3CON = 0x90;                                                             // PWM3POL active_low; PWM3EN enabled;
    PWM3DCH = 0x3E;    
    PWM3DCL = 0x40;  
 }
 




 void PWM3_LoadDutyValue(uint16_t dutyValue)
 {    
     PWM3DCH = (dutyValue & 0x03FC)>>2;                                         // Writing to 8 MSBs of PWM duty cycle in PWMDCH register    
     PWM3DCL = (dutyValue & 0x0003)<<6;                                         // Writing to 2 LSBs of PWM duty cycle in PWMDCL register
 }
/////////////  INICIO DEL PROGRAMA PRINCIPAL //////////////////////////


void MuyBajo(void)
{
    LATCbits.LATC0 = 1;
    LATCbits.LATC1 = 1;
    LATBbits.LATB4 = 1;
    __delay_ms(500);
    LATCbits.LATC0 = 0;
    LATCbits.LATC1 = 0;
    LATBbits.LATB4 = 0;
    __delay_ms(500);
    PWM3_LoadDutyValue(0);
}

void Bajo(void)
{
    LATCbits.LATC0 = 1;
    LATCbits.LATC1 = 0;
    LATBbits.LATB4 = 0;
    PWM3_LoadDutyValue(250);
    __delay_ms(100);
}

void Intermedio(void)
{
    LATCbits.LATC0 = 1;
    LATCbits.LATC1 = 1;
    LATBbits.LATB4 = 0;
    PWM3_LoadDutyValue(500);
    __delay_ms(100);
}

void Alto(void)
{
    LATCbits.LATC0 = 1;
    LATCbits.LATC1 = 1;
    LATBbits.LATB4 = 1;
    PWM3_LoadDutyValue(900);
    __delay_ms(100);
}

void Falla(void)
{
    LATCbits.LATC0 = 0;
    LATCbits.LATC1 = 0;
    LATBbits.LATB4 = 0;
    PWM3_LoadDutyValue(0);
    __delay_ms(100);
}

void Leer()
{
    if((PORTCbits.RC7==0)&&(PORTCbits.RC6==0)&&(PORTCbits.RC5==0))
    {
        MuyBajo();
    }
    else if((PORTCbits.RC7==1)&&(PORTCbits.RC6==0)&&(PORTCbits.RC5==0))
    {
        Bajo();
    }
    else if((PORTCbits.RC7==1)&&(PORTCbits.RC6==1)&&(PORTCbits.RC5==0))
    {
        Intermedio();
    }
    else if((PORTCbits.RC7==1)&&(PORTCbits.RC6==1)&&(PORTCbits.RC5==1))
    {
        Alto();
    }
    else{Falla();}
}

void main(void)
{
    PIN_MANAGER_Initialize();
    OSCILLATOR_Initialize();
    TMR2_Initialize();
    PWM3_Initialize();
    estado=0;
    while(1){
      Leer();
       __delay_ms(50);
    }
}