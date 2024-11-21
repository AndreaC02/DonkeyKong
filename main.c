/************************************Includes***************************************/
#include "./MultimodDrivers/multimod.h"

#include "./threads.h"


#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>
#include <driverlib/adc.h>
#include <driverlib/interrupt.h>
#include <G8RTOS/G8RTOS.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_types.h>
#include <inc/hw_memmap.h>
#include <inc/hw_i2c.h>
#include <inc/hw_gpio.h>

/************************************Includes***************************************/

/*************************************Defines***************************************/
/*************************************Defines***************************************/

/********************************Public Variables***********************************/
/********************************Public Variables***********************************/

/********************************Public Functions***********************************/
/********************************Public Functions***********************************/

/************************************MAIN*******************************************/

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    G8RTOS_Init();

    // Initializes the necessary peripherals.
    multimod_init();

    //Initialize FIFOs
    //UARTprintf("ADDING FIFOS\n");
    G8RTOS_InitFIFO(JOYSTICK_FIFO);


    //Add Semaphores
    //UARTprintf("ADDING SEMAPHORES THREADS\n");
    G8RTOS_InitSemaphore(&sem_I2CA, 1);
    G8RTOS_InitSemaphore(&sem_SPIA, 1);
    G8RTOS_InitSemaphore(&sem_PCA9555_Debounce, 0);

    //Add Background Threads
    //UARTprintf("ADDING BACKGROUND THREADS\n");
    G8RTOS_AddThread(Idle_Thread, 255, "idle\0");
    G8RTOS_AddThread(MarioMove_Thread, 1, "move_mario\0"); //1
    G8RTOS_AddThread(Read_Buttons, 1, "buttons\0");

    //Add Periodic Threads
    //UARTprintf("ADDING PERIODIC THREADS\n");
    G8RTOS_Add_PeriodicEvent(Get_Joystick, 5, 5); //100, 5
    G8RTOS_Add_PeriodicEvent(Draw_Screen, 2, 5);
    G8RTOS_Add_PeriodicEvent(Generate_Barrel, 2, 200);
    //G8RTOS_Add_PeriodicEvent(Check_Collision, 20, 6);


    //Add Aperiodic Threads
    //UARTprintf("ADDING APERIODIC THREADS\n");
    G8RTOS_Add_APeriodicEvent(GPIOE_Handler, 5, INT_GPIOE);


    G8RTOS_Launch();

    while (1);
}
