# Embedded Systems Project

## Goal
Develop a system capable of the following functionalities:
1. ~When Switch SW2 is pushed, all external LEDs must be set off. The external red LED is the MOST significant bit. You must use the interrupt mechanism.~
2. ~When no phototransistor (including the sampling one) is masked, the green LED on the Tiva must be on.~
3. ~When all phototransistors (including the sampling one) are masked, the red LED on the Tiva must be on.~
4. ~When I mask any n sensing phototransistors, the external LEDs will count by n whenever I wave a finger over the sampling phototransistor.~
5. If the previous functionalities are working, your surface area sensor is ready. Build a tray to repeatedly measure a 2cmx2cm dark square. Your sensor must repeatedly display the same reasonable count +/-1.

See the assignment Project-Phase2.docx for more details

## How to Use
Open Project2Draft.c and the accompanying startup.s in a Keil uVision project; compile and load to the Tiva. I re-used the C12_EdgeInterrupt directory and project because I received obscure errors when I tried to start a project from scratch.
## Current development state
It works. Ish. Beware an odd problem that will occur to me of bump the system while I am trying to use it: The LEDs will flicker continuously under light and this cannot be fixed with a restart; requires a reload from Keil altogether. Weird. Also my measuring tray is dirty and I can't tell if its picking up different shapes right or not.
## To Do
- Implement Interrupt code to interpret the data from Port E and light LEDs appropriately
- Clean up the code for submission (remove LEDCheck and Delay functions)