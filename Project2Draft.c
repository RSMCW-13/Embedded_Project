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
	
// Define port base addresses: Port D
#define GPIO_PORTD_DATA_R       (*((volatile unsigned long *)0x400073FC))
#define GPIO_PORTD_DIR_R        (*((volatile unsigned long *)0x40007400))			
#define GPIO_PORTD_AFSEL_R      (*((volatile unsigned long *)0x40007420))
#define GPIO_PORTD_PUR_R        (*((volatile unsigned long *)0x40007510))
#define GPIO_PORTD_DEN_R        (*((volatile unsigned long *)0x4000751C))
#define GPIO_PORTD_LOCK_R       (*((volatile unsigned long *)0x40007520))
#define GPIO_PORTD_AMSEL_R      (*((volatile unsigned long *)0x40007528))
#define GPIO_PORTD_PCTL_R       (*((volatile unsigned long *)0x4000752C))
#define GPIO_PORTD_CR_R         (*((volatile unsigned long *)0x40007524))

// Define interrupt addresses: Port D -- Needed for timely updates
#define GPIO_PORTD_IS_R         (*((volatile unsigned long *)0x40007404))
#define GPIO_PORTD_IBE_R        (*((volatile unsigned long *)0x40007408))
#define GPIO_PORTD_IEV_R        (*((volatile unsigned long *)0x4000740C))
#define GPIO_PORTD_ICR_R        (*((volatile unsigned long *)0x4000741C))
#define GPIO_PORTD_IM_R         (*((volatile unsigned long *)0x40007410))
#define GPIO_PORTD_RIS_R        (*((volatile unsigned long *)0x40007414))

// Define port base addresses: Port F.
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
#define NVIC_PRI0_R             (*((volatile unsigned long *)0xE000E400))
#define NVIC_PRI7_R             (*((volatile unsigned long *)0xE000E41C))
#define NVIC_EN0_R              (*((volatile unsigned long *)0xE000E100))


// Define the clock
#define SYSCTL_RCGC2_R      	(*((volatile unsigned long *)0x400FE108))

//Declare system functions
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

// Global Variables
unsigned int state;
unsigned long count = 0;
volatile unsigned long FallingEdges_PortF = 0;
volatile unsigned long FallingEdges_PortD = 0;

// Function Prototypes
void PortB_Init(void);
void PortD_Init(void);
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
 
 *  PD0 - "Sampling" Phototransistor
 *  PD1 - Sensing Phototransistor 0 (Nearest the Sampling Phototransitor)
 *  PD2 - Sensing Phototransistor 1
 *  PD3 - Sensing Phototransistor 2
 *  PD6 - Sensing Phototransistor 3
 *  PD7 - Sensing Phototransistor 4 (Farthest the Sampling Phototransistor)
**/

int main(void){		
	PortF_Init(); // Sets internal device pins
	PortB_Init(); // Sets external LED display device pins
	PortD_Init(); // Sets external phototransistor device pins
	EnableInterrupts();
	
	LEDCheck(); // Do this to confirm that the LEDs do, in fact, light up when you write to their associated ports
	
	while(1)
	{
		Delay(3); count++;
		updateLEDs(count);	
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

void PortD_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000008;   // 1) activate B clock
	delay = SYSCTL_RCGC2_R;         // delay
	GPIO_PORTD_LOCK_R = 0x4C4F434B;	// 2) unlock PortD PD7
	GPIO_PORTD_AMSEL_R &= ~(0xFF);	// 3) disable analog function
	GPIO_PORTD_PCTL_R  &= ~(0xFF);  // 4) GPIO clear bit PCTL
	GPIO_PORTD_DIR_R   &= ~(0xCF);	// 5) Set i/o: 0 = input  (PD0, PD1, PD2, PD3, PD6, PD7)
	GPIO_PORTD_AFSEL_R &= ~(0xFF);	// 6) no alternate function
	//GPIO_PORTD_PUR_R |= 0xF0;		// 7) Only enable pull-up resistor for negative logic switches
	GPIO_PORTD_DEN_R   |=   0xFF;	// 8) Enable digital function (PD0 - 7)

  GPIO_PORTD_IS_R &= ~0x01;     	// (d) PD0 is edge-sensitive
  GPIO_PORTD_IBE_R &= ~0x01;    	//     PD0 is not both edges
  GPIO_PORTD_IEV_R &= ~0x01;    	//     PD0 falling edge event
  GPIO_PORTD_ICR_R = 0x01;      	// (e) clear flag0
  GPIO_PORTD_IM_R |= 0x01;      	// (f) arm interrupt on PD0
  
  
  NVIC_PRI0_R = (NVIC_PRI0_R&0xFF00FFFF)|0x00A00000; // (g) priority 5 for PD0
  NVIC_EN0_R = 0x00000008;      	// (h) enable interrupt 3 in NVIC
	
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
  
  GPIO_PORTF_IS_R &= ~0x11;     	// (d) PF4, PF0 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;    	//     PF4, PF0 is not both edges
  GPIO_PORTF_IEV_R &= ~0x11;    	//     PF4, PF0 falling edge event
  GPIO_PORTF_ICR_R = 0x11;      	// (e) clear flag4, 0
  GPIO_PORTF_IM_R |= 0x11;      	// (f) arm interrupt on PF4, PF0
  
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5 for PF4, PF0
  NVIC_EN0_R = 0x40000000;      	// (h) enable interrupt 30 in NVIC
  
}

//check external switch states and add to the global state
void getPhotoState(void)
{	
  unsigned long In;
  In  = GPIO_PORTD_DATA_R;	// Collect phototransistor state
  In &= 0xF0; 				// Clear bits that we don't care about
  state = In;
}

void updateLEDs(unsigned long num){
	GPIO_PORTB_DATA_R &= ~(0xF);
	GPIO_PORTB_DATA_R |= (num&15);
	//num&15
	//num&7
	//num&3
	//num&1
}

void GPIOPortF_Handler(void){
	// Copy the flags
	unsigned int flags_in = GPIO_PORTF_RIS_R; //save a copy of the interrupts
	// Scan flags to determine which subroutines must be run
	if ((flags_in & 0x10) >> 4) //flag4 - PF4 (SW1) triggered
	{
		GPIO_PORTF_ICR_R = 0x10;// Acknowledge PF0
		// Do something
		GPIO_PORTF_DATA_R &= ~(0x0A); Delay(1);
		GPIO_PORTF_DATA_R ^= (0x02); Delay(1);
		GPIO_PORTF_DATA_R ^= (0x02); Delay(1);
		GPIO_PORTF_DATA_R ^= (0x02); Delay(1);
		GPIO_PORTF_DATA_R ^= (0x02); Delay(1);
		GPIO_PORTF_DATA_R ^= (0x02); Delay(1);
		GPIO_PORTF_DATA_R &= ~(0x0A);
	}
	if (flags_in & 0x01) //flag0 - PF0 triggered
	{
		GPIO_PORTF_ICR_R = 0x01; // Acknowledge PF0
		//Clear all external LEDs
		count = 0;
		updateLEDs(count);
		
	}
	flags_in = GPIO_PORTF_RIS_R;
	if(flags_in){ // If flags remain, clear them
		GPIO_PORTF_ICR_R = 0x1F; // Clear all flags
	}
}

void GPIOPortD_Handler(void){
	volatile unsigned long int D_flags_in = GPIO_PORTD_RIS_R;
	GPIO_PORTD_ICR_R |= 0x01;
	LEDCheck();
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
