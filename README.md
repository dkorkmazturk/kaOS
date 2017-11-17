# kaOS
A {chaotic} operating system that written for **TI TM4C123GXL** *(ARM Cortex-M4F)* in order to grasp every aspects of the MCU.

## What I have done so far
* Seperated user and kernel modes and seperated stack pointers for those modes.
* Round-Robin context switch using SysTick & PendSV
* System calls implemented via supervisor calls (SVC)
* Semaphores
* Thread sleeping
* Thread-safe hardware abstraction layer (HAL) for GPIO, UART and I²C peripherals.

## Work To-Do
* Proper watchdog timer implementation (Instead of kicking it every SysTick interrupt, which is useless)
* Extend HAL to other peripherals like ADC, SPI, CAN, etc.
* More testing

## License
This project is licensed under the GPLv3 License - see the LICENSE.md file for details.
