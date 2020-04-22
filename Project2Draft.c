// COMP 6720 "Real-Time Embedded Systems"
// Project Phase 2 Draft
// Group 8: Ryan McWilliams & Stephanie Meyer
// April 20, 2020

// Define port base addresses: Port B
#define GPIO_PORTB_DATA_R       (*((volatile unsigned long *)0x400053FC))
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_PUR_R        (*((volatile unsigned long *)0x40005510))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
	
// Define port base addresses: Port E
#define GPIO_PORTE_DATA_R       (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))		
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_PUR_R        (*((volatile unsigned long *)0x40024510))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_LOCK_R       (*((volatile unsigned long *)0x40024520))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
#define GPIO_PORTE_CR_R         (*((volatile unsigned long *)0x40024524))
	
// Define interrupt addresses: Port E -- Needed for timely updates
#define GPIO_PORTE_IS_R         (*((volatile unsigned long *)0x40024404))
#define GPIO_PORTE_IBE_R        (*((volatile unsigned long *)0x40024408))
#define GPIO_PORTE_IEV_R        (*((volatile unsigned long *)0x4002440C))
#define GPIO_PORTE_ICR_R        (*((volatile unsigned long *)0x4002441C))
#define GPIO_PORTE_IM_R         (*((volatile unsigned long *)0x40024410))
#define GPIO_PORTE_RIS_R        (*((volatile unsigned long *)0x40024414))

// Define port base addresses: Port F
#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))

// Define interrupt addresses: Port F -- Needed for reset buttons
#define GPIO_PORTF_IS_R 		(*((volatile unsigned long *)0x40025404))
#define GPIO_PORTF_IBE_R 		(*((volatile unsigned long *)0x40025408))
#define GPIO_PORTF_IEV_R 		(*((volatile unsigned long *)0x4002540C))
#define GPIO_PORTF_ICR_R 		(*((volatile unsigned long *)0x4002541C))
#define GPIO_PORTF_IM_R 		(*((volatile unsigned long *)0x40025410))
#define GPIO_PORTF_RIS_R 		(*((volatile unsigned long *)0x40025414))
	
// Define Interrupt Priority and Enable Registers
#define NVIC_PRI1_R             (*((volatile unsigned long *)0xE000E404))
#define NVIC_PRI7_R             (*((volatile unsigned long *)0xE000E41C))
#define NVIC_EN0_R              (*((volatile unsigned long *)0xE000E100))


// Define the clock
#define SYSCTL_RCGC2_R      	(*((volatile unsigned long *)0x400FE108))

//Declare system functions
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

// Global Variables
unsigned long state;
unsigned long count = 0;
volatile unsigned long FallingEdges_PortF = 0;
volatile unsigned long RisingEdges_PortE = 0;

// Function Prototypes
void PortB_Init(void);
void PortE_Init(void);
void PortF_Init(void);
void LEDCheck(void);
void Delay(int e);
void getPhotoState(void);
void updateLEDs(unsigned long);

/**
 * Pinout:
 *  PF0 - SW2 (reset)
 *  PF2 - LED red 
 *  PF3 - LED green
 *  PF4 - SW1 (ADD)
 *
 *  PB0 - LED0 (LSB - GREEN 1)
 *  PB1 - LED1 (GREEN 2)
 *  PB2 - LED2 (YELLOW)
 *  PB3 - LED3 (RED)
 
 *  PE0 - "Sampling" Phototransistor
 *  PE1 - Sensing Phototransistor 0 (Nearest the Sampling Phototransitor)
 *  PE2 - Sensing Phototransistor 1
 *  PE3 - Sensing Phototransistor 2
 *  PE4 - Sensing Phototransistor 3
 *  PE5 - Sensing Phototransistor 4 (Farthest the Sampling Phototransistor)
**/

int main(void){		
	PortF_Init(); // Sets internal device pins
	PortB_Init(); // Sets external LED display device pins
	PortE_Init(); // Sets external phototransistor device pins
	EnableInterrupts();
	
	while(1)
	{
		if((GPIO_PORTE_DATA_R&0x1F) == 0){ 	// If none of the photodiodes are triggered
			GPIO_PORTF_DATA_R |= 0x08;				// Set the internal LED to green
		}
		else{
			GPIO_PORTF_DATA_R &= ~(0x08);			// Turn off the internal green LED
		}
	}
}

void LEDCheck(void){	
	// Make the LEDs blink three times
	
	GPIO_PORTB_DATA_R |= 0x0F; // Set (turn on PB0 PB1 PB2 PB3)
	GPIO_PORTF_DATA_R &= ~(0x0A); GPIO_PORTF_DATA_R |= 0x02; // Clear Internal, then set to red
	Delay(1);
	GPIO_PORTB_DATA_R ^= 0x0F; GPIO_PORTF_DATA_R &= ~(0x02); GPIO_PORTF_DATA_R |= (0x08); Delay(1);
	GPIO_PORTB_DATA_R ^= 0x0F; GPIO_PORTF_DATA_R &= ~(0x08); GPIO_PORTF_DATA_R |= (0x02); Delay(1);
	GPIO_PORTB_DATA_R ^= 0x0F; GPIO_PORTF_DATA_R &= ~(0x02); GPIO_PORTF_DATA_R |= (0x08); Delay(1);
	GPIO_PORTB_DATA_R ^= 0x0F; GPIO_PORTF_DATA_R &= ~(0x08); GPIO_PORTF_DATA_R |= (0x02); Delay(1);
	GPIO_PORTB_DATA_R ^= 0x0F; GPIO_PORTF_DATA_R &= ~(0x02); // Clear (turn off PB1 PB2 PB3 PF1 PF3)
}

void PortB_Init(void)
{ 
  // External Devices:
  //    Switches: PB4, PB5, PB6, PB7 :: pSW0, pSW1, pSW2, pSW3
  //    LED:      PB0, PB1, PB2, PB3 :: LED0, LED1, LED2, LED3
  volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000002;    // 1) activate B clock
  delay = SYSCTL_RCGC2_R;          // delay   
  GPIO_PORTB_AMSEL_R &= ~(0xFF);   // 3) disable analog function
  GPIO_PORTB_PCTL_R  &= ~(0xFF);   // 4) GPIO clear bit PCTL
  GPIO_PORTB_DIR_R   &= ~(0xF0);   // 5) Set i/o: 0 = input  (PB4, PB5, PB6, PB7)
  GPIO_PORTB_DIR_R   |=  (0x0F);   // 5) Set i/o: 1 = output (PB0, PB1, PB2, PB3)
  GPIO_PORTB_AFSEL_R &= ~(0xFF);   // 6) no alternate function
  //GPIO_PORTB_PUR_R |= 0xF0;      // 7) Only enable pull-up resistor for negative logic switches
  GPIO_PORTB_DEN_R   |=   0xFF;    // 8) Enable digital function (PB0 - 7)
}

void PortE_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000010;   // 1) activate E clock
	delay = SYSCTL_RCGC2_R;         // delay
//GPIO_PORTE_LOCK_R = 0x4C4F434B;	// 2) unlocking not necessary
	GPIO_PORTE_AMSEL_R &= ~(0xFF);	// 3) disable analog function
	GPIO_PORTE_PCTL_R  &= ~(0xFF);  // 4) GPIO clear bit PCTL
	GPIO_PORTE_DIR_R   &= ~(0x1F);	// 5) Set i/o: 0 = input  (PE0, PE1, PE2, PE3, PE4, PE5)
	GPIO_PORTE_AFSEL_R &= ~(0xFF);	// 6) no alternate function
	//GPIO_PORTE_PUR_R |= 0xF0;		// 7) Only enable pull-up resistor for negative logic switches
	GPIO_PORTE_DEN_R   |=   0xFF;	// 8) Enable digital function (PD0 - 7)

  GPIO_PORTE_IS_R &= ~0x01;     	// (d) PE0 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x01;    	//     PE0 is not both edges
  GPIO_PORTE_IEV_R |= 0x01;    		//     PE0 is a RISING edge event
  GPIO_PORTE_ICR_R = 0x01;      	// (e) clear flag0
  GPIO_PORTE_IM_R |= 0x01;      	// (f) arm interrupt on PE0
  
  
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFF00FFFF)|0x00A00000; // (g) priority 5 for PE0 // Double check "priority bits"
  NVIC_EN0_R = 0x00000010;      	// (h) enable interrupt 4 in NVIC
	
}

void PortF_Init(void)
{ 
  // Internal devices: 
  //     Switches: PF0, PF4 :: SW1, SW2
  //     LED:      PF1, PF3 :: red LED, green LED
  volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;  	// 1) activate F clock
  delay = SYSCTL_RCGC2_R;        	// delay
  FallingEdges_PortF = 0;
  GPIO_PORTF_LOCK_R = 0x4C4F434B;	// 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1B;        	// allow changes to PF0 PF1 PF3 PF4
  GPIO_PORTF_AMSEL_R &= ~(0x1B); 	// 3) disable analog function
  GPIO_PORTF_PCTL_R  &= ~(0x1B); 	// 4) GPIO clear bit PCTL
  GPIO_PORTF_DIR_R   &= ~(0x11); 	// 5) Set i/o:  0 = input (PF0, PF4)
  GPIO_PORTF_DIR_R   |=   0x0A;  	// 5) Set i/o:  1 = output (PF1, PF3)
  GPIO_PORTF_AFSEL_R &= ~(0x1B); 	// 6) no alternate function
  GPIO_PORTF_PUR_R   |=   0x11;  	// 7) Enable pull up on PF0, PF4
  GPIO_PORTF_DEN_R   |=   0x1B;  	// 8) Enable digital function (PF0, PF1, PF3, PF4)
  
  GPIO_PORTF_IS_R  &= ~0x11;     	// (d) PF4, PF0 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;    	//     PF4, PF0 is not both edges
  GPIO_PORTF_IEV_R &= ~0x11;    	//     PF4, PF0 falling edge event
  GPIO_PORTF_ICR_R  = 0x11;      	// (e) clear flag4, 0
  GPIO_PORTF_IM_R  |= 0x11;      	// (f) arm interrupt on PF4, PF0
  
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5 for PF4, PF0
  NVIC_EN0_R = 0x40000000;      	// (h) enable interrupt 30 in NVIC
  
}

//check external switch states and add to the global state
void getPhotoState(void)
{	
  unsigned long In;
  In  = GPIO_PORTE_DATA_R;	// Collect phototransistor state // How many bits is this, actually?
  In &= ~(0xE0); 			// Clear bits that we don't care about
  state = In;
}

void updateLEDs(unsigned long num){
	GPIO_PORTB_DATA_R &= ~(0xF); 	// Turn all LEDs off
	GPIO_PORTB_DATA_R |= (num&15); 	// Turn on LEDs corresponding to value
}

void GPIOPortF_Handler(void){
	// Copy the flags
	unsigned int flags_in = GPIO_PORTF_RIS_R; //save a copy of the interrupts
	// Scan flags to determine which subroutines must be run
	if (flags_in & 0x01) //flag0 - PF0 (SW2) triggered
	{
		// Do Nothing
		// You should de-arm this interrupt entirely
	}
	if ((flags_in & 0x10) >> 4) //flag4 - PF4 (SW1) triggered
	{
		GPIO_PORTF_ICR_R = 0x01; // Acknowledge PF0
		// Reset count, clear external LEDs
		count = 0;
		updateLEDs(count);
		
	}
	flags_in = GPIO_PORTF_RIS_R;
	if(flags_in){ // If flags remain, clear them
		GPIO_PORTF_ICR_R = 0x1F; // Clear all flags
	}
}

void GPIOPortE_Handler(void){
	unsigned long dataCopy = GPIO_PORTE_DATA_R;
	GPIO_PORTE_ICR_R |= 0x01; 							// Acknowledge the interrupt flag, PE0
	GPIO_PORTF_DATA_R &= ~(0x0A);						// Turn off the internal LED
	if ((GPIO_PORTE_DATA_R & 0x1F) == 0x1F){// Remainder of 0b11111 means that all are triggered
		GPIO_PORTF_DATA_R &= ~(0x0A);
		GPIO_PORTF_DATA_R |=  (0x02); 				// Set the internal LED to red
		
	}
	count = 0; dataCopy >>= 1; dataCopy &= 0x1F;
	while (dataCopy){
		if (dataCopy) {count++;}
		dataCopy >>= 1;
	}
	updateLEDs(count);
	Delay(3);
	updateLEDs(0); GPIO_PORTF_DATA_R &= ~(0x0A); // Close out
}

//Delay e * 100 ms
void Delay(int e)
{
  unsigned long i;
	while(e>0)
	{
		i = 1333333;  // this number means 100ms //TODO: MAKE ONE LOOP
		while(i > 0)
		{
		  i = i - 1;
		}
		e = e - 1;
	}
}
