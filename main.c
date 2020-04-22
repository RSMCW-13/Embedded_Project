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
#define GPIO_PORTE_PDR_R        (*((volatile unsigned long *)0x40024514))
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
#define GPIO_PORTF_IS_R 				(*((volatile unsigned long *)0x40025404))
#define GPIO_PORTF_IBE_R 				(*((volatile unsigned long *)0x40025408))
#define GPIO_PORTF_IEV_R 				(*((volatile unsigned long *)0x4002540C))
#define GPIO_PORTF_ICR_R 				(*((volatile unsigned long *)0x4002541C))
#define GPIO_PORTF_IM_R 				(*((volatile unsigned long *)0x40025410))
#define GPIO_PORTF_RIS_R 				(*((volatile unsigned long *)0x40025414))
	
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
unsigned long photodiode_state = 0;
unsigned long count = 0;
unsigned long sum = 0;
unsigned char newShape = 0;

// Function Prototypes
void PortB_Init(void);
void PortE_Init(void);
void PortF_Init(void);
void LEDCheck(void);
void Delay(int e);
void updateLEDs(unsigned long);

/**
 * Pinout:
 *  PF2 - LED red  //TODO: not needed by project guidelines
 *  PF3 - LED green
 *  PF4 - SW1 (reset)
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
	
	LEDCheck(); //TODO: not needed by project guidelines
	
	while(1)
	{
		//TODO: if sample flag is passed, check saved port data
		if((GPIO_PORTE_DATA_R&0x1F) == 0x1F){ 		// If none of the photodiodes are masked (covered)
			GPIO_PORTF_DATA_R |= 0x08;							// Set the internal LED to green
		}
		else{																			// If any of the photodiodes are masked (covered)
			GPIO_PORTF_DATA_R &= ~(0x08);						// Turn off the internal green LED
		}
	}
}

void LEDCheck(void){	
	// Make the LEDs blink three times
	
	GPIO_PORTB_DATA_R |= 0x0F; // Set (turn on PB0 PB1 PB2 PB3)
	GPIO_PORTF_DATA_R &= ~(0x0A); GPIO_PORTF_DATA_R |= 0x02; // Clear Internal, then set to red //todo: red is not to spec
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
  //    LED:      PB0, PB1, PB2, PB3 :: LED0, LED1, LED2, LED3
  volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000002;    // 1) activate B clock
  delay = SYSCTL_RCGC2_R;          // delay   
  GPIO_PORTB_AMSEL_R &= ~(0x0F);   // 3) disable analog function
  GPIO_PORTB_PCTL_R  &= ~(0x0F);   // 4) GPIO clear bit PCTL
  GPIO_PORTB_DIR_R   |=  (0x0F);   // 5) Set i/o: 1 = output (PB0, PB1, PB2, PB3)
  GPIO_PORTB_AFSEL_R &= ~(0x0F);   // 6) no alternate function
  GPIO_PORTB_DEN_R   |=  (0x0F);    // 8) Enable digital function (PB0 - 3)
}

void PortE_Init(void){
	// External Devices:
	//		Photodiodes: PE0, PE1, PE2, PE3, PE4 :: sensing diodes, PE5 :: sampling diode
  volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000010;   // 1) activate E clock
  delay = SYSCTL_RCGC2_R;         // delay
  GPIO_PORTE_AMSEL_R &= ~(0x3F);	// 3) disable analog function
  GPIO_PORTE_PCTL_R  &= ~(0x3F);  // 4) GPIO clear bit PCTL
  GPIO_PORTE_DIR_R   &= ~(0x3F);	// 5) Set i/o: 0 = input  (PE0, PE1, PE2, PE3, PE4, PE5)
  GPIO_PORTE_AFSEL_R &= ~(0x3F);	// 6) no alternate function
  GPIO_PORTE_DEN_R   |=  (0x3F);  // 8) Enable digital function (PE0 - 5)

	// Interrupts:
  GPIO_PORTE_IS_R 	&= ~(0x20);     // (d) PE5 is edge-sensitive
  GPIO_PORTE_IBE_R 	&= ~(0x20);    	//     PE5 is not both edges
  GPIO_PORTE_IEV_R  &= ~(0x20);		  // Umm... maybe falling is rising? Since negative? //Photodiodes should have positive logic - 1 when activated, 0 when deactivated. are you sure??
  //GPIO_PORTE_IEV_R 	|= (0x20);    	//     PE5 is a RISING edge event
  GPIO_PORTE_ICR_R 	|=  (0x20);     // (e) clear flag5
  GPIO_PORTE_IM_R 	|=  (0x20);      	// (f) arm interrupt on PE5
  
  //Port E: interrupt number 4, vector number 20
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF1F)| 0x0000000A0; // (g) priority 5 for port E interrupts // TODO: test
  NVIC_EN0_R  = 0x00000010;      										   // (h) enable interrupt 4 in NVIC
}

void PortF_Init(void)
{ 
  // Internal devices: 
  //     Switches: PF4 :: SW1
  //     LED:      PF1, PF3 :: red LED, green LED //TODO: remove red LED
  volatile unsigned long delay;
  SYSCTL_RCGC2_R 			|= 0x00000020;  	// 1) activate F clock
  delay = SYSCTL_RCGC2_R;        	// delay
  //GPIO_PORTF_LOCK_R    = 0x4C4F434B;	// 2) unlock PortF PF0  
  //GPIO_PORTF_CR_R      = 0x01;        	// allow changes to PF0
  GPIO_PORTF_AMSEL_R  &= ~(0x1A); 	// 3) disable analog function
  GPIO_PORTF_PCTL_R   &= ~(0x1A); 	// 4) GPIO clear bit PCTL
  GPIO_PORTF_DIR_R    &= ~(0x10); 	// 5) Set i/o:  0 = input (PF4)
  GPIO_PORTF_DIR_R    |=  (0x0A);  	// 5) Set i/o:  1 = output (PF1, PF3)
  GPIO_PORTF_AFSEL_R  &= ~(0x1A); 	// 6) no alternate function
  GPIO_PORTF_PUR_R    |=  (0x10);  	// 7) Enable pull up on PF4
  GPIO_PORTF_DEN_R    |=  (0x1A);  	// 8) Enable digital function (PF1, PF3, PF4)
  
  GPIO_PORTF_IS_R     &= ~(0x10);     	// (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R    &= ~(0x10);    	  //     PF4 is not both edges
  GPIO_PORTF_IEV_R    &= ~(0x10);    	  //     PF4 falling edge event
  GPIO_PORTF_ICR_R    |=  (0x10);      	// (e) clear flag4
  GPIO_PORTF_IM_R     |=  (0x10);      	// (f) arm interrupt on PF4
  
  //Port F: interrupt number 30, vector number 46
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF1FFFFF)|0x00A00000; // (g) priority 5 for Port F interrupts
  NVIC_EN0_R = 0x40000000;      										 // (h) enable interrupt 30 in NVIC
}

// Updates the LED display to show the given number
void updateLEDs(unsigned long num){
	unsigned long IN = GPIO_PORTB_DATA_R; //Save the data for manipulation, to avoid flashing
	IN &= ~(0x0F); 	    // Turn all LEDs off
	IN |= (num & 15); 	// Turn on LEDs corresponding to value (truncated to a 4-bit number)
	GPIO_PORTB_DATA_R = IN;
}

// Interrupt handler for Port F, which interfaces with the internal switch 1 for clearing
void GPIOPortF_Handler(void){
	// Copy the flags
	unsigned int flags_in = GPIO_PORTF_RIS_R; //save a copy of the interrupts
	
	// Scan the flags
	if ((flags_in & 0x10) >> 4) //flag4 - PF4 (SW1) triggered
	{
		GPIO_PORTF_ICR_R = 0x10; // Acknowledge PF4
		// Reset count, clear external LEDs
		count = 0;
		updateLEDs(count);
	}
	flags_in = GPIO_PORTF_RIS_R;
	if(flags_in){ // If flags remain, clear them (this should be redundant)
		GPIO_PORTF_ICR_R |= 0x1F; // Clear all flags
	}
}

// Interrupt Handler for Port E, which interfaces with the sampling photodiode
void GPIOPortE_Handler(void){
	unsigned long dataCopy = GPIO_PORTE_DATA_R;
	GPIO_PORTE_ICR_R   |=  (0x20); 							// Acknowledge the interrupt flag, PE5
	GPIO_PORTF_DATA_R  &= ~(0x0A);						  // Turn off the internal LED //TODO: update to 0x08 for just the green?
	
	if((dataCopy & 0x1F) == 0x1F && newShape == 0){	// 0b11111 remainder (all unmasked) and we finished the previous shape										
		sum = 0; newShape = 1; 						// If photodiodes are all unmasked... we finished the shape!
		return;
	}
	else if((dataCopy & 0x1F) == 0){ 				// 0b00000 remainder (all masked)  
		GPIO_PORTF_DATA_R &= ~(0x0A);
		GPIO_PORTF_DATA_R |=  (0x02);
	}
	
	count = 0; dataCopy >>= 1; dataCopy &= 0x1F; // Shift to remove PE0, "&=" to remove additional bits
	while (dataCopy){
		if (dataCopy) {count++;}
		dataCopy >>= 1;
	}
	sum += (5 - count); updateLEDs(sum&15); newShape = 0;
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