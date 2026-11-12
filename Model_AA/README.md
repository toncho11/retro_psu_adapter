# Target

Model AA is primamry designed for the Vax station 3100 M38, but can be used for other computers that require a specific power good (DCOK) delay, protection and diagnostic. Diagnostic is via messages on the serial port from the Arduino.

# Current firmware functionality

sfsdfsdf

# How to build

* Print pcb
* Check BOM, order components
* solder electronic components
* check [warnings section](https://github.com/toncho11/retro_psu_adapter/blob/main/Model_AA/README.md#warnings)!!!
* compile and install firmware on the Arduino Nano
* tap into the 5V near the motherboard's connector and the GND - these are inputs on the PCB
* calibrate the TAP 5V - ...
* test before connecting to a motherboard

# Warnings

* WARNING: External ATX PSU may leave chassis (you computer's case) unearthed! You are probably using an external ATX PSU that it is not touching the computer's case. An internal PSU is grounded and it touches the case. This grounds the case as well. An external PSU changes that. The case might still be grounded through the motherboard's several ground lines, but a proper Protective Earth (PE) line is still needed between the case and the ATX PSU. 
* WARNING: External PSU may compromise internal computer cooling! In Vax 3100 M38 the fans of the PSU are responsible for cooling both the PSU and motherboard, HDDs, etc. Removing the old internal PSU removes this cooling mechanism. Not having an internal PSU means less heat is produced inside the case, but (some) cooling of the motherboard is still needed.
* WARNING: Arduino Nano can be powered on input pin +5VSB through the ATX PSU or through its USB port. Make sure you are not using both. This means that you might want to wait before soldering permanently the +5VSB line from the ATX to the Model AA adapter. 
* VERIFY: The Arduino will read ATX's POWER GOOD (PG) as HIGH if PG > 3.0V. Verify that your PG > 3.0V.

