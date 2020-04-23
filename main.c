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
unsigned long photodiode_state = 0; // a save of the most recent photodiode state
unsigned char data_ready = 0; 			// flag to indicate that new data is ready to be processed
unsigned long count = 0;						// area count for the most recent photodiode state
unsigned long sum = 0;						  // running area sum
unsigned char newShape = 0;					// indicates if a new shape has been started

// Function Prototypes
void PortB_Init(void);
void PortE_Init(void);
void PortF_Init(void);
void updateLEDs(unsigned long);

/**
 * Pinout:
 *  PF2 - LED red
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
	unsigned long dataCopy;
	PortF_Init(); // Sets internal device pins
	PortB_Init(); // Sets external LED display device pins
	PortE_Init(); // Sets external phototransistor device pins
	EnableInterrupts();
		
	while(1)
	{
		// ****************************************************************************************************************************
		// ** Functionality 2: When no phototransistor (including the sampling one) is masked, the green LED on the Tiva must be on. **
		// ****************************************************************************************************************************
		if ((GPIO_PORTE_DATA_R & 0x3F) == 0x3F)   // If none of the photodiodes are masked (covered)
		{ 		
			GPIO_PORTF_DATA_R |= 0x08;							// Set the internal LED to green
		} else																		// If any of the photodiodes are masked (covered)
		{																			    
			GPIO_PORTF_DATA_R &= ~(0x08);						// Turn off the internal green LED
		}
		// ****************************************************************************************************************************

		// ******************************************************************************************************************************
		// ** Functionality 3: When all phototransistors (including the sampling one)  are masked, the red LED on the Tiva must be on. **
		// ******************************************************************************************************************************
		if ((GPIO_PORTE_DATA_R & 0x3F) == 0x00)   // If all of the photodiodes are masked (covered)
		{
			GPIO_PORTF_DATA_R |= 0x02; 					// Set the internal LED to red
		} else																		// If any of the photodiodes are not masked (covered)
		{
			GPIO_PORTF_DATA_R &= ~(0x02); 					// Turn off the internal red LED
		}
		// ******************************************************************************************************************************

			
		// ******************************************************************************************************
		// ** Functionality 4: Count and display the masked photodiodes whenever the sampling diode is masked. **
		// ******************************************************************************************************
		
		//if sample flag is passed, check saved port data
		if (data_ready)
		{
			//LEDCheck();
			data_ready = 0;
			dataCopy = photodiode_state; // save a local copy of the diode state, so the interrupt won't overwrite while we process
			
			if((dataCopy & 0x1F) == 0x1F && newShape == 0) // 0b11111 remainder (all unmasked) and we finished the previous shape
			{											
				sum = 0; newShape = 1; 						           // If photodiodes are all unmasked... we finished the shape!
			} 
			else{
				count = 0; dataCopy = dataCopy&0x1F; // "&=" to remove additional bits
				while (dataCopy)
				{
					if(dataCopy&1){
						count++;
					}
					dataCopy = dataCopy >> 1;
				}
				//updateLED(count & 15); //legacy code from functionality 4
				// ******************************************************************************************************
		
				// *********************************************************************
				// ** Functionality 5: Display the current surface area for an object **
				// *********************************************************************
			
				sum = (sum + (5-count)) & 15; //increase the sum and truncate to an 8-bit number
				updateLEDs(sum); newShape = 0;		  
				// *********************************************************************
				}
		}
	}
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
  GPIO_PORTE_IEV_R 	|= (0x20);    //     PE5 is a RISING edge event
  GPIO_PORTE_ICR_R 	|=  (0x20);     // (e) clear flag5
  GPIO_PORTE_IM_R 	|=  (0x20);     // (f) arm interrupt on PE5
  
  //Port E: interrupt number 4, vector number 20
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF1F)| 0x0000000A0; // (g) priority 5 for port E interrupts
  NVIC_EN0_R  = 0x00000010;      										   // (h) enable interrupt 4 in NVIC
}

void PortF_Init(void)
{ 
  // Internal devices: 
  //     Switches: PF4 :: SW1
  //     LED:      PF1, PF3 :: red LED, green LED
  volatile unsigned long delay;
  SYSCTL_RCGC2_R 			|= 0x00000020; // 1) activate F clock
  delay = SYSCTL_RCGC2_R;        	  // delay
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
	IN |= (num); 	      // Turn on LEDs corresponding to value
	GPIO_PORTB_DATA_R = IN;
}


// ***********************************************************************************
// ** FUNCTIONALITY 1: When Switch SW1 is pushed, all external LEDs must be set off **
// ***********************************************************************************

// Interrupt handler for Port F, which interfaces with the internal switch 1 for clearing
void GPIOPortF_Handler(void){
	// Copy the flags
	unsigned int flags_in = GPIO_PORTF_RIS_R; //save a copy of the interrupts
	
	// Scan the flags
	if ((flags_in & 0x10) >> 4) //flag4 - PF4 (SW1) triggered
	{
		GPIO_PORTF_ICR_R = 0x10; // Acknowledge PF4
		// Reset count and sum, clear external LEDs
		count = 0;
		sum = 0;
		updateLEDs(sum);
	}
}
// ***********************************************************************************


// Interrupt Handler for Port E, which interfaces with the sampling photodiode
void GPIOPortE_Handler(void){
	photodiode_state = GPIO_PORTE_DATA_R; 			// Save the current input state
	GPIO_PORTE_ICR_R   |=  (0x20); 							// Acknowledge the interrupt flag, PE5

	data_ready = 1; 														// Flag that new data is available
}
